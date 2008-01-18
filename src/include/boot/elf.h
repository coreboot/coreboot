#ifndef ELF_H
#define	ELF_H

/* Standard ELF types.  */

#include <stdint.h>
#include <stddef.h>
#include <arch/boot/boot.h>

/* Type for a 16-bit quantity.  */
typedef uint16_t Elf32_Half;
typedef uint16_t Elf64_Half;

/* Types for signed and unsigned 32-bit quantities.  */
typedef uint32_t Elf32_Word;
typedef	int32_t  Elf32_Sword;
typedef uint32_t Elf64_Word;
typedef	int32_t  Elf64_Sword;

/* Types for signed and unsigned 64-bit quantities.  */
typedef uint64_t Elf32_Xword;
typedef	int64_t  Elf32_Sxword;
typedef uint64_t Elf64_Xword;
typedef	int64_t  Elf64_Sxword;

/* Type of addresses.  */
typedef uint32_t Elf32_Addr;
typedef uint64_t Elf64_Addr;

/* Type of file offsets.  */
typedef uint32_t Elf32_Off;
typedef uint64_t Elf64_Off;

/* Type for section indices, which are 16-bit quantities.  */
typedef uint16_t Elf32_Section;
typedef uint16_t Elf64_Section;

/* Type of symbol indices.  */
typedef uint32_t Elf32_Symndx;
typedef uint64_t Elf64_Symndx;


/* The ELF file header.  This appears at the start of every ELF file.  */

#define EI_NIDENT (16)

typedef struct
{
  unsigned char	e_ident[EI_NIDENT];	/* Magic number and other info */
  Elf32_Half	e_type;			/* Object file type */
  Elf32_Half	e_machine;		/* Architecture */
  Elf32_Word	e_version;		/* Object file version */
  Elf32_Addr	e_entry;		/* Entry point virtual address */
  Elf32_Off	e_phoff;		/* Program header table file offset */
  Elf32_Off	e_shoff;		/* Section header table file offset */
  Elf32_Word	e_flags;		/* Processor-specific flags */
  Elf32_Half	e_ehsize;		/* ELF header size in bytes */
  Elf32_Half	e_phentsize;		/* Program header table entry size */
  Elf32_Half	e_phnum;		/* Program header table entry count */
  Elf32_Half	e_shentsize;		/* Section header table entry size */
  Elf32_Half	e_shnum;		/* Section header table entry count */
  Elf32_Half	e_shstrndx;		/* Section header string table index */
} Elf32_Ehdr;

typedef struct
{
  unsigned char	e_ident[EI_NIDENT];	/* Magic number and other info */
  Elf64_Half	e_type;			/* Object file type */
  Elf64_Half	e_machine;		/* Architecture */
  Elf64_Word	e_version;		/* Object file version */
  Elf64_Addr	e_entry;		/* Entry point virtual address */
  Elf64_Off	e_phoff;		/* Program header table file offset */
  Elf64_Off	e_shoff;		/* Section header table file offset */
  Elf64_Word	e_flags;		/* Processor-specific flags */
  Elf64_Half	e_ehsize;		/* ELF header size in bytes */
  Elf64_Half	e_phentsize;		/* Program header table entry size */
  Elf64_Half	e_phnum;		/* Program header table entry count */
  Elf64_Half	e_shentsize;		/* Section header table entry size */
  Elf64_Half	e_shnum;		/* Section header table entry count */
  Elf64_Half	e_shstrndx;		/* Section header string table index */
} Elf64_Ehdr;

/* Fields in the e_ident array.  The EI_* macros are indices into the
   array.  The macros under each EI_* macro are the values the byte
   may have.  */

#define EI_MAG0		0		/* File identification byte 0 index */
#define ELFMAG0		0x7f		/* Magic number byte 0 */

#define EI_MAG1		1		/* File identification byte 1 index */
#define ELFMAG1		'E'		/* Magic number byte 1 */

#define EI_MAG2		2		/* File identification byte 2 index */
#define ELFMAG2		'L'		/* Magic number byte 2 */

#define EI_MAG3		3		/* File identification byte 3 index */
#define ELFMAG3		'F'		/* Magic number byte 3 */

/* Conglomeration of the identification bytes, for easy testing as a word.  */
#define	ELFMAG		"\177ELF"
#define	SELFMAG		4

#define EI_CLASS	4		/* File class byte index */
#define ELFCLASSNONE	0		/* Invalid class */
#define ELFCLASS32	1		/* 32-bit objects */
#define ELFCLASS64	2		/* 64-bit objects */
#define ELFCLASSNUM	3

#define EI_DATA		5		/* Data encoding byte index */
#define ELFDATANONE	0		/* Invalid data encoding */
#define ELFDATA2LSB	1		/* 2's complement, little endian */
#define ELFDATA2MSB	2		/* 2's complement, big endian */
#define ELFDATANUM	3

#define EI_VERSION	6		/* File version byte index */
					/* Value must be EV_CURRENT */

#define EI_OSABI	7		/* OS ABI identification */
#define ELFOSABI_SYSV		0	/* UNIX System V ABI */
#define ELFOSABI_HPUX		1	/* HP-UX */
#define ELFOSABI_ARM		97	/* ARM */
#define ELFOSABI_STANDALONE	255	/* Standalone (embedded) application */

#define EI_ABIVERSION	8		/* ABI version */

#define EI_PAD		9		/* Byte index of padding bytes */

/* Legal values for e_type (object file type).  */

#define ET_NONE		0		/* No file type */
#define ET_REL		1		/* Relocatable file */
#define ET_EXEC		2		/* Executable file */
#define ET_DYN		3		/* Shared object file */
#define ET_CORE		4		/* Core file */
#define	ET_NUM		5		/* Number of defined types */
#define ET_LOPROC	0xff00		/* Processor-specific */
#define ET_HIPROC	0xffff		/* Processor-specific */

/* Legal values for e_machine (architecture).  */

#define EM_NONE		 0		/* No machine */
#define EM_M32		 1		/* AT&T WE 32100 */
#define EM_SPARC	 2		/* SUN SPARC */
#define EM_386		 3		/* Intel 80386 */
#define EM_68K		 4		/* Motorola m68k family */
#define EM_88K		 5		/* Motorola m88k family */
#define EM_486		 6		/* Intel 80486 */
#define EM_860		 7		/* Intel 80860 */
#define EM_MIPS		 8		/* MIPS R3000 big-endian */
#define EM_S370		 9		/* Amdahl */
#define EM_MIPS_RS4_BE	10		/* MIPS R4000 big-endian */
#define EM_RS6000	11		/* RS6000 */

#define EM_PARISC	15		/* HPPA */
#define EM_nCUBE	16		/* nCUBE */
#define EM_VPP500	17		/* Fujitsu VPP500 */
#define EM_SPARC32PLUS	18		/* Sun's "v8plus" */
#define EM_960		19		/* Intel 80960 */
#define EM_PPC		20		/* PowerPC */

#define EM_V800		36		/* NEC V800 series */
#define EM_FR20		37		/* Fujitsu FR20 */
#define EM_RH32		38		/* TRW RH32 */
#define EM_MMA		39		/* Fujitsu MMA */
#define EM_ARM		40		/* ARM */
#define EM_FAKE_ALPHA	41		/* Digital Alpha */
#define EM_SH		42		/* Hitachi SH */
#define EM_SPARCV9	43		/* SPARC v9 64-bit */
#define EM_TRICORE	44		/* Siemens Tricore */
#define EM_ARC		45		/* Argonaut RISC Core */
#define EM_H8_300	46		/* Hitachi H8/300 */
#define EM_H8_300H	47		/* Hitachi H8/300H */
#define EM_H8S		48		/* Hitachi H8S */
#define EM_H8_500	49		/* Hitachi H8/500 */
#define EM_IA_64	50		/* Intel Merced */
#define EM_MIPS_X	51		/* Stanford MIPS-X */
#define EM_COLDFIRE	52		/* Motorola Coldfire */
#define EM_68HC12	53		/* Motorola M68HC12 */
#define EM_NUM		54

/* If it is necessary to assign new unofficial EM_* values, please
   pick large random numbers (0x8523, 0xa7f2, etc.) to minimize the
   chances of collision with official or non-GNU unofficial values.  */

#define EM_ALPHA	0x9026

/* Legal values for e_version (version).  */

#define EV_NONE		0		/* Invalid ELF version */
#define EV_CURRENT	1		/* Current version */
#define EV_NUM		2


/* Program segment header.  */

typedef struct
{
  Elf32_Word	p_type;			/* Segment type */
  Elf32_Off	p_offset;		/* Segment file offset */
  Elf32_Addr	p_vaddr;		/* Segment virtual address */
  Elf32_Addr	p_paddr;		/* Segment physical address */
  Elf32_Word	p_filesz;		/* Segment size in file */
  Elf32_Word	p_memsz;		/* Segment size in memory */
  Elf32_Word	p_flags;		/* Segment flags */
  Elf32_Word	p_align;		/* Segment alignment */
} Elf32_Phdr;

typedef struct
{
  Elf64_Word	p_type;			/* Segment type */
  Elf64_Word	p_flags;		/* Segment flags */
  Elf64_Off	p_offset;		/* Segment file offset */
  Elf64_Addr	p_vaddr;		/* Segment virtual address */
  Elf64_Addr	p_paddr;		/* Segment physical address */
  Elf64_Xword	p_filesz;		/* Segment size in file */
  Elf64_Xword	p_memsz;		/* Segment size in memory */
  Elf64_Xword	p_align;		/* Segment alignment */
} Elf64_Phdr;

/* Legal values for p_type (segment type).  */

#define	PT_NULL		0		/* Program header table entry unused */
#define PT_LOAD		1		/* Loadable program segment */
#define PT_DYNAMIC	2		/* Dynamic linking information */
#define PT_INTERP	3		/* Program interpreter */
#define PT_NOTE		4		/* Auxiliary information */
#define PT_SHLIB	5		/* Reserved */
#define PT_PHDR		6		/* Entry for header table itself */
#define	PT_NUM		7		/* Number of defined types.  */
#define PT_LOOS		0x60000000	/* Start of OS-specific */
#define PT_HIOS		0x6fffffff	/* End of OS-specific */
#define PT_LOPROC	0x70000000	/* Start of processor-specific */
#define PT_HIPROC	0x7fffffff	/* End of processor-specific */

/* Legal values for p_flags (segment flags).  */

#define PF_X		(1 << 0)	/* Segment is executable */
#define PF_W		(1 << 1)	/* Segment is writable */
#define PF_R		(1 << 2)	/* Segment is readable */
#define PF_MASKPROC	0xf0000000	/* Processor-specific */


/* Note section contents.  Each entry in the note section begins with
   a header of a fixed form.  */

typedef struct
{
  Elf32_Word n_namesz;			/* Length of the note's name.  */
  Elf32_Word n_descsz;			/* Length of the note's descriptor.  */
  Elf32_Word n_type;			/* Type of the note.  */
} Elf32_Nhdr;

typedef struct
{
  Elf64_Word n_namesz;			/* Length of the note's name.  */
  Elf64_Word n_descsz;			/* Length of the note's descriptor.  */
  Elf64_Word n_type;			/* Type of the note.  */
} Elf64_Nhdr;

/* Known names of notes.  */

/* Solaris entries in the note section have this name.  */
#define ELF_NOTE_SOLARIS	"SUNW Solaris"

/* Note entries for GNU systems have this name.  */
#define ELF_NOTE_GNU		"GNU"


/* Defined types of notes for Solaris.  */

/* Value of descriptor (one word) is desired pagesize for the binary.  */
#define ELF_NOTE_PAGESIZE_HINT	1


/* Defined note types for GNU systems.  */

/* ABI information.  The descriptor consists of words:
   word 0: OS descriptor
   word 1: major version of the ABI
   word 2: minor version of the ABI
   word 3: subminor version of the ABI
*/
#define ELF_NOTE_ABI		1

/* Known OSes.  These value can appear in word 0 of an ELF_NOTE_ABI
   note section entry.  */
#define ELF_NOTE_OS_LINUX	0
#define ELF_NOTE_OS_GNU		1
#define ELF_NOTE_OS_SOLARIS2	2


/* Motorola 68k specific definitions.  */

/* Intel 80386 specific definitions.  */

/* SUN SPARC specific definitions.  */

/* Values for Elf64_Ehdr.e_flags.  */

#define EF_SPARCV9_MM		3
#define EF_SPARCV9_TSO		0
#define EF_SPARCV9_PSO		1
#define EF_SPARCV9_RMO		2
#define EF_SPARC_EXT_MASK	0xFFFF00
#define EF_SPARC_SUN_US1	0x000200
#define EF_SPARC_HAL_R1		0x000400

/* MIPS R3000 specific definitions.  */

/* Legal values for e_flags field of Elf32_Ehdr.  */

#define EF_MIPS_NOREORDER   1		/* A .noreorder directive was used */
#define EF_MIPS_PIC	    2		/* Contains PIC code */
#define EF_MIPS_CPIC	    4		/* Uses PIC calling sequence */
#define EF_MIPS_XGOT	    8
#define EF_MIPS_64BIT_WHIRL 16
#define EF_MIPS_ABI2	    32
#define EF_MIPS_ABI_ON32    64
#define EF_MIPS_ARCH	    0xf0000000	/* MIPS architecture level */

/* Legal values for MIPS architecture level.  */

#define EF_MIPS_ARCH_1	    0x00000000	/* -mips1 code.  */
#define EF_MIPS_ARCH_2	    0x10000000	/* -mips2 code.  */
#define EF_MIPS_ARCH_3	    0x20000000	/* -mips3 code.  */
#define EF_MIPS_ARCH_4	    0x30000000	/* -mips4 code.  */
#define EF_MIPS_ARCH_5	    0x40000000	/* -mips5 code.  */

/* Legal values for p_type field of Elf32_Phdr.  */

#define PT_MIPS_REGINFO	0x70000000	/* Register usage information */
#define PT_MIPS_RTPROC  0x70000001	/* Runtime procedure table. */
#define PT_MIPS_OPTIONS 0x70000002

/* Special program header types.  */

#define PF_MIPS_LOCAL	0x10000000


/* HPPA specific definitions.  */

/* Legal values for e_flags field of Elf32_Ehdr.  */

#define EF_PARISC_TRAPNL	1	/* Trap nil pointer dereference.  */
#define EF_PARISC_EXT		2	/* Program uses arch. extensions.  */
#define EF_PARISC_ARCH		0xffff0000 /* Architecture version.  */
/* Defined values are:
				0x020b	PA-RISC 1.0 big-endian
				0x0210	PA-RISC 1.1 big-endian
				0x028b	PA-RISC 1.0 little-endian
				0x0290	PA-RISC 1.1 little-endian
*/


/* Alpha specific definitions.  */

/* Legal values for e_flags field of Elf64_Ehdr.  */

#define EF_ALPHA_32BIT		1	/* All addresses must be < 2GB.  */
#define EF_ALPHA_CANRELAX	2	/* Relocations for relaxing exist.  */


/* PowerPC specific declarations */

/* ARM specific declarations */

/* Processor specific flags for the ELF header e_flags field.  */
#define EF_ARM_RELEXEC     0x01
#define EF_ARM_HASENTRY    0x02
#define EF_ARM_INTERWORK   0x04
#define EF_ARM_APCS_26     0x08
#define EF_ARM_APCS_FLOAT  0x10
#define EF_ARM_PIC         0x20
#define EF_ALIGN8          0x40		/* 8-bit structure alignment is in use */
#define EF_NEW_ABI         0x80
#define EF_OLD_ABI         0x100

/* ARM-specific program header flags */
#define PF_ARM_SB          0x10000000   /* Segment contains the location
					   addressed by the static base */

#if ELF_CLASS == ELFCLASS32
typedef Elf32_Ehdr Elf_ehdr;
typedef Elf32_Phdr Elf_phdr;
#endif

#if ELF_CLASS == ELFCLASS64
typedef Elf64_Ehdr Elf_ehdr;
typedef Elf64_Phdr Elf_phdr;
#endif

extern int elf_check_arch(Elf_ehdr *ehdr);
extern void jmp_to_elf_entry(void *entry, unsigned long buffer);
struct lb_memory;
extern int elfboot(struct lb_memory *mem);

#define FIRMWARE_TYPE "coreboot"
#define BOOTLOADER "elfboot"
#define BOOTLOADER_VERSION "1.3"

#endif	/* elf.h */
