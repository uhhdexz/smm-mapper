#include <intrin.h>

#pragma intrinsic(__inbyte)
#pragma intrinsic(__outbyte)
#pragma function(memset)
#pragma function(memcpy)

typedef unsigned char UINT8;
typedef unsigned char BOOLEAN;
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef unsigned long long UINT64;
typedef unsigned long long UINTN;
typedef unsigned long long size_t;
typedef long long INTN;
typedef unsigned short CHAR16;
typedef void VOID;
typedef VOID *EFI_HANDLE;
typedef VOID *EFI_EVENT;
typedef UINT64 EFI_STATUS;
typedef UINTN EFI_TPL;
typedef UINT64 EFI_PHYSICAL_ADDRESS;

#define EFIAPI __cdecl

#define EFI_SUCCESS 0
#define EFI_INVALID_PARAMETER 2
#define EFI_OUT_OF_RESOURCES 9
#define EFI_NOT_FOUND 14
#define EFI_ALREADY_STARTED 18
#define EFI_ERROR(Status) ((Status) != EFI_SUCCESS)

#define EVT_TIMER 0x80000000U
#define EVT_NOTIFY_SIGNAL 0x00000200U
#define TPL_CALLBACK 8U
#define COM1_PORT 0x3F8
#define PAYLOAD_FILE_LIMIT (256U * 1024U)
#define VERBOSE 1
#define COMM_MAGIC 0x56444D53U
#define COMM_VERSION 4U
#define COMM_HEADER_SIZE 32U
#define MAILBOX_MAGIC 0x58425645444D4D53ULL
#define MAILBOX_VERSION 1U
#define MAILBOX_HEADER_SIZE 0x1000U
#define MAILBOX_PAYLOAD_CAPACITY (256U * 1024U)
#define MAILBOX_TOTAL_SIZE \
  (MAILBOX_HEADER_SIZE + MAILBOX_PAYLOAD_CAPACITY)
#define SW_SMI_VALUE 0xD5U
#define STATUS_IDLE 0U
#define EFI_VARIABLE_NON_VOLATILE 0x00000001U
#define EFI_VARIABLE_BOOTSERVICE_ACCESS 0x00000002U
#define EFI_VARIABLE_RUNTIME_ACCESS 0x00000004U
#define EFI_RESERVED_MEMORY_TYPE 0U
#define EFI_RUNTIME_SERVICES_DATA 6U
#define EFI_CONVENTIONAL_MEMORY 7U
#define EFI_ACPI_MEMORY_NVS 10U

typedef struct {
  UINT32 Data1;
  UINT16 Data2;
  UINT16 Data3;
  UINT8 Data4[8];
} EFI_GUID;

typedef struct {
  UINT64 Signature;
  UINT32 Revision;
  UINT32 HeaderSize;
  UINT32 CRC32;
  UINT32 Reserved;
} EFI_TABLE_HEADER;

typedef struct {
  EFI_GUID VendorGuid;
  VOID *VendorTable;
} EFI_CONFIGURATION_TABLE;

typedef struct {
  UINT32 Type;
  UINT32 Pad;
  UINT64 PhysicalStart;
  UINT64 VirtualStart;
  UINT64 NumberOfPages;
  UINT64 Attribute;
} EFI_MEMORY_DESCRIPTOR;

typedef struct {
  UINT32 Version;
  UINT32 NumberOfEntries;
  UINT32 DescriptorSize;
  UINT32 Reserved;
} EDKII_PI_SMM_COMMUNICATION_REGION_TABLE;

typedef struct EFI_BOOT_SERVICES EFI_BOOT_SERVICES;
typedef struct EFI_RUNTIME_SERVICES EFI_RUNTIME_SERVICES;
typedef struct EFI_SYSTEM_TABLE EFI_SYSTEM_TABLE;
typedef struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;
typedef struct EFI_FILE_PROTOCOL EFI_FILE_PROTOCOL;
typedef struct EFI_SMM_COMMUNICATION_PROTOCOL EFI_SMM_COMMUNICATION_PROTOCOL;

typedef EFI_STATUS(EFIAPI *EFI_LOCATE_HANDLE_BUFFER)(
    UINTN SearchType, EFI_GUID *Protocol, VOID *SearchKey, UINTN *NoHandles,
    EFI_HANDLE **Buffer);
typedef EFI_STATUS(EFIAPI *EFI_LOCATE_PROTOCOL)(EFI_GUID *Protocol,
                                                VOID *Registration,
                                                VOID **Interface);
typedef EFI_STATUS(EFIAPI *EFI_HANDLE_PROTOCOL)(EFI_HANDLE Handle,
                                                EFI_GUID *Protocol,
                                                VOID **Interface);
typedef EFI_STATUS(EFIAPI *EFI_FREE_POOL)(VOID *Buffer);
typedef enum {
  AllocateAnyPages,
  AllocateMaxAddress,
  AllocateAddress
} EFI_ALLOCATE_TYPE;
typedef EFI_STATUS(EFIAPI *EFI_ALLOCATE_PAGES)(EFI_ALLOCATE_TYPE Type,
                                               UINT32 MemoryType,
                                               UINTN Pages,
                                               EFI_PHYSICAL_ADDRESS *Memory);
typedef VOID(EFIAPI *EFI_EVENT_NOTIFY)(EFI_EVENT Event, VOID *Context);
typedef enum {
  TimerCancel,
  TimerPeriodic,
  TimerRelative
} EFI_TIMER_DELAY;
typedef EFI_STATUS(EFIAPI *EFI_CREATE_EVENT)(UINT32 Type, EFI_TPL NotifyTpl,
                                             EFI_EVENT_NOTIFY NotifyFunction,
                                             VOID *NotifyContext,
                                             EFI_EVENT *Event);
typedef EFI_STATUS(EFIAPI *EFI_SET_TIMER)(EFI_EVENT Event,
                                          EFI_TIMER_DELAY Type,
                                          UINT64 TriggerTime);
typedef EFI_STATUS(EFIAPI *EFI_CLOSE_EVENT)(EFI_EVENT Event);
typedef EFI_STATUS(EFIAPI *EFI_REGISTER_PROTOCOL_NOTIFY)(EFI_GUID *Protocol,
                                                         EFI_EVENT Event,
                                                         VOID **Registration);
typedef EFI_STATUS(EFIAPI *EFI_SET_VARIABLE)(CHAR16 *VariableName,
                                             EFI_GUID *VendorGuid,
                                             UINT32 Attributes,
                                             UINTN DataSize, VOID *Data);

struct EFI_BOOT_SERVICES {
  EFI_TABLE_HEADER Hdr;
  VOID *RaiseTPL;
  VOID *RestoreTPL;
  EFI_ALLOCATE_PAGES AllocatePages;
  VOID *FreePages;
  VOID *GetMemoryMap;
  VOID *AllocatePool;
  EFI_FREE_POOL FreePool;
  EFI_CREATE_EVENT CreateEvent;
  EFI_SET_TIMER SetTimer;
  VOID *WaitForEvent;
  VOID *SignalEvent;
  EFI_CLOSE_EVENT CloseEvent;
  VOID *CheckEvent;
  VOID *InstallProtocolInterface;
  VOID *ReinstallProtocolInterface;
  VOID *UninstallProtocolInterface;
  EFI_HANDLE_PROTOCOL HandleProtocol;
  VOID *Reserved;
  EFI_REGISTER_PROTOCOL_NOTIFY RegisterProtocolNotify;
  VOID *LocateHandle;
  VOID *LocateDevicePath;
  VOID *InstallConfigurationTable;
  VOID *LoadImage;
  VOID *StartImage;
  VOID *Exit;
  VOID *UnloadImage;
  VOID *ExitBootServices;
  VOID *GetNextMonotonicCount;
  VOID *Stall;
  VOID *SetWatchdogTimer;
  VOID *ConnectController;
  VOID *DisconnectController;
  VOID *OpenProtocol;
  VOID *CloseProtocol;
  VOID *OpenProtocolInformation;
  VOID *ProtocolsPerHandle;
  EFI_LOCATE_HANDLE_BUFFER LocateHandleBuffer;
  EFI_LOCATE_PROTOCOL LocateProtocol;
  VOID *InstallMultipleProtocolInterfaces;
  VOID *UninstallMultipleProtocolInterfaces;
  VOID *CalculateCrc32;
  VOID *CopyMem;
  VOID *SetMem;
  VOID *CreateEventEx;
};

struct EFI_RUNTIME_SERVICES {
  EFI_TABLE_HEADER Hdr;
  VOID *GetTime;
  VOID *SetTime;
  VOID *GetWakeupTime;
  VOID *SetWakeupTime;
  VOID *SetVirtualAddressMap;
  VOID *ConvertPointer;
  VOID *GetVariable;
  VOID *GetNextVariableName;
  EFI_SET_VARIABLE SetVariable;
};

struct EFI_SYSTEM_TABLE {
  EFI_TABLE_HEADER Hdr;
  CHAR16 *FirmwareVendor;
  UINT32 FirmwareRevision;
  EFI_HANDLE ConsoleInHandle;
  VOID *ConIn;
  EFI_HANDLE ConsoleOutHandle;
  VOID *ConOut;
  EFI_HANDLE StandardErrorHandle;
  VOID *StdErr;
  EFI_RUNTIME_SERVICES *RuntimeServices;
  EFI_BOOT_SERVICES *BootServices;
  UINTN NumberOfTableEntries;
  EFI_CONFIGURATION_TABLE *ConfigurationTable;
};

typedef EFI_STATUS(EFIAPI *EFI_FILE_OPEN)(EFI_FILE_PROTOCOL *This,
                                          EFI_FILE_PROTOCOL **NewHandle,
                                          CHAR16 *FileName, UINT64 OpenMode,
                                          UINT64 Attributes);
typedef EFI_STATUS(EFIAPI *EFI_FILE_CLOSE)(EFI_FILE_PROTOCOL *This);
typedef EFI_STATUS(EFIAPI *EFI_FILE_READ)(EFI_FILE_PROTOCOL *This,
                                          UINTN *BufferSize, VOID *Buffer);

struct EFI_FILE_PROTOCOL {
  UINT64 Revision;
  EFI_FILE_OPEN Open;
  EFI_FILE_CLOSE Close;
  VOID *Delete;
  EFI_FILE_READ Read;
  VOID *Write;
  VOID *GetPosition;
  VOID *SetPosition;
  VOID *GetInfo;
  VOID *SetInfo;
  VOID *Flush;
};

typedef EFI_STATUS(EFIAPI *EFI_OPEN_VOLUME)(
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *This, EFI_FILE_PROTOCOL **Root);

struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL {
  UINT64 Revision;
  EFI_OPEN_VOLUME OpenVolume;
};

typedef EFI_STATUS(EFIAPI *EFI_SMM_COMMUNICATE)(
    const EFI_SMM_COMMUNICATION_PROTOCOL *This, VOID *CommBuffer,
    UINTN *CommSize);

struct EFI_SMM_COMMUNICATION_PROTOCOL {
  EFI_SMM_COMMUNICATE Communicate;
};

#pragma pack(push, 1)
typedef struct {
  EFI_GUID HeaderGuid;
  UINTN MessageLength;
  UINT8 Data[1];
} EFI_SMM_COMMUNICATE_HEADER;

typedef struct {
  UINT32 Magic;
  UINT32 Version;
  UINT32 Command;
  UINT32 PayloadSize;
  UINT64 MailboxPhysical;
  UINT32 MailboxSize;
  UINT32 SwSmiValue;
  UINT8 Payload[1];
} COMM_MESSAGE;
#pragma pack(pop)

typedef struct {
  UINT64 Magic;
  UINT32 Version;
  UINT32 HeaderSize;
  UINT32 TotalSize;
  UINT32 Command;
  UINT32 Status;
  UINT32 SwSmiValue;
  UINT32 PayloadCapacity;
  UINT32 PayloadSize;
  UINT32 Loaded;
  UINT32 Generation;
  UINT32 LastCommand;
  UINT64 Sequence;
  UINT64 Result;
  UINT64 PayloadHash;
  UINT64 PayloadOffset;
  UINT64 PayloadBase;
  UINT8 Reserved[128];
} MAILBOX;

typedef struct {
  UINT64 Magic;
  UINT32 Version;
  UINT32 Size;
  UINT32 SwSmiValue;
  UINT64 MailboxPhysical;
  UINT32 MailboxSize;
  UINT32 PayloadCapacity;
} MAILBOX_INFO;

static EFI_GUID gEfiSimpleFileSystemProtocolGuid = {
    0x0964e5b22,
    0x6459,
    0x11d2,
    {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};
static EFI_GUID gEfiSmmCommunicationProtocolGuid = {
    0xc68ed8e2,
    0x9dc6,
    0x4cbd,
    {0x9d, 0x94, 0xdb, 0x65, 0xac, 0xc5, 0xc3, 0x32}};
static EFI_GUID gCommunicationGuid = {
    0x2b29c9ad,
    0x2d8f,
    0x4f5e,
    {0x97, 0x4e, 0x5d, 0xe5, 0x54, 0x2e, 0x40, 0x31}};
static EFI_GUID gMailboxInfoGuid = {
    0x7b6a1a7d,
    0x2f5d,
    0x4fb8,
    {0xb4, 0x22, 0x91, 0x0d, 0x8a, 0x27, 0x46, 0x51}};
static EFI_GUID gEdkiiPiSmmCommunicationRegionTableGuid = {
    0x4e28ca50,
    0xd582,
    0x44ac,
    {0xa1, 0x1f, 0xe3, 0xd5, 0x65, 0x26, 0xdb, 0x34}};

static EFI_SYSTEM_TABLE *gSystemTable;
static EFI_EVENT gSimpleFsEvent;
static EFI_EVENT gSmmCommEvent;
static EFI_EVENT gRetryTimerEvent;
static VOID *gSimpleFsRegistration;
static VOID *gSmmCommRegistration;
static UINTN gSent;
static UINTN gRetryCount;
static EFI_PHYSICAL_ADDRESS gMailboxPhysical;
static UINT32 gMailboxSize;
static UINT8 gPayloadFile[PAYLOAD_FILE_LIMIT];
static UINT8 gCommBuffer[24 + COMM_HEADER_SIZE + PAYLOAD_FILE_LIMIT];

static BOOLEAN CompareGuid(const EFI_GUID *A, const EFI_GUID *B) {
  const UINT8 *Ap = (const UINT8 *)A;
  const UINT8 *Bp = (const UINT8 *)B;
  for (UINTN Index = 0; Index < sizeof(EFI_GUID); Index++) {
    if (Ap[Index] != Bp[Index]) {
      return 0;
    }
  }
  return 1;
}

static VOID SerialPutChar(char Ch) {
  UINTN Guard = 100000;
  if (Ch == '\n') {
    SerialPutChar('\r');
  }
  while (((__inbyte(COM1_PORT + 5) & 0x20) == 0) && Guard--) {
  }
  __outbyte(COM1_PORT, (UINT8)Ch);
}

static VOID SerialPrint(const char *Text) {
  while (*Text) {
    SerialPutChar(*Text++);
  }
}

static VOID SerialHex64(UINT64 Value) {
  char Hex[] = "0123456789ABCDEF";
  for (INTN Shift = 60; Shift >= 0; Shift -= 4) {
    SerialPutChar(Hex[(Value >> Shift) & 0xF]);
  }
}

static VOID ZeroMem(VOID *Buffer, UINTN Size) {
  UINT8 *Ptr = (UINT8 *)Buffer;
  while (Size--) {
    *Ptr++ = 0;
  }
}

static VOID CopyMemLocal(VOID *Destination, const VOID *Source, UINTN Size) {
  UINT8 *Dst = (UINT8 *)Destination;
  const UINT8 *Src = (const UINT8 *)Source;
  while (Size--) {
    *Dst++ = *Src++;
  }
}

VOID *memset(VOID *Destination, int Value, size_t Size) {
  UINT8 *Dst = (UINT8 *)Destination;
  while (Size--) {
    *Dst++ = (UINT8)Value;
  }
  return Destination;
}

VOID *memcpy(VOID *Destination, const VOID *Source, size_t Size) {
  CopyMemLocal(Destination, Source, Size);
  return Destination;
}

static VOID InitializeMailboxContents(MAILBOX *Mailbox,
                                      EFI_PHYSICAL_ADDRESS Physical,
                                      UINT32 Size) {
  ZeroMem(Mailbox, Size);
  Mailbox->Magic = MAILBOX_MAGIC;
  Mailbox->Version = MAILBOX_VERSION;
  Mailbox->HeaderSize = MAILBOX_HEADER_SIZE;
  Mailbox->TotalSize = Size;
  Mailbox->Status = STATUS_IDLE;
  Mailbox->SwSmiValue = SW_SMI_VALUE;
  Mailbox->PayloadCapacity = MAILBOX_PAYLOAD_CAPACITY;
  Mailbox->PayloadOffset = MAILBOX_HEADER_SIZE;
  Mailbox->PayloadBase = (UINT64)Physical + MAILBOX_HEADER_SIZE;
}

static EFI_STATUS PublishMailboxVariable(EFI_PHYSICAL_ADDRESS Physical,
                                         UINT32 Size) {
  static CHAR16 VariableName[] = {
      'S', 'M', 'M', '_', 'H', 'O', 'T',
      'R', 'E', 'L', 'O', 'A', 'D', 0};
  MAILBOX_INFO Info;
  EFI_STATUS Status;

  if (gSystemTable == 0 || gSystemTable->RuntimeServices == 0 ||
      gSystemTable->RuntimeServices->SetVariable == 0) {
    return EFI_INVALID_PARAMETER;
  }
  ZeroMem(&Info, sizeof(Info));
  Info.Magic = MAILBOX_MAGIC;
  Info.Version = MAILBOX_VERSION;
  Info.Size = (UINT32)sizeof(Info);
  Info.SwSmiValue = SW_SMI_VALUE;
  Info.MailboxPhysical = Physical;
  Info.MailboxSize = Size;
  Info.PayloadCapacity = MAILBOX_PAYLOAD_CAPACITY;
  Status = gSystemTable->RuntimeServices->SetVariable(
      VariableName, &gMailboxInfoGuid,
      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
      sizeof(Info), &Info);
  if (EFI_ERROR(Status)) {
    SerialPrint("SetVariable(mailbox) failed ");
    SerialHex64(Status);
    SerialPrint("\n");
    Status = gSystemTable->RuntimeServices->SetVariable(
        VariableName, &gMailboxInfoGuid,
        EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS |
            EFI_VARIABLE_RUNTIME_ACCESS,
        sizeof(Info), &Info);
    if (EFI_ERROR(Status)) {
      SerialPrint("SetVariable(mailbox NV) failed ");
      SerialHex64(Status);
      SerialPrint("\n");
    } else if (VERBOSE) {
      SerialPrint("mailbox variable published NV fallback\n");
    }
  } else if (VERBOSE) {
    SerialPrint("mailbox variable published\n");
  }
  return Status;
}

static EFI_STATUS EnsureMailbox(VOID) {
  EFI_STATUS Status;
  EFI_PHYSICAL_ADDRESS Address = 0xFFFFFFFFULL;
  UINTN Pages = (MAILBOX_TOTAL_SIZE + 0xFFFU) >> 12;

  if (gMailboxPhysical != 0) {
    return EFI_SUCCESS;
  }
  if (gSystemTable == 0 || gSystemTable->BootServices == 0 ||
      gSystemTable->BootServices->AllocatePages == 0) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gSystemTable->BootServices->AllocatePages(
      AllocateMaxAddress, EFI_RUNTIME_SERVICES_DATA, Pages, &Address);
  if (EFI_ERROR(Status)) {
    SerialPrint("AllocatePages(mailbox) failed ");
    SerialHex64(Status);
    SerialPrint("\n");
    return Status;
  }

  gMailboxPhysical = Address;
  gMailboxSize = (UINT32)(Pages << 12);
  InitializeMailboxContents((MAILBOX *)(UINTN)Address, Address,
                            gMailboxSize);
  if (VERBOSE) {
    SerialPrint("mailbox base=0x");
    SerialHex64(Address);
    SerialPrint(" size=0x");
    SerialHex64(gMailboxSize);
    SerialPrint(" sw=0x");
    SerialHex64(SW_SMI_VALUE);
    SerialPrint("\n");
  }
  PublishMailboxVariable(Address, gMailboxSize);
  return EFI_SUCCESS;
}

static EFI_STATUS ReadPayloadFile(UINTN *PayloadSize) {
  static CHAR16 PayloadPath[] = {
      '\\', 'E', 'F', 'I', '\\', 'S', 'M', 'M', '\\', 'P', 'A',
      'Y',  'L', 'O', 'A', 'D',  '.', 'E', 'F', 'I', 0};
  EFI_BOOT_SERVICES *Bs = gSystemTable->BootServices;
  EFI_HANDLE *Handles = 0;
  UINTN HandleCount = 0;
  EFI_STATUS Status;

  *PayloadSize = 0;
  Status = Bs->LocateHandleBuffer(2, &gEfiSimpleFileSystemProtocolGuid, 0,
                                  &HandleCount, &Handles);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  for (UINTN Index = 0; Index < HandleCount; Index++) {
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *Fs = 0;
    EFI_FILE_PROTOCOL *Root = 0;
    EFI_FILE_PROTOCOL *File = 0;
    UINTN ReadSize = PAYLOAD_FILE_LIMIT;

    Status = Bs->HandleProtocol(Handles[Index],
                                &gEfiSimpleFileSystemProtocolGuid,
                                (VOID **)&Fs);
    if (EFI_ERROR(Status) || Fs == 0) {
      continue;
    }

    Status = Fs->OpenVolume(Fs, &Root);
    if (EFI_ERROR(Status) || Root == 0) {
      continue;
    }

    Status = Root->Open(Root, &File, PayloadPath, 0x0000000000000001ULL, 0);
    if (!EFI_ERROR(Status) && File != 0) {
      ZeroMem(gPayloadFile, PAYLOAD_FILE_LIMIT);
      Status = File->Read(File, &ReadSize, gPayloadFile);
      File->Close(File);
      Root->Close(Root);
      if (!EFI_ERROR(Status) && ReadSize > 0 && ReadSize < PAYLOAD_FILE_LIMIT) {
        *PayloadSize = ReadSize;
        Bs->FreePool(Handles);
        return EFI_SUCCESS;
      }
    } else {
      Root->Close(Root);
    }
  }

  if (Handles != 0) {
    Bs->FreePool(Handles);
  }
  return EFI_NOT_FOUND;
}

static VOID *FindSmmCommRegion(UINTN RequiredSize, UINT32 *OriginalType) {
  EDKII_PI_SMM_COMMUNICATION_REGION_TABLE *Table = 0;
  UINT8 *Entry;

  *OriginalType = 0xFFFFFFFFU;
  if (gSystemTable == 0 || gSystemTable->ConfigurationTable == 0) {
    return 0;
  }

  for (UINTN Index = 0; Index < gSystemTable->NumberOfTableEntries; Index++) {
    EFI_CONFIGURATION_TABLE *Config = &gSystemTable->ConfigurationTable[Index];
    if (CompareGuid(&Config->VendorGuid,
                    &gEdkiiPiSmmCommunicationRegionTableGuid)) {
      Table = (EDKII_PI_SMM_COMMUNICATION_REGION_TABLE *)Config->VendorTable;
      break;
    }
  }
  if (Table == 0 || Table->Version != 1 || Table->NumberOfEntries == 0 ||
      Table->DescriptorSize < sizeof(EFI_MEMORY_DESCRIPTOR)) {
    if (VERBOSE) {
      SerialPrint("SMM comm region table missing\n");
    }
    return 0;
  }

  Entry = (UINT8 *)(Table + 1);
  for (UINTN Index = 0; Index < Table->NumberOfEntries; Index++) {
    EFI_MEMORY_DESCRIPTOR *Desc =
        (EFI_MEMORY_DESCRIPTOR *)(Entry + (Index * Table->DescriptorSize));
    UINT64 Bytes = Desc->NumberOfPages << 12;
    if (Bytes < RequiredSize) {
      continue;
    }
    if (Desc->Type == EFI_RESERVED_MEMORY_TYPE ||
        Desc->Type == EFI_RUNTIME_SERVICES_DATA ||
        Desc->Type == EFI_ACPI_MEMORY_NVS ||
        Desc->Type == EFI_CONVENTIONAL_MEMORY) {
      *OriginalType = Desc->Type;
      if (Desc->Type == EFI_CONVENTIONAL_MEMORY) {
        Desc->Type = EFI_RESERVED_MEMORY_TYPE;
      }
      if (VERBOSE) {
        SerialPrint("using SMM comm region base=0x");
        SerialHex64(Desc->PhysicalStart);
        SerialPrint(" size=0x");
        SerialHex64(Bytes);
        SerialPrint(" type=0x");
        SerialHex64(*OriginalType);
        SerialPrint("\n");
      }
      return (VOID *)(UINTN)Desc->PhysicalStart;
    }
  }

  if (VERBOSE) {
    SerialPrint("no SMM comm region large enough\n");
  }
  return 0;
}

static VOID RestoreSmmCommRegionType(VOID *CommBuffer, UINT32 OriginalType) {
  EDKII_PI_SMM_COMMUNICATION_REGION_TABLE *Table = 0;
  UINT8 *Entry;

  if (CommBuffer == 0 || OriginalType == 0xFFFFFFFFU ||
      gSystemTable == 0 || gSystemTable->ConfigurationTable == 0) {
    return;
  }
  if (OriginalType != EFI_CONVENTIONAL_MEMORY) {
    return;
  }

  for (UINTN Index = 0; Index < gSystemTable->NumberOfTableEntries; Index++) {
    EFI_CONFIGURATION_TABLE *Config = &gSystemTable->ConfigurationTable[Index];
    if (CompareGuid(&Config->VendorGuid,
                    &gEdkiiPiSmmCommunicationRegionTableGuid)) {
      Table = (EDKII_PI_SMM_COMMUNICATION_REGION_TABLE *)Config->VendorTable;
      break;
    }
  }
  if (Table == 0) {
    return;
  }
  Entry = (UINT8 *)(Table + 1);
  for (UINTN Index = 0; Index < Table->NumberOfEntries; Index++) {
    EFI_MEMORY_DESCRIPTOR *Desc =
        (EFI_MEMORY_DESCRIPTOR *)(Entry + (Index * Table->DescriptorSize));
    if ((VOID *)(UINTN)Desc->PhysicalStart == CommBuffer) {
      Desc->Type = OriginalType;
      return;
    }
  }
}

static EFI_STATUS TrySendPayload(const char *Reason) {
  EFI_BOOT_SERVICES *Bs = gSystemTable->BootServices;
  EFI_SMM_COMMUNICATION_PROTOCOL *SmmComm = 0;
  EFI_SMM_COMMUNICATE_HEADER *Header;
  COMM_MESSAGE *Message;
  VOID *CommBuffer;
  UINT32 OriginalRegionType;
  EFI_STATUS Status;
  EFI_STATUS TimerStatus;
  UINTN PayloadSize = 0;
  UINTN CommSize;

  if (gSent != 0) {
    return EFI_ALREADY_STARTED;
  }

  if (VERBOSE) {
    SerialPrint("bridge attempt: ");
    SerialPrint(Reason);
    SerialPrint("\n");
  }

  Status = ReadPayloadFile(&PayloadSize);
  if (EFI_ERROR(Status)) {
    if (VERBOSE) {
      SerialPrint("payload not available ");
      SerialHex64(Status);
      SerialPrint("\n");
    }
    return Status;
  }

  Status = Bs->LocateProtocol(&gEfiSmmCommunicationProtocolGuid, 0,
                              (VOID **)&SmmComm);
  if (EFI_ERROR(Status) || SmmComm == 0) {
    if (VERBOSE) {
      SerialPrint("SmmCommunication unavailable ");
      SerialHex64(Status);
      SerialPrint("\n");
    }
    return Status;
  }

  EnsureMailbox();

  CommSize = 24 + COMM_HEADER_SIZE + PayloadSize;
  CommBuffer = FindSmmCommRegion(CommSize, &OriginalRegionType);
  if (CommBuffer == 0) {
    if (VERBOSE) {
      SerialPrint("falling back to static comm buffer\n");
    }
    CommBuffer = gCommBuffer;
    OriginalRegionType = 0xFFFFFFFFU;
  }

  ZeroMem(CommBuffer, CommSize);
  Header = (EFI_SMM_COMMUNICATE_HEADER *)CommBuffer;
  Header->HeaderGuid = gCommunicationGuid;
  Header->MessageLength = COMM_HEADER_SIZE + PayloadSize;
  Message = (COMM_MESSAGE *)Header->Data;
  Message->Magic = COMM_MAGIC;
  Message->Version = COMM_VERSION;
  Message->Command = 1;
  Message->PayloadSize = (UINT32)PayloadSize;
  Message->MailboxPhysical = (UINT64)gMailboxPhysical;
  Message->MailboxSize = gMailboxSize;
  Message->SwSmiValue = SW_SMI_VALUE;
  CopyMemLocal(Message->Payload, gPayloadFile, PayloadSize);

  if (VERBOSE) {
    SerialPrint("communicating payload bytes=0x");
    SerialHex64(PayloadSize);
    SerialPrint("\n");
  }
  Status = SmmComm->Communicate(SmmComm, Header, &CommSize);
  RestoreSmmCommRegionType(CommBuffer, OriginalRegionType);
  if (!EFI_ERROR(Status)) {
    SerialPrint("boot payload sent bytes=0x");
    SerialHex64(PayloadSize);
    SerialPrint("\n");
    gSent = 1;
    if (gRetryTimerEvent != 0) {
      TimerStatus = Bs->SetTimer(gRetryTimerEvent, TimerCancel, 0);
      if (EFI_ERROR(TimerStatus)) {
        SerialPrint("timer cancel failed ");
        SerialHex64(TimerStatus);
        SerialPrint("\n");
      } else if (VERBOSE) {
        SerialPrint("retry timer canceled\n");
      }
    }
  } else {
    SerialPrint("communicate failed ");
    SerialHex64(Status);
    SerialPrint(" size=0x");
    SerialHex64(CommSize);
    SerialPrint("\n");
  }
  return Status;
}

static VOID EFIAPI BridgeNotify(EFI_EVENT Event, VOID *Context) {
  (void)Event;
  if (Context == (VOID *)"timer") {
    gRetryCount++;
    if ((gRetryCount % 2) != 0) {
      return;
    }
  }
  TrySendPayload((const char *)Context);
}

static VOID RegisterRetryTimer(VOID) {
  EFI_STATUS Status;
  EFI_BOOT_SERVICES *Bs = gSystemTable->BootServices;

  if (gRetryTimerEvent != 0) {
    return;
  }
  Status = Bs->CreateEvent(EVT_TIMER | EVT_NOTIFY_SIGNAL, TPL_CALLBACK,
                           BridgeNotify, (VOID *)"timer",
                           &gRetryTimerEvent);
  if (EFI_ERROR(Status)) {
    SerialPrint("CreateEvent(timer) failed ");
    SerialHex64(Status);
    SerialPrint("\n");
    gRetryTimerEvent = 0;
    return;
  }
  Status = Bs->SetTimer(gRetryTimerEvent, TimerPeriodic, 5000000ULL);
  if (EFI_ERROR(Status)) {
    SerialPrint("SetTimer failed ");
    SerialHex64(Status);
    SerialPrint("\n");
    Bs->CloseEvent(gRetryTimerEvent);
    gRetryTimerEvent = 0;
    return;
  }
  if (VERBOSE) {
    SerialPrint("retry timer registered\n");
  }
}

static VOID RegisterNotify(EFI_GUID *Guid, EFI_EVENT *Event, VOID **Registration,
                           const char *Name) {
  EFI_STATUS Status;
  EFI_BOOT_SERVICES *Bs = gSystemTable->BootServices;

  if (*Event != 0) {
    return;
  }
  Status = Bs->CreateEvent(EVT_NOTIFY_SIGNAL, TPL_CALLBACK, BridgeNotify,
                           (VOID *)Name, Event);
  if (EFI_ERROR(Status)) {
    SerialPrint("CreateEvent failed ");
    SerialHex64(Status);
    SerialPrint("\n");
    *Event = 0;
    return;
  }
  Status = Bs->RegisterProtocolNotify(Guid, *Event, Registration);
  if (EFI_ERROR(Status)) {
    SerialPrint("RegisterProtocolNotify failed ");
    SerialHex64(Status);
    SerialPrint("\n");
    Bs->CloseEvent(*Event);
    *Event = 0;
    return;
  }
  if (VERBOSE) {
    SerialPrint("notify registered for ");
    SerialPrint(Name);
    SerialPrint("\n");
  }
}

EFI_STATUS EFIAPI DxeBridgeEntry(EFI_HANDLE ImageHandle,
                                        EFI_SYSTEM_TABLE *SystemTable) {
  (void)ImageHandle;
  gSystemTable = SystemTable;
  SerialPrint("DXE init\n");

  if (SystemTable == 0 || SystemTable->BootServices == 0) {
    return EFI_INVALID_PARAMETER;
  }

  EnsureMailbox();
  if (EFI_ERROR(TrySendPayload("entry"))) {
    RegisterRetryTimer();
    RegisterNotify(&gEfiSimpleFileSystemProtocolGuid, &gSimpleFsEvent,
                   &gSimpleFsRegistration, "SimpleFS");
    RegisterNotify(&gEfiSmmCommunicationProtocolGuid, &gSmmCommEvent,
                   &gSmmCommRegistration, "SmmCommunication");
  }
  return EFI_SUCCESS;
}