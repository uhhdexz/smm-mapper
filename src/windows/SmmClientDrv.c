#include <ntddk.h>
#include "SmmClient.h"

static UNICODE_STRING gDosName;
static PDEVICE_OBJECT gDeviceObject;

static unsigned long long FnV1a64(const void *Buffer, unsigned int Size) {
  const unsigned char *Data = (const unsigned char *)Buffer;
  unsigned long long Hash = 0xCBF29CE484222325ULL;
  while (Size--) {
    Hash ^= *Data++;
    Hash *= 0x100000001B3ULL;
  }
  return Hash;
}

static NTSTATUS WaitForSmm(MAILBOX *Mailbox, unsigned int TimeoutMs) {
  LARGE_INTEGER Delay;
  unsigned int Elapsed = 0;

  Delay.QuadPart = -10000;
  if (TimeoutMs == 0) {
    TimeoutMs = 2000;
  }
  while (Elapsed < TimeoutMs) {
    if (Mailbox->Command == COMMAND_NONE &&
        Mailbox->Status != STATUS_BUSY) {
      return STATUS_SUCCESS;
    }
    KeDelayExecutionThread(KernelMode, FALSE, &Delay);
    Elapsed++;
  }
  return STATUS_IO_TIMEOUT;
}

static NTSTATUS ExecuteRequest(REQUEST *Request,
                               unsigned long InputLength,
                               unsigned long *OutputLength) {
  PHYSICAL_ADDRESS Physical;
  MAILBOX *Mailbox;
  unsigned int Command;
  NTSTATUS NtStatus;

  *OutputLength = REQUEST_HEADER_SIZE;
  if (Request == NULL || InputLength < REQUEST_HEADER_SIZE ||
      Request->MailboxPhysical == 0 ||
      Request->MailboxSize < MAILBOX_TOTAL_SIZE) {
    return STATUS_INVALID_PARAMETER;
  }

  Command = Request->Command;
  if (Command != COMMAND_PING && Command != COMMAND_STATUS &&
      Command != COMMAND_UNLOAD && Command != COMMAND_RELOAD) {
    return STATUS_INVALID_PARAMETER;
  }
  if (Command == COMMAND_RELOAD) {
    if (Request->PayloadSize == 0 ||
        Request->PayloadSize > MAILBOX_PAYLOAD_CAPACITY ||
        InputLength < REQUEST_HEADER_SIZE + Request->PayloadSize) {
      return STATUS_INVALID_PARAMETER;
    }
  }

  Physical.QuadPart = Request->MailboxPhysical;
  Mailbox = (MAILBOX *)MmMapIoSpace(
      Physical, Request->MailboxSize, MmNonCached);
  if (Mailbox == NULL) {
    return STATUS_INSUFFICIENT_RESOURCES;
  }

  NtStatus = STATUS_SUCCESS;
  if (Mailbox->Magic != MAILBOX_MAGIC ||
      Mailbox->Version != MAILBOX_VERSION ||
      Mailbox->HeaderSize < sizeof(MAILBOX) ||
      Mailbox->TotalSize < MAILBOX_TOTAL_SIZE ||
      Mailbox->PayloadOffset + MAILBOX_PAYLOAD_CAPACITY >
          Mailbox->TotalSize) {
    NtStatus = STATUS_INVALID_DEVICE_STATE;
    goto Done;
  }

  if (Command == COMMAND_RELOAD) {
    unsigned char *PayloadBase =
        (unsigned char *)Mailbox + Mailbox->PayloadOffset;
    RtlCopyMemory(PayloadBase, Request->Payload, Request->PayloadSize);
    Mailbox->PayloadSize = Request->PayloadSize;
    Mailbox->PayloadHash = FnV1a64(Request->Payload, Request->PayloadSize);
  }

  Mailbox->Result = 0;
  Mailbox->Status = STATUS_BUSY;
  Mailbox->Command = Command;
  Mailbox->Sequence++;
  KeMemoryBarrier();

  WRITE_PORT_UCHAR((PUCHAR)(ULONG_PTR)0xB2,
                   (UCHAR)(Request->SwSmiValue ?
                               Request->SwSmiValue :
                               Mailbox->SwSmiValue));

  NtStatus = WaitForSmm(Mailbox, Request->TimeoutMs);
  Request->Status = Mailbox->Status;
  Request->Loaded = Mailbox->Loaded;
  Request->Generation = Mailbox->Generation;
  Request->Result = Mailbox->Result;
  Request->Sequence = Mailbox->Sequence;

Done:
  MmUnmapIoSpace(Mailbox, Request->MailboxSize);
  return NtStatus;
}

static NTSTATUS DispatchCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
  (void)DeviceObject;
  Irp->IoStatus.Status = STATUS_SUCCESS;
  Irp->IoStatus.Information = 0;
  IoCompleteRequest(Irp, IO_NO_INCREMENT);
  return STATUS_SUCCESS;
}

static NTSTATUS DispatchDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
  PIO_STACK_LOCATION Stack;
  NTSTATUS Status;
  unsigned long OutputLength = 0;
  (void)DeviceObject;

  Stack = IoGetCurrentIrpStackLocation(Irp);
  if (Stack->Parameters.DeviceIoControl.IoControlCode == IOCTL_EXECUTE) {
    Status = ExecuteRequest((REQUEST *)Irp->AssociatedIrp.SystemBuffer,
                            Stack->Parameters.DeviceIoControl.InputBufferLength,
                            &OutputLength);
  } else {
    Status = STATUS_INVALID_DEVICE_REQUEST;
  }

  Irp->IoStatus.Status = Status;
  Irp->IoStatus.Information = NT_SUCCESS(Status) ? OutputLength : 0;
  IoCompleteRequest(Irp, IO_NO_INCREMENT);
  return Status;
}

static void DriverUnload(PDRIVER_OBJECT DriverObject) {
  (void)DriverObject;
  IoDeleteSymbolicLink(&gDosName);
  if (gDeviceObject != NULL) {
    IoDeleteDevice(gDeviceObject);
  }
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject,
                     PUNICODE_STRING RegistryPath) {
  UNICODE_STRING NtName;
  NTSTATUS Status;
  (void)RegistryPath;

  RtlInitUnicodeString(&NtName, NT_DEVICE_NAME);
  RtlInitUnicodeString(&gDosName, DOS_DEVICE_NAME);
  Status = IoCreateDevice(DriverObject, 0, &NtName, FILE_DEVICE_UNKNOWN, 0,
                          FALSE, &gDeviceObject);
  if (!NT_SUCCESS(Status)) {
    return Status;
  }
  Status = IoCreateSymbolicLink(&gDosName, &NtName);
  if (!NT_SUCCESS(Status)) {
    IoDeleteDevice(gDeviceObject);
    gDeviceObject = NULL;
    return Status;
  }

  DriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchCreateClose;
  DriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchCreateClose;
  DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchDeviceControl;
  DriverObject->DriverUnload = DriverUnload;
  return STATUS_SUCCESS;
}