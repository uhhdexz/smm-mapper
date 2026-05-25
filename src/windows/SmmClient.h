#ifndef SMM_CLIENT_H
#define SMM_CLIENT_H

#define DEVICE_NAME L"\\\\.\\SmmClient"
#define NT_DEVICE_NAME L"\\Device\\SmmClient"
#define DOS_DEVICE_NAME L"\\DosDevices\\SmmClient"

#define IOCTL_EXECUTE \
  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define MAILBOX_MAGIC 0x58425645444D4D53ULL
#define MAILBOX_HEADER_SIZE 0x1000U
#define MAILBOX_PAYLOAD_CAPACITY (256U * 1024U)
#define MAILBOX_LOG_CAPACITY 3072U
#define MAILBOX_TOTAL_SIZE \
  (MAILBOX_HEADER_SIZE + MAILBOX_PAYLOAD_CAPACITY)
#define SW_SMI_VALUE 0xD5U

#define COMMAND_NONE 0U
#define COMMAND_PING 1U
#define COMMAND_STATUS 2U
#define COMMAND_UNLOAD 3U
#define COMMAND_RELOAD 4U

#define STATUS_IDLE 0U
#define STATUS_BUSY 1U
#define STATUS_OK 2U
#define STATUS_ERROR 0x80000000U

typedef struct {
  unsigned long long Magic;
  unsigned int HeaderSize;
  unsigned int TotalSize;
  volatile unsigned int Command;
  volatile unsigned int Status;
  unsigned int SwSmiValue;
  unsigned int PayloadCapacity;
  volatile unsigned int PayloadSize;
  volatile unsigned int Loaded;
  volatile unsigned int Generation;
  volatile unsigned int LastCommand;
  volatile unsigned long long Sequence;
  volatile unsigned long long Result;
  volatile unsigned long long PayloadHash;
  unsigned long long PayloadOffset;
  volatile unsigned long long PayloadBase;
  volatile unsigned int DebugLogSize;
  unsigned int DebugLogCapacity;
  unsigned char DebugLog[MAILBOX_LOG_CAPACITY];
  unsigned char Reserved[128];
} MAILBOX;

typedef struct {
  unsigned long long Magic;
  unsigned int Size;
  unsigned int SwSmiValue;
  unsigned long long MailboxPhysical;
  unsigned int MailboxSize;
  unsigned int PayloadCapacity;
} MAILBOX_INFO;

typedef struct {
  unsigned long long MailboxPhysical;
  unsigned int MailboxSize;
  unsigned int SwSmiValue;
  unsigned int Command;
  unsigned int PayloadSize;
  unsigned int TimeoutMs;
  unsigned int Status;
  unsigned int Loaded;
  unsigned int Generation;
  unsigned int DebugLogSize;
  unsigned long long Result;
  unsigned long long Sequence;
  char DebugLog[MAILBOX_LOG_CAPACITY];
  unsigned char Payload[1];
} REQUEST;

#define REQUEST_HEADER_SIZE (sizeof(REQUEST) - 1U)

#endif