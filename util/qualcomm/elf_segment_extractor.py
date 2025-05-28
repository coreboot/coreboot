#!/usr/bin/env python
import sys
import os
import struct
import argparse # Import the argparse module

# --- Constants for ELF parsing ---

# e_ident (first 16 bytes of ELF header)
EI_MAG0 = 0   # File identifier
EI_MAG1 = 1
EI_MAG2 = 2
EI_MAG3 = 3
EI_CLASS = 4  # File class (32-bit or 64-bit)
EI_DATA = 5   # Data encoding (endianness)
EI_VERSION = 6 # ELF header version
EI_OSABI = 7  # OS/ABI identification
EI_ABIVERSION = 8 # ABI version
EI_PAD = 9    # Start of padding bytes

# ELF Class definitions
ELFCLASSNONE = 0
ELFCLASS32 = 1 # 32-bit ELF
ELFCLASS64 = 2 # 64-bit ELF

# ELF Data encoding definitions
ELFDATANONE = 0
ELFDATA2LSB = 1 # Little-endian
ELFDATA2MSB = 2 # Big-endian

# ELF Header offsets and sizes (for 32-bit and 64-bit)
# All values are in bytes
ELF_HEADER_SIZE_32 = 52
ELF_HEADER_SIZE_64 = 64

# Program Header (Phdr) offsets and sizes (relative to start of Phdr entry)
# These vary based on ELF class (32-bit or 64-bit)

# 32-bit Program Header (Elf32_Phdr) structure
# typedef struct {
#     Elf32_Word  p_type;    /* Segment type */
#     Elf32_Off   p_offset;  /* Segment file offset */
#     Elf32_Addr  p_vaddr;   /* Segment virtual address */
#     Elf32_Addr  p_paddr;   /* Segment physical address */
#     Elf32_Word  p_filesz;  /* Segment size in file */
#     Elf32_Word  p_memsz;   /* Segment size in memory */
#     Elf32_Word  p_flags;   /* Segment flags */
#     Elf32_Word  p_align;   /* Segment alignment */
# } Elf32_Phdr;
PHDR_32_FMT = '<IIIIIIII' # Little-endian, 8 unsigned ints (4 bytes each)
PHDR_32_SIZE = 32 # Total size of 32-bit program header entry

# 64-bit Program Header (Elf64_Phdr) structure
# typedef struct {
#     Elf64_Word  p_type;    /* Segment type */
#     Elf64_Word  p_flags;   /* Segment flags */
#     Elf64_Off   p_offset;  /* Segment file offset */
#     Elf64_Addr  p_vaddr;   /* Segment virtual address */
#     Elf64_Addr  p_paddr;   /* Segment physical address */
#     Elf64_Xword p_filesz;  /* Segment size in file */
#     Elf64_Xword p_memsz;   /* Segment size in memory */
#     Elf64_Xword p_align;   /* Segment alignment */
# } Elf64_Phdr;
# Note: p_type and p_flags are 4 bytes, others are 8 bytes
PHDR_64_FMT = '<IIQQQQQQ' # Little-endian, 2 unsigned ints (4 bytes), 6 unsigned long longs (8 bytes)
PHDR_64_SIZE = 56 # Total size of 64-bit program header entry

def parse_elf_header(f):
    """
    Parses the ELF header from the given file object and returns both parsed info and raw header bytes.

    Args:
        f (file): The file object opened in binary read mode.

    Returns:
        tuple: A tuple containing (dict: parsed ELF header information, bytes: raw ELF header data).
               Returns (None, None) if the file is not a valid ELF or unsupported.
    """
    f.seek(0)
    e_ident = f.read(16)

    # Check ELF magic bytes
    if not (e_ident[EI_MAG0] == 0x7F and
            e_ident[EI_MAG1] == ord('E') and
            e_ident[EI_MAG2] == ord('L') and
            e_ident[EI_MAG3] == ord('F')):
        print("Error: Not an ELF file (invalid magic bytes).")
        return None, None

    elf_class = e_ident[EI_CLASS]
    elf_data_encoding = e_ident[EI_DATA]

    endian_char = ''
    if elf_data_encoding == ELFDATA2LSB:
        endian_char = '<' # Little-endian
    elif elf_data_encoding == ELFDATA2MSB:
        endian_char = '>' # Big-endian
    else:
        print("Error: Unsupported ELF data encoding.")
        return None, None

    raw_header_bytes = b'' # To store the entire raw ELF header

    if elf_class == ELFCLASS32:
        # 32-bit ELF header format: 2xH, 5xI, 6xH (Total 36 bytes for remaining header)
        header_fmt = f"{endian_char}HHI IIII HHHHHH"
        header_data_rest = f.read(ELF_HEADER_SIZE_32 - 16)
        raw_header_bytes = e_ident + header_data_rest # Concatenate e_ident with the rest

        (e_type, e_machine, e_version, e_entry, e_phoff, e_shoff,
         e_flags, e_ehsize, e_phentsize, e_phnum, e_shentsize, e_shnum, e_shstrndx) = \
            struct.unpack(header_fmt, header_data_rest)
        elf_header = {
            'class': elf_class,
            'endian': endian_char,
            'e_phoff': e_phoff,
            'e_phentsize': e_phentsize,
            'e_phnum': e_phnum,
            'ehsize': ELF_HEADER_SIZE_32
        }
    elif elf_class == ELFCLASS64:
        # 64-bit ELF header format: 2xH, 1xI, 3xQ, 1xI, 6xH (Total 48 bytes for remaining header)
        header_fmt = f"{endian_char}HHI QQQ I HHHHHH"
        header_data_rest = f.read(ELF_HEADER_SIZE_64 - 16)
        raw_header_bytes = e_ident + header_data_rest # Concatenate e_ident with the rest

        (e_type, e_machine, e_version, e_entry, e_phoff, e_shoff,
         e_flags, e_ehsize, e_phentsize, e_phnum, e_shentsize, e_shnum, e_shstrndx) = \
            struct.unpack(header_fmt, header_data_rest)
        elf_header = {
            'class': elf_class,
            'endian': endian_char,
            'e_phoff': e_phoff,
            'e_phentsize': e_phentsize,
            'e_phnum': e_phnum,
            'ehsize': ELF_HEADER_SIZE_64
        }
    else:
        print("Error: Unsupported ELF class.")
        return None, None

    return elf_header, raw_header_bytes

def parse_program_header(f, elf_header, segment_index):
    """
    Parses a specific program header (segment) from the ELF file.

    Args:
        f (file): The file object.
        elf_header (dict): Parsed ELF header information.
        segment_index (int): The 0-based index of the program header to parse.

    Returns:
        dict: A dictionary containing parsed program header information (p_offset, p_filesz, p_type, p_flags).
              Returns None if the index is out of bounds or parsing fails.
    """
    e_phoff = elf_header['e_phoff']
    e_phentsize = elf_header['e_phentsize']
    e_phnum = elf_header['e_phnum']
    endian_char = elf_header['endian']
    elf_class = elf_header['class']

    if not (0 <= segment_index < e_phnum):
        print(f"Error: Segment index {segment_index} is out of bounds.")
        print(f"This ELF file has {e_phnum} segments (0 to {e_phnum - 1}).")
        return None

    # Calculate the offset to the desired program header entry
    phdr_offset = e_phoff + (segment_index * e_phentsize)
    f.seek(phdr_offset)

    if elf_class == ELFCLASS32:
        if e_phentsize != PHDR_32_SIZE:
            print(f"Warning: Unexpected phentsize for 32-bit ELF: {e_phentsize}. Expected {PHDR_32_SIZE}.")
        phdr_data = f.read(PHDR_32_SIZE)
        (p_type, p_offset, p_vaddr, p_paddr, p_filesz, p_memsz, p_flags, p_align) = \
            struct.unpack(f"{endian_char}{PHDR_32_FMT[1:]}", phdr_data)
    elif elf_class == ELFCLASS64:
        if e_phentsize != PHDR_64_SIZE:
            print(f"Warning: Unexpected phentsize for 64-bit ELF: {e_phentsize}. Expected {PHDR_64_SIZE}.")
        phdr_data = f.read(PHDR_64_SIZE)
        (p_type, p_flags, p_offset, p_vaddr, p_paddr, p_filesz, p_memsz, p_align) = \
            struct.unpack(f"{endian_char}{PHDR_64_FMT[1:]}", phdr_data)
    else:
        print("Error: Unsupported ELF class for parsing program header.")
        return None

    return {
        'p_type': p_type,
        'p_offset': p_offset,
        'p_vaddr': p_vaddr,
        'p_paddr': p_paddr,
        'p_filesz': p_filesz,
        'p_memsz': p_memsz,
        'p_flags': p_flags,
        'p_align': p_align
    }


def extract_elf_data(elf_file_path: str, output_file_path: str, extract_eh: bool, extract_pht: bool, segment_args_str: str = None, extract_hashtable: bool = False):
    """
    Extracts specified components from an ELF file (ELF Header, Program Headers, a specific Segment)
    and concatenates them into a single output binary file.

    Args:
        elf_file_path (str): The path to the ELF file.
        output_file_path (str): The path to the output binary file.
        extract_eh (bool): True to extract the ELF header, False otherwise.
        extract_pht (bool): True to extract all Program Headers, False otherwise.
        segment_args_str (str, optional): A comma-separated string of 0-based segment indices,
                                     or '$' / 'N' for the last segment. None if not extracting segments.
        extract_hashtable (bool): True to extract segments where p_type is NULL (0) and p_flags is 0x02000000.
    """
    try:
        with open(elf_file_path, 'rb') as f:
            elf_header_info, raw_elf_header_bytes = parse_elf_header(f)
            if elf_header_info is None:
                return

            output_data = b''

            # 1. Extract ELF Header if requested
            if extract_eh:
                output_data += raw_elf_header_bytes
                print(f"Included ELF Header ({len(raw_elf_header_bytes)} bytes).")

            # 2. Extract Program Header Table (PHT) if requested
            num_segments = elf_header_info['e_phnum']
            all_program_headers_bytes = b''
            if extract_pht:
                if num_segments > 0:
                    f.seek(elf_header_info['e_phoff'])
                    all_program_headers_bytes = f.read(elf_header_info['e_phentsize'] * num_segments)
                    output_data += all_program_headers_bytes
                    print(f"Included all {num_segments} Program Headers ({len(all_program_headers_bytes)} bytes).")
                else:
                    print("Warning: No Program Headers found in ELF file to extract for PHT.")

            # 3. Extract specific Segment data if requested via --segment
            if segment_args_str is not None:
                segments_to_extract_indices = []
                # Split the comma-separated string and process each segment argument
                for seg_arg in segment_args_str.split(','):
                    seg_arg = seg_arg.strip() # Remove any whitespace
                    if not seg_arg: # Skip empty strings from multiple commas (e.g., "0,,1")
                        continue

                    actual_segment_index = None
                    if seg_arg == "$" or seg_arg.upper() == "N":
                        if num_segments == 0:
                            print(f"Error: No segments found in ELF file '{elf_file_path}'. Cannot extract last segment for '{seg_arg}'.")
                            return
                        actual_segment_index = num_segments - 1
                        print(f"'{seg_arg}' detected. Auto-selecting last segment (index {actual_segment_index}).")
                    else:
                        try:
                            actual_segment_index = int(seg_arg)
                        except ValueError:
                            print(f"Error: Invalid segment index or alias '{seg_arg}'. Segment index must be an integer, '$', or 'N'.")
                            return

                    if not (0 <= actual_segment_index < num_segments):
                        print(f"Error: Segment index {actual_segment_index} is out of bounds for '{elf_file_path}'.")
                        print(f"This ELF file has {num_segments} segments (0 to {num_segments - 1}).")
                        return

                    segments_to_extract_indices.append(actual_segment_index)

                # Now extract data for each resolved segment index
                for current_segment_index in segments_to_extract_indices:
                    segment_info = parse_program_header(f, elf_header_info, current_segment_index)
                    if segment_info is None:
                        # Error message already printed by parse_program_header
                        return

                    p_offset = segment_info['p_offset']
                    p_filesz = segment_info['p_filesz']

                    if p_filesz > 0:
                        f.seek(p_offset)
                        segment_data_bytes = f.read(p_filesz)
                        output_data += segment_data_bytes
                        print(f"Included Segment {current_segment_index} data ({len(segment_data_bytes)} bytes) via --segment.")
                    else:
                        print(f"Warning: Segment {current_segment_index} has no data in the file (p_filesz is 0). Skipping via --segment.")

            # 4. Extract segments based on --hashtable criteria
            if extract_hashtable:
                print("\nSearching for segments with p_type = 0 (NULL) and p_flags = 0x02000000...")
                found_hashtable_segments = False
                for i in range(num_segments):
                    segment_info = parse_program_header(f, elf_header_info, i)
                    if segment_info is None:
                        continue # Skip if parsing failed for this segment

                    if segment_info['p_type'] == 0 and segment_info['p_flags'] == 0x02000000:
                        found_hashtable_segments = True
                        p_offset = segment_info['p_offset']
                        p_filesz = segment_info['p_filesz']

                        if p_filesz > 0:
                            f.seek(p_offset)
                            segment_data_bytes = f.read(p_filesz)
                            output_data += segment_data_bytes
                            print(f"Included Segment {i} (Type: NULL, Flags: 0x{segment_info['p_flags']:x}) data ({len(segment_data_bytes)} bytes) via --hashtable.")
                        else:
                            print(f"Warning: Segment {i} (Type: NULL, Flags: 0x{segment_info['p_flags']:x}) has no data in the file (p_filesz is 0). Skipping via --hashtable.")
                if not found_hashtable_segments:
                    print("No segments found matching --hashtable criteria.")

            # Write the concatenated data to the output file
            if output_data:
                try:
                    with open(output_file_path, 'wb') as out_f:
                        out_f.write(output_data)
                    print(f"\nSuccessfully wrote {len(output_data)} bytes to '{output_file_path}'")
                except IOError as io_e:
                    print(f"Error writing to output file '{output_file_path}': {io_e}")
                except Exception as write_e:
                    print(f"An unexpected error occurred while writing: {write_e}")
            else:
                print("No data extracted based on the provided options. Output file was not created.")


    except FileNotFoundError:
        print(f"Error: ELF file not found at '{elf_file_path}'")
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Extracts ELF Header, Program Headers, and/or specific Segments from an ELF file.",
        formatter_class=argparse.RawTextHelpFormatter
    )

    parser.add_argument(
        '--eh',
        action='store_true',
        help='Extract the ELF Header.'
    )
    parser.add_argument(
        '--pht',
        action='store_true',
        help='Extract all Program Headers (Program Header Table).'
    )
    parser.add_argument(
        '--segment',
        type=str,
        help='Specify a comma-separated list of 0-based segment indices to extract (e.g., "0,1,2").\n'
             'Use "$" or "N" for the last segment (e.g., "0,N"). Segments will be concatenated in order.'
    )
    parser.add_argument(
        '--hashtable',
        action='store_true',
        help='Extract all segments where p_type is NULL (0) and p_flags is 0x02000000.'
    )
    parser.add_argument(
        'elf_file_path',
        type=str,
        help='Path to the input ELF file.'
    )
    parser.add_argument(
        'output_file_path',
        type=str,
        help='Path to the output binary file where extracted data will be concatenated.'
    )

    args = parser.parse_args()

    # Ensure at least one extraction option is provided
    if not args.eh and not args.pht and args.segment is None and not args.hashtable:
        parser.error("At least one extraction option (--eh, --pht, --segment, or --hashtable) must be provided.")

    extract_elf_data(
        args.elf_file_path,
        args.output_file_path,
        args.eh,
        args.pht,
        args.segment,
        args.hashtable # Pass the new argument to the function
    )
