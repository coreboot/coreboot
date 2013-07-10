#ifndef ELF_BOOT_H
#define ELF_BOOT_H

#include <stdint.h>

/* This defines the structure of a table of parameters useful for ELF
 * bootable images.  These parameters are all passed and generated
 * by the bootloader to the booted image.  For simplicity and
 * consistency the Elf Note format is reused.
 *
 * All of the information must be Position Independent Data.
 * That is it must be safe to relocate the whole ELF boot parameter
 * block without changing the meaning or correctness of the data.
 * Additionally it must be safe to permute the order of the ELF notes
 * to any possible permutation without changing the meaning or correctness
 * of the data.
 *
 */

#define ELF_HEAD_SIZE		(8*1024)
#define ELF_BOOT_MAGIC		0x0E1FB007

typedef uint16_t Elf_Half;
typedef uint32_t Elf_Word;
typedef uint64_t Elf_Xword;

typedef struct
{
	Elf_Word b_signature; /* "0x0E1FB007" */
	Elf_Word b_size;
	Elf_Half b_checksum;
	Elf_Half b_records;
} Elf_Bhdr;

typedef struct
{
	Elf_Word n_namesz;		/* Length of the note's name.  */
	Elf_Word n_descsz;		/* Length of the note's descriptor.  */
	Elf_Word n_type;		/* Type of the note.  */
} Elf_Nhdr;


/* For standard notes n_namesz must be zero */
/* All of the following standard note types provide a single null
 * terminated string in the descriptor.
 */
#define EBN_FIRMWARE_TYPE	0x00000001
/* On platforms that support multiple classes of firmware this field
 * specifies the class of firmware you are loaded under.
 */
#define EBN_BOOTLOADER_NAME	0x00000002
/* This specifies just the name of the bootloader for easy comparison */
#define EBN_BOOTLOADER_VERSION	0x00000003
/* This specifies the version of the bootloader */
#define EBN_COMMAND_LINE	0x00000004
/* This specifies a command line that can be set by user interaction,
 * and is provided as a free form string to the loaded image.
 */


/* Standardized Elf image notes for booting... The name for all of these is ELFBoot */

#define ELF_NOTE_BOOT		"ELFBoot"

#define EIN_PROGRAM_NAME	0x00000001
/* The program in this ELF file */
#define EIN_PROGRAM_VERSION	0x00000002
/* The version of the program in this ELF file */
#define EIN_PROGRAM_CHECKSUM	0x00000003
/* ip style checksum of the memory image. */


/* Linux image notes for booting... The name for all of these is Linux */

#define LINUX_NOTE_BOOT		"Linux"

#define LIN_COMMAND_LINE	0x00000001
/* The command line to pass to the loaded kernel. */
#define LIN_ROOT_DEV		0x00000002
/* The root dev to pass to the loaded kernel. */
#define LIN_RAMDISK_FLAGS	0x00000003
/* Various old ramdisk flags */
#define LIN_INITRD_START	0x00000004
/* Start of the ramdisk in bytes */
#define LIN_INITRD_SIZE		0x00000005
/* Size of the ramdisk in bytes */


#endif /* ELF_BOOT_H */
