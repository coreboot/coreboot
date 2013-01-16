#include <stdint.h>

#define PACKED __attribute__((packed))

typedef uint64_t UINT64;
typedef uint32_t UINT32;
typedef uint16_t UINT16;
typedef uint8_t UINT8;

typedef struct {
 UINT32 Data1;
 UINT16 Data2;
 UINT16 Data3;
 UINT8  Data4[8];
} PACKED EFI_GUID;

typedef UINT32 EFI_FVB_ATTRIBUTES;

#define EFI_FVB_READ_DISABLED_CAP  0x00000001
#define EFI_FVB_READ_ENABLED_CAP   0x00000002
#define EFI_FVB_READ_STATUS        0x00000004
#define EFI_FVB_WRITE_DISABLED_CAP 0x00000008
#define EFI_FVB_WRITE_ENABLED_CAP  0x00000010
#define EFI_FVB_WRITE_STATUS       0x00000020
#define EFI_FVB_LOCK_CAP           0x00000040
#define EFI_FVB_LOCK_STATUS        0x00000080
#define EFI_FVB_STICKY_WRITE       0x00000200
#define EFI_FVB_MEMORY_MAPPED      0x00000400
#define EFI_FVB_ERASE_POLARITY     0x00000800
#define EFI_FVB_ALIGNMENT_CAP      0x00008000

#define EFI_FVB_ALIGNMENT_2   0x00010000
#define EFI_FVB_ALIGNMENT_4   0x00020000
#define EFI_FVB_ALIGNMENT_8   0x00040000
#define EFI_FVB_ALIGNMENT_16  0x00080000
#define EFI_FVB_ALIGNMENT_32  0x00100000
#define EFI_FVB_ALIGNMENT_64  0x00200000
#define EFI_FVB_ALIGNMENT_128 0x00400000
#define EFI_FVB_ALIGNMENT_256 0x00800000
#define EFI_FVB_ALIGNMENT_512 0x01000000
#define EFI_FVB_ALIGNMENT_1K  0x02000000
#define EFI_FVB_ALIGNMENT_2K  0x04000000
#define EFI_FVB_ALIGNMENT_4K  0x08000000
#define EFI_FVB_ALIGNMENT_8K  0x10000000
#define EFI_FVB_ALIGNMENT_16K 0x20000000
#define EFI_FVB_ALIGNMENT_32K 0x40000000
#define EFI_FVB_ALIGNMENT_64K 0x80000000

typedef struct {
  UINT32 NumBlocks;
  UINT32 Length;
} PACKED EFI_FV_BLOCK_MAP_ENTRY;

typedef struct {
  UINT8                   ZeroVector[16];
  EFI_GUID                FileSystemGuid;
  UINT64                  FvLength;
  UINT32                  Signature;
  EFI_FVB_ATTRIBUTES      Attributes;
  UINT16                  HeaderLength;
  UINT16                  Checksum;
  UINT16		  ExtHeaderOffset;
  UINT8                   Reserved[1];
  UINT8                   Revision;
  EFI_FV_BLOCK_MAP_ENTRY  FvBlockMap[1];
} PACKED EFI_FIRMWARE_VOLUME_HEADER;


typedef union {
  struct {
    UINT8 Header;
    UINT8 File;
  } Checksum;
  UINT16 TailReference;
} PACKED EFI_FFS_INTEGRITY_CHECK;

typedef UINT8 EFI_FV_FILETYPE;

#define EFI_FV_FILETYPE_RAW                   0x01
#define EFI_FV_FILETYPE_FREEFORM              0x02
#define EFI_FV_FILETYPE_SECURITY_CORE         0x03
#define EFI_FV_FILETYPE_PEI_CORE              0x04
#define EFI_FV_FILETYPE_DXE_CORE              0x05
#define EFI_FV_FILETYPE_PEIM                  0x06
#define EFI_FV_FILETYPE_DRIVER                0x07
#define EFI_FV_FILETYPE_COMBINED_PEIM_DRIVER  0x08
#define EFI_FV_FILETYPE_APPLICATION           0x09
#define EFI_FV_FILETYPE_FIRMWARE_VOLUME_IMAGE 0x0b
#define EFI_FV_FILETYPE_FFS_PAD               0xf0

typedef UINT8 EFI_FFS_FILE_ATTRIBUTES;
#define FFS_ATTRIB_TAIL_PRESENT     0x01
#define FFS_ATTRIB_RECOVERY         0x02
#define FFS_ATTRIB_HEADER_EXTENSION 0x04
#define FFS_ATTRIB_DATA_ALIGNMENT   0x38
#define FFS_ATTRIB_CHECKSUM         0x40

typedef UINT8 EFI_FFS_FILE_STATE;
#define EFI_FILE_HEADER_CONSTRUCTION 0x01
#define EFI_FILE_HEADER_VALID        0x02
#define EFI_FILE_DATA_VALID          0x04
#define EFI_FILE_MARKED_FOR_UPDATE   0x08
#define EFI_FILE_DELETED             0x10
#define EFI_FILE_HEADER_INVALID      0x20

typedef struct {
  EFI_GUID                Name;
  EFI_FFS_INTEGRITY_CHECK IntegrityCheck;
  EFI_FV_FILETYPE         Type;
  EFI_FFS_FILE_ATTRIBUTES Attributes;
  UINT8                   Size[3];
  EFI_FFS_FILE_STATE      State;
} PACKED EFI_FFS_FILE_HEADER;

