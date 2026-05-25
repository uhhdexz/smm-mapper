#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winioctl.h>
#include <stdio.h>
#include <stdint.h>
#include "SmmClient.h"

static int EnablePrivilege(const wchar_t *Name) {
  HANDLE Token;
  TOKEN_PRIVILEGES Tp;
  LUID Luid;

  if (!OpenProcessToken(GetCurrentProcess(),
                        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &Token)) {
    return 0;
  }
  if (!LookupPrivilegeValueW(NULL, Name, &Luid)) {
    CloseHandle(Token);
    return 0;
  }
  Tp.PrivilegeCount = 1;
  Tp.Privileges[0].Luid = Luid;
  Tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
  AdjustTokenPrivileges(Token, FALSE, &Tp, sizeof(Tp), NULL, NULL);
  CloseHandle(Token);
  return GetLastError() == ERROR_SUCCESS;
}

static uint64_t FnV1a64(const void *Buffer, uint32_t Size) {
  const unsigned char *Data = (const unsigned char *)Buffer;
  uint64_t Hash = 0xCBF29CE484222325ULL;
  while (Size--) {
    Hash ^= *Data++;
    Hash *= 0x100000001B3ULL;
  }
  return Hash;
}

static int ReadMailboxInfo(MAILBOX_INFO *Info) {
  DWORD Got;
  const wchar_t *Guid =
      L"{7B6A1A7D-2F5D-4FB8-B422-910D8A274651}";
  ZeroMemory(Info, sizeof(*Info));
  EnablePrivilege(L"SeSystemEnvironmentPrivilege");
  Got = GetFirmwareEnvironmentVariableW(L"SMM_HOTRELOAD", Guid, Info,
                                        sizeof(*Info));
  if (Got != sizeof(*Info) || Info->Magic != MAILBOX_MAGIC ||
      Info->MailboxPhysical == 0 ||
      Info->MailboxSize < MAILBOX_TOTAL_SIZE) {
    fwprintf(stderr,
             L"Could not read valid SMM_HOTRELOAD firmware variable "
             L"(got %lu, error %lu).\n",
             Got, GetLastError());
    return 0;
  }
  return 1;
}

static uint8_t *ReadFileBytes(const wchar_t *Path, DWORD *SizeOut) {
  HANDLE File;
  LARGE_INTEGER Size;
  DWORD Got;
  uint8_t *Bytes;

  *SizeOut = 0;
  File = CreateFileW(Path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                     FILE_ATTRIBUTE_NORMAL, NULL);
  if (File == INVALID_HANDLE_VALUE) {
    return NULL;
  }
  if (!GetFileSizeEx(File, &Size) || Size.QuadPart <= 0 ||
      Size.QuadPart > MAILBOX_PAYLOAD_CAPACITY) {
    CloseHandle(File);
    return NULL;
  }
  Bytes = (uint8_t *)HeapAlloc(GetProcessHeap(), 0, (SIZE_T)Size.QuadPart);
  if (Bytes == NULL) {
    CloseHandle(File);
    return NULL;
  }
  if (!ReadFile(File, Bytes, (DWORD)Size.QuadPart, &Got, NULL) ||
      Got != (DWORD)Size.QuadPart) {
    HeapFree(GetProcessHeap(), 0, Bytes);
    CloseHandle(File);
    return NULL;
  }
  CloseHandle(File);
  *SizeOut = Got;
  return Bytes;
}

static void Com1Write(const char *Text, DWORD Size) {
  HANDLE Port;
  DCB Dcb;
  COMMTIMEOUTS Timeouts;
  DWORD Written;

  if (Text == NULL || Size == 0) {
    return;
  }
  Port = CreateFileW(L"\\\\.\\COM1", GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
                     0, NULL);
  if (Port == INVALID_HANDLE_VALUE) {
    return;
  }
  SecureZeroMemory(&Dcb, sizeof(Dcb));
  Dcb.DCBlength = sizeof(Dcb);
  if (GetCommState(Port, &Dcb)) {
    Dcb.BaudRate = CBR_115200;
    Dcb.ByteSize = 8;
    Dcb.Parity = NOPARITY;
    Dcb.StopBits = ONESTOPBIT;
    Dcb.fBinary = TRUE;
    Dcb.fDtrControl = DTR_CONTROL_ENABLE;
    Dcb.fRtsControl = RTS_CONTROL_ENABLE;
    SetCommState(Port, &Dcb);
  }
  SecureZeroMemory(&Timeouts, sizeof(Timeouts));
  Timeouts.WriteTotalTimeoutConstant = 250;
  SetCommTimeouts(Port, &Timeouts);
  WriteFile(Port, Text, Size, &Written, NULL);
  CloseHandle(Port);
}

static void PrintDebugLog(const REQUEST *Request) {
  DWORD Size;

  if (Request == NULL || Request->DebugLogSize == 0) {
    return;
  }
  Size = Request->DebugLogSize;
  if (Size > MAILBOX_LOG_CAPACITY) {
    Size = MAILBOX_LOG_CAPACITY;
  }
  fwrite(Request->DebugLog, 1, Size, stdout);
  if (Size == 0 || Request->DebugLog[Size - 1] != '\n') {
    fputc('\n', stdout);
  }
  Com1Write(Request->DebugLog, Size);
}

static int ExecuteCommand(uint32_t Command, const wchar_t *PayloadPath) {
  MAILBOX_INFO Info;
  HANDLE Device;
  REQUEST *Request;
  DWORD PayloadSize = 0;
  uint8_t *Payload = NULL;
  DWORD RequestSize;
  DWORD Returned = 0;
  BOOL Ok;

  if (!ReadMailboxInfo(&Info)) {
    return 1;
  }
  if (Command == COMMAND_RELOAD) {
    Payload = ReadFileBytes(PayloadPath, &PayloadSize);
    if (Payload == NULL) {
      fwprintf(stderr, L"Could not read payload %ls\n", PayloadPath);
      return 1;
    }
    wprintf(L"payload size=0x%X hash=0x%llX\n", PayloadSize,
            (unsigned long long)FnV1a64(Payload, PayloadSize));
  }

  RequestSize = (DWORD)(REQUEST_HEADER_SIZE + PayloadSize);
  Request = (REQUEST *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                        RequestSize);
  if (Request == NULL) {
    HeapFree(GetProcessHeap(), 0, Payload);
    return 1;
  }
  Request->MailboxPhysical = Info.MailboxPhysical;
  Request->MailboxSize = Info.MailboxSize;
  Request->SwSmiValue = Info.SwSmiValue;
  Request->Command = Command;
  Request->PayloadSize = PayloadSize;
  Request->TimeoutMs = 3000;
  if (PayloadSize != 0) {
    CopyMemory(Request->Payload, Payload, PayloadSize);
  }

  Device = CreateFileW(DEVICE_NAME, GENERIC_READ | GENERIC_WRITE, 0,
                       NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (Device == INVALID_HANDLE_VALUE) {
    fwprintf(stderr, L"Could not open %ls (error %lu). Is the driver loaded?\n",
             DEVICE_NAME, GetLastError());
    HeapFree(GetProcessHeap(), 0, Payload);
    HeapFree(GetProcessHeap(), 0, Request);
    return 1;
  }

  Ok = DeviceIoControl(Device, IOCTL_EXECUTE, Request, RequestSize,
                       Request, RequestSize, &Returned, NULL);
  if (!Ok) {
    fwprintf(stderr, L"DeviceIoControl failed: %lu\n", GetLastError());
    CloseHandle(Device);
    HeapFree(GetProcessHeap(), 0, Payload);
    HeapFree(GetProcessHeap(), 0, Request);
    return 1;
  }

  wprintf(L"mailbox=0x%llX size=0x%X sw=0x%X\n",
          (unsigned long long)Info.MailboxPhysical, Info.MailboxSize,
          Info.SwSmiValue);
  wprintf(L"status=0x%X result=0x%llX loaded=%u generation=%u sequence=%llu\n",
          Request->Status, (unsigned long long)Request->Result,
          Request->Loaded, Request->Generation,
          (unsigned long long)Request->Sequence);
  PrintDebugLog(Request);

  CloseHandle(Device);
  HeapFree(GetProcessHeap(), 0, Payload);
  HeapFree(GetProcessHeap(), 0, Request);
  return 0;
}

int wmain(int argc, wchar_t **argv) {
  if (argc == 2 && _wcsicmp(argv[1], L"status") == 0) {
    return ExecuteCommand(COMMAND_STATUS, NULL);
  }
  if (argc == 2 && _wcsicmp(argv[1], L"ping") == 0) {
    return ExecuteCommand(COMMAND_PING, NULL);
  }
  if (argc == 2 && _wcsicmp(argv[1], L"unload") == 0) {
    return ExecuteCommand(COMMAND_UNLOAD, NULL);
  }
  if (argc == 3 && _wcsicmp(argv[1], L"reload") == 0) {
    return ExecuteCommand(COMMAND_RELOAD, argv[2]);
  }

  fwprintf(stderr,
           L"Usage:\n"
           L"  SmmClient.exe status\n"
           L"  SmmClient.exe ping\n"
           L"  SmmClient.exe unload\n"
           L"  SmmClient.exe reload C:\\path\\PAYLOAD.EFI\n");
  return 1;
}