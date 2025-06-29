#!/usr/bin/env python3
#============================================================================
#
#/** @file createxbl.py
#
# GENERAL DESCRIPTION
#   Concatentates XBL segments into one ELF image
#
# SPDX-License-Identifier: BSD-3-Clause

#**/
#
#----------------------------------------------------------------------------
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who       what, where, why
# --------   ---       ------------------------------------------------------
# 05/21/19   rissha    Added --mbn_version to add MBN header accordingly
# 03/26/18   tv        Added -e to enable extended MBNV5 support
# 09/04/15   et        Added -x and -d to embed xbl_sec ELF
# 02/11/15   ck        Fixed missing elf type check in ZI OOB feature
# 11/04/14   ck        Updated calls to mbn_tools functions
# 10/22/14   ck        Added -z option to remove out of bounds ZI segments when
#                      converting from 64 to 32
# 10/10/14   ck        Added -c option and logic to enable elf type swapping
# 09/12/14   ck        Added single file logic
# 08/29/14   ck        Added no_hash option
# 08/29/14   ck        Refactored to use proper python arguments and cleaned code
# 06/16/14   niting    xbl.mbn to xbl.elf
# 05/28/14   niting    Initial revision
#
#============================================================================
from optparse import OptionParser
import os
import sys
import shutil
import mbn_tools

PAGE_SIZE       = 4096
SEGMENT_ALIGN   = 16
ELF32_HDR_SIZE  = 52
ELF32_PHDR_SIZE = 32
ELF64_HDR_SIZE  = 64
ELF64_PHDR_SIZE = 56


##############################################################################
# main
##############################################################################
def main():
  parser = OptionParser(usage='usage: %prog [options] arguments')

  parser.add_option("-f", "--first_filepath",
                    action="store", type="string", dest="elf_inp_file1",
                    help="First ELF file to merge.")

  parser.add_option("-s", "--second_filepath",
                    action="store", type="string", dest="elf_inp_file2",
                    help="Second ELF file to merge.")

  parser.add_option("-x", "--xbl_sec_filepath",
                    action="store", type="string", dest="elf_inp_xbl_sec",
                    help="Second ELF file to merge.")

  parser.add_option("-o", "--output_filepath",
                    action="store", type="string", dest="binary_out",
                    help="Merged filename and path.")

  parser.add_option("-a", "--first_elf_arch",
                    action="store", type="string", dest="elf_1_arch",
                    help="First (and output) ELF file architecture.  '32' or '64'")

  parser.add_option("-b", "--second_elf_arch",
                    action="store", type="string", dest="elf_2_arch",
                    help="Second ELF file architecture.  '32' or '64'")

  parser.add_option("-d", "--xbl_sec_elf_arch",
                    action="store", type="string", dest="elf_xbl_sec_arch",
                    help="xbl_sec file architecture.  '32' or '64'")

  parser.add_option("-c", "--output_elf_arch",
                    action="store", type="string", dest="elf_out_arch",
                    help="Output ELF file architecture. '32' or '64'" + \
                         "  If not given defaults to first file arch.")

  parser.add_option("-n", "--no_hash",
                    action="store_true", dest="hash_image",
                    help="Disables hashing of image after merging.")

  parser.add_option("-z", "--zi_out_of_bounds",
                    action="store_true", dest="zi_oob",
                    help="Removes ZI segments that have addresses greater" + \
                         " than 32 bits when converting from a 64 to 32 bit ELF")

  parser.add_option("--mbn_version",
                    action="store", type="int", dest="mbn_version",
                    help="Add mbn header in elf image. '3', '5', '6' or '7'")


  (options, args) = parser.parse_args()
  if not options.elf_inp_file1:
    parser.error('First ELF filename not given')

  if not options.binary_out:
    parser.error('Output filename not given')

  if not options.elf_1_arch:
    parser.error('First ELF architecture not given')

  if (not options.elf_1_arch == '64') and (not options.elf_1_arch == '32'):
    parser.error('Invalid First ELF architecture given')

  # Only evaluate elf_2_arch if two files are given for merging
  if options.elf_inp_file2:
    if (not options.elf_2_arch == '64') and (not options.elf_2_arch == '32'):
      parser.error('Invalid Second ELF architecture given')

  # Only evaluate elf_xbl_sec_arch if file is given
  if options.elf_inp_xbl_sec:
    if (not options.elf_xbl_sec_arch == '64') and (not options.elf_xbl_sec_arch == '32'):
      parser.error('Invalid xbl_sec ELF architecture given')

  # If output file architecture is given ensure it is either '32' or '64'
  if options.elf_out_arch:
    if (not options.elf_out_arch == '64') and (not options.elf_out_arch == '32'):
      parser.error('Invalid Output ELF architecture given')


  gen_dict = {}

  elf_inp_file1 = options.elf_inp_file1

  # It is valid for only one file to be "merged".  This essentially just
  # strips off the section names.  If second file name is not given then
  # set elf_inp_file2 to ""
  if options.elf_inp_file2:
    elf_inp_file2 = options.elf_inp_file2
  else:
    elf_inp_file2 = ""

  # Do same for xbl_sec
  elf_inp_xbl_sec = options.elf_inp_xbl_sec if options.elf_inp_xbl_sec else ""

  binary_out = options.binary_out

  if options.elf_1_arch == '64':
    is_elf1_64_bit = True
  else:
    is_elf1_64_bit = False

  # If second filename is not given then set is_elf2_64_bit to false so it
  # can be passed even though it is not used.
  if options.elf_inp_file2:
    if options.elf_2_arch == '64':
      is_elf2_64_bit = True
    else:
      is_elf2_64_bit = False
  else:
    is_elf2_64_bit = False

  if options.elf_inp_xbl_sec:
    if options.elf_xbl_sec_arch == '64':
      is_elf_xbl_sec_64_bit = True
    else:
      is_elf_xbl_sec_64_bit = False
  else:
    is_elf_xbl_sec_64_bit = False

  # If output ELF arch is given then set is_out_elf_64_bit accordingly.
  # If not then default to be input1's setting
  if options.elf_out_arch:
    if options.elf_out_arch == '64':
      is_out_elf_64_bit = True
    else:
      is_out_elf_64_bit = False
  else:
    is_out_elf_64_bit = is_elf1_64_bit


  # Store ZI Out of Bounds value
  if not options.zi_oob:
    zi_oob_enabled = False
  else:
    zi_oob_enabled = True

  header_version = 3

  if options.elf_inp_xbl_sec:
    header_version = 5

  if options.mbn_version:
    header_version = options.mbn_version

  mbn_type = 'elf'
  header_format = 'reg'
  gen_dict['IMAGE_KEY_IMAGE_ID'] = mbn_tools.ImageType.APPSBL_IMG
  #gen_dict['IMAGE_KEY_IMAGE_SOURCE'] = 0
  #gen_dict['IMAGE_KEY_IMAGE_DEST'] = 0
  gen_dict['IMAGE_KEY_MBN_TYPE'] = mbn_type
  image_header_secflag = 'non_secure'

  source_base = os.path.splitext(str(binary_out))[0]
  target_base = os.path.splitext(str(binary_out))[0]
  merged_elf = source_base + "_merged.elf"
  source_elf = source_base + "_nohash.elf"
  target_hash = target_base + ".hash"
  target_hash_hd = target_base + "_hash.hd"
  target_phdr_elf = target_base + "_phdr.pbn"
  target_nonsec = target_base + "_combined_hash.mbn"


  #print("Input file 1:", elf_inp_file1)
  #print("Input file 2:", elf_inp_file2)
  #print("Output file:", binary_out)

  merge_elfs([],
             elf_inp_file1,
	     elf_inp_file2,
       elf_inp_xbl_sec,
	     merged_elf,
	     is_elf1_64_bit,
	     is_elf2_64_bit,
       is_elf_xbl_sec_64_bit,
	     is_out_elf_64_bit,
	     zi_oob_enabled,
	     header_version)


  # Hash the image if user did not explicitly say not to
  if options.hash_image:
    # Just copy the merged elf to the final output name
    shutil.move(merged_elf, binary_out)
  else:
    shutil.copy(merged_elf, source_elf)

    # Create hash table
    rv = mbn_tools.pboot_gen_elf([],
                                 source_elf,
				 target_hash,
                                 elf_out_file_name = target_phdr_elf,
                                 secure_type = image_header_secflag,
                                 header_version = header_version )
    if rv:
       raise RuntimeError("Failed to run pboot_gen_elf")

    # Create hash table header
    rv = mbn_tools.image_header([],
                                gen_dict,
				target_hash,
				target_hash_hd,
                         	image_header_secflag,
				elf_file_name = source_elf,
				header_version = header_version)
    if rv:
       raise RuntimeError("Failed to create image header for hash segment")

    files_to_cat_in_order = [target_hash_hd, target_hash]
    mbn_tools.concat_files (target_nonsec, files_to_cat_in_order)

    # Add the hash segment into the ELF
    mbn_tools.pboot_add_hash([],
                             target_phdr_elf,
                             target_nonsec,
			     binary_out)

  return


##############################################################################
# roundup
##############################################################################
def roundup(x, precision):
  return x if x % precision == 0 else (x + precision - (x % precision))

##############################################################################
# merge_elfs
##############################################################################
def merge_elfs(env,
               elf_in_file_name1,
               elf_in_file_name2,
               elf_in_file_xbl_sec,
               elf_out_file_name,
               is_elf1_64_bit,
               is_elf2_64_bit,
               is_elf_xbl_sec_64_bit,
	             is_out_elf_64_bit,
	             zi_oob_enabled,
	             header_version):

  [elf_header1, phdr_table1] = \
    mbn_tools.preprocess_elf_file(elf_in_file_name1)

  # Check to make sure second file path exists before using
  if elf_in_file_name2 != "":
    [elf_header2, phdr_table2] = \
      mbn_tools.preprocess_elf_file(elf_in_file_name2)

  # Check to make sure xbl_sec file path exists before using
  if elf_in_file_xbl_sec != "":
    [elf_headerxblsec, phdr_tablexblsec] = \
      mbn_tools.preprocess_elf_file(elf_in_file_xbl_sec)

  # Open Files
  elf_in_fp1 = mbn_tools.OPEN(elf_in_file_name1, "rb")
  if elf_in_file_name2 != "":
    elf_in_fp2 = mbn_tools.OPEN(elf_in_file_name2, "rb")
  if elf_in_file_xbl_sec != "":
    elf_in_fpxblsec = mbn_tools.OPEN(elf_in_file_xbl_sec, "rb")

  if elf_out_file_name is not None:
    elf_out_fp = mbn_tools.OPEN(elf_out_file_name, "wb+")


  # Calculate the new program header size.  This is dependant on the output
  # ELF type and number of program headers going into output.
  if is_out_elf_64_bit:
    phdr_total_size = elf_header1.e_phnum * ELF64_PHDR_SIZE
    phdr_total_count = elf_header1.e_phnum
  else:
    phdr_total_size = elf_header1.e_phnum * ELF32_PHDR_SIZE
    phdr_total_count = elf_header1.e_phnum


  # This logic only applies if two files are to be merged
  if elf_in_file_name2 != "":
    if is_out_elf_64_bit:
      phdr_total_size += elf_header2.e_phnum * ELF64_PHDR_SIZE
      phdr_total_count += elf_header2.e_phnum
    else:
      phdr_total_size += elf_header2.e_phnum * ELF32_PHDR_SIZE
      phdr_total_count += elf_header2.e_phnum

  # Account for xbl_sec header if included
  if elf_in_file_xbl_sec != "":
    phdr_total_count += 1
    if is_out_elf_64_bit:
      phdr_total_size += ELF64_PHDR_SIZE
    else:
      phdr_total_size += ELF32_PHDR_SIZE

  # Create a new ELF header for the output file
  if is_out_elf_64_bit:
    out_elf_header = mbn_tools.Elf64_Ehdr(b'\0' * ELF64_HDR_SIZE)
    out_elf_header.e_phoff     = ELF64_HDR_SIZE
    out_elf_header.e_ehsize    = ELF64_HDR_SIZE
    out_elf_header.e_phentsize = ELF64_PHDR_SIZE
    out_elf_header.e_machine   = 183
    out_elf_header.e_ident     = str('\x7f' + 'E' + 'L' + 'F' + \
                                 '\x02' + \
                                 '\x01' + \
                                 '\x01' + \
                                 '\x00' + \
			         '\x00' + \
                                 ('\x00' * 7))

    out_elf_header.e_entry     = elf_header1.e_entry
  else:
    out_elf_header = mbn_tools.Elf32_Ehdr(b'\0' * ELF32_HDR_SIZE)
    out_elf_header.e_phoff     = ELF32_HDR_SIZE
    out_elf_header.e_ehsize    = ELF32_HDR_SIZE
    out_elf_header.e_phentsize = ELF32_PHDR_SIZE
    out_elf_header.e_machine   = 40
    out_elf_header.e_entry       = elf_header1.e_entry
    out_elf_header.e_ident     = str('\x7f' + 'E' + 'L' + 'F' + \
                                 '\x01' + \
                                 '\x01' + \
                                 '\x01' + \
                                 '\x00' + \
  	                         '\x00' + \
                                 ('\x00' * 7))

    # Address needs to be verified that it is not greater than 32 bits
    # as it is possible to go from a 64 bit elf to 32.
    if (elf_header1.e_entry > 0xFFFFFFFF):
      print("ERROR: File 1's entry point is too large to convert.")
      exit()
    out_elf_header.e_entry     = elf_header1.e_entry

  # Common header entries
  out_elf_header.e_type        = 2
  out_elf_header.e_version     = 1
  out_elf_header.e_shoff       = 0
  out_elf_header.e_flags       = 0
  out_elf_header.e_shentsize   = 0
  out_elf_header.e_shnum       = 0
  out_elf_header.e_shstrndx    = 0


  # If ZI OOB is enabled then it is possible that a segment could be discarded
  # Scan for that instance and handle before setting e_phnum and writing header
  # Ensure ELF output is 32 bit
  if zi_oob_enabled == True and is_out_elf_64_bit == False:
    for i in range(len(phdr_table1)):
      if (phdr_table1[i].p_vaddr > 0xFFFFFFFF) or \
         (phdr_table1[i].p_paddr > 0xFFFFFFFF):
        if phdr_table1[i].p_filesz == 0:
          phdr_total_count = phdr_total_count - 1

    if elf_in_file_name2 != "":
      for i in range(len(phdr_table2)):
        if (phdr_table2[i].p_vaddr > 0xFFFFFFFF) or \
           (phdr_table2[i].p_paddr > 0xFFFFFFFF):
          if phdr_table2[i].p_filesz == 0:
            phdr_total_count = phdr_total_count - 1
    # Do not include xbl_sec in above calculation
    # xbl_sec is to be treated as a single blob


  # Now it is ok to populate the ELF header and write it out
  out_elf_header.e_phnum = phdr_total_count

  # write elf header
  if is_out_elf_64_bit == False:
    elf_out_fp.write(mbn_tools.Elf32_Ehdr.getPackedData(out_elf_header))
  else:
    elf_out_fp.write(mbn_tools.Elf64_Ehdr.getPackedData(out_elf_header))

  phdr_offset = out_elf_header.e_phoff  # offset of where to put next phdr

  # offset the start of the segments just after the program headers
  segment_offset = roundup(out_elf_header.e_phoff + phdr_total_size, PAGE_SIZE)


  # Output first elf data
  for i in range(elf_header1.e_phnum):
    curr_phdr = phdr_table1[i]

    # Copy program header piece by piece to ensure possible conversion success
    if is_out_elf_64_bit == True:
      # Converting from 32 to 64 elf requires no data size validation
      new_phdr = mbn_tools.Elf64_Phdr(b'\0' * ELF64_PHDR_SIZE)
      new_phdr.p_type   = curr_phdr.p_type
      new_phdr.p_offset = segment_offset
      new_phdr.p_vaddr  = curr_phdr.p_vaddr
      new_phdr.p_paddr  = curr_phdr.p_paddr
      new_phdr.p_filesz = curr_phdr.p_filesz
      new_phdr.p_memsz  = curr_phdr.p_memsz
      new_phdr.p_flags  = curr_phdr.p_flags
      new_phdr.p_align  = curr_phdr.p_align
    else:
      # Converting from 64 to 32 elf requires data size validation
      # Note that there is an option to discard a segment if it is only ZI
      # and its address is greater than 32 bits
      new_phdr = mbn_tools.Elf32_Phdr(b'\0' * ELF32_PHDR_SIZE)
      new_phdr.p_type   = curr_phdr.p_type
      new_phdr.p_offset = segment_offset

      if curr_phdr.p_vaddr > 0xFFFFFFFF:
        if (zi_oob_enabled == True) and (curr_phdr.p_filesz == 0):
          continue
        else:
          print("ERROR: File 1 VAddr is too large for conversion.")
          exit()
      new_phdr.p_vaddr  = curr_phdr.p_vaddr

      if curr_phdr.p_paddr > 0xFFFFFFFF:
        if (zi_oob_enabled == True) and (curr_phdr.p_filesz == 0):
          continue
        else:
          print("ERROR: File 1 PAddr is too large for conversion.")
          exit()
      new_phdr.p_paddr  = curr_phdr.p_paddr

      if curr_phdr.p_filesz > 0xFFFFFFFF:
        print("ERROR: File 1 Filesz is too large for conversion.")
        exit()
      new_phdr.p_filesz = curr_phdr.p_filesz

      if curr_phdr.p_memsz > 0xFFFFFFFF:
        print("ERROR: File 1 Memsz is too large for conversion.")
        exit()
      new_phdr.p_memsz  = curr_phdr.p_memsz

      if curr_phdr.p_flags > 0xFFFFFFFF:
        print("ERROR: File 1 Flags is too large for conversion.")
        exit()
      new_phdr.p_flags  = curr_phdr.p_flags

      if curr_phdr.p_align > 0xFFFFFFFF:
        print("ERROR: File 1 Align is too large for conversion.")
        exit()
      new_phdr.p_align  = curr_phdr.p_align


    #print("i=",i)
    #print("phdr_offset=", phdr_offset)

    # update output file location to next phdr location
    elf_out_fp.seek(phdr_offset)
    # increment phdr_offset to next location
    phdr_offset += out_elf_header.e_phentsize

    inp_data_offset = curr_phdr.p_offset # used to read data from input file

#    print("inp_data_offset=")
#    print(inp_data_offset)
#
#    print("curr_phdr.p_offset=")
#    print(curr_phdr.p_offset)
#
#    print("curr_phdr.p_filesz=")
#    print(curr_phdr.p_filesz)

    # output current phdr
    if is_out_elf_64_bit == False:
      elf_out_fp.write(mbn_tools.Elf32_Phdr.getPackedData(new_phdr))
    else:
      elf_out_fp.write(mbn_tools.Elf64_Phdr.getPackedData(new_phdr))

    # Copy the ELF segment
    bytes_written = mbn_tools.file_copy_offset(elf_in_fp1,
                 inp_data_offset,
                                               elf_out_fp,
                 new_phdr.p_offset,
                                               new_phdr.p_filesz)

    # update data segment offset to be aligned after previous segment
    segment_offset += roundup(new_phdr.p_filesz, SEGMENT_ALIGN);
  elf_in_fp1.close()

  # Output second elf data if applicable
  if elf_in_file_name2 != "":
    for i in range(elf_header2.e_phnum):
      curr_phdr = phdr_table2[i]

      # Copy program header piece by piece to ensure possible conversion success
      if is_out_elf_64_bit == True:
        # Converting from 32 to 64 elf requires no data size validation
        new_phdr = mbn_tools.Elf64_Phdr(b'\0' * ELF64_PHDR_SIZE)
        new_phdr.p_type   = curr_phdr.p_type
        new_phdr.p_offset = segment_offset
        new_phdr.p_vaddr  = curr_phdr.p_vaddr
        new_phdr.p_paddr  = curr_phdr.p_paddr
        new_phdr.p_filesz = curr_phdr.p_filesz
        new_phdr.p_memsz  = curr_phdr.p_memsz
        new_phdr.p_flags  = curr_phdr.p_flags
        new_phdr.p_align  = curr_phdr.p_align
      else:
        # Converting from 64 to 32 elf requires data size validation
        # Note that there is an option to discard a segment if it is only ZI
        # and its address is greater than 32 bits
        new_phdr = mbn_tools.Elf32_Phdr(b'\0' * ELF32_PHDR_SIZE)
        new_phdr.p_type   = curr_phdr.p_type
        new_phdr.p_offset = segment_offset

        if curr_phdr.p_vaddr > 0xFFFFFFFF:
          if (zi_oob_enabled == True) and (curr_phdr.p_filesz == 0):
            continue
          else:
            print("ERROR: File 2 VAddr is too large for conversion.")
            exit()
        new_phdr.p_vaddr  = curr_phdr.p_vaddr

        if curr_phdr.p_paddr > 0xFFFFFFFF:
          if (zi_oob_enabled == True) and (curr_phdr.p_filesz == 0):
            continue
          else:
            print("ERROR: File 2 PAddr is too large for conversion.")
            exit()
        new_phdr.p_paddr  = curr_phdr.p_paddr

        if curr_phdr.p_filesz > 0xFFFFFFFF:
          print("ERROR: File 2 Filesz is too large for conversion.")
          exit()
        new_phdr.p_filesz = curr_phdr.p_filesz

        if curr_phdr.p_memsz > 0xFFFFFFFF:
          print("ERROR: File 2 Memsz is too large for conversion.")
          exit()
        new_phdr.p_memsz  = curr_phdr.p_memsz

        if curr_phdr.p_flags > 0xFFFFFFFF:
          print("ERROR: File 2 Flags is too large for conversion.")
          exit()
        new_phdr.p_flags  = curr_phdr.p_flags

        if curr_phdr.p_align > 0xFFFFFFFF:
          print("ERROR: File 2 Align is too large for conversion.")
          exit()
        new_phdr.p_align  = curr_phdr.p_align


#     print("i=",i)
#     print("phdr_offset=", phdr_offset)

      # update output file location to next phdr location
      elf_out_fp.seek(phdr_offset)
      # increment phdr_offset to next location
      phdr_offset += out_elf_header.e_phentsize

      inp_data_offset = curr_phdr.p_offset # used to read data from input file

#     print("inp_data_offset=")
#     print(inp_data_offset)
#
#     print("curr_phdr.p_offset=")
#     print(curr_phdr.p_offset)
#
#     print("curr_phdr.p_filesz=")
#     print(curr_phdr.p_filesz)

      # output current phdr
      if is_out_elf_64_bit == False:
        elf_out_fp.write(mbn_tools.Elf32_Phdr.getPackedData(new_phdr))
      else:
        elf_out_fp.write(mbn_tools.Elf64_Phdr.getPackedData(new_phdr))

      # Copy the ELF segment
      bytes_written = mbn_tools.file_copy_offset(elf_in_fp2,
                                                 inp_data_offset,
                                                 elf_out_fp,
                                                 new_phdr.p_offset,
                                                 new_phdr.p_filesz)

      # update data segment offset to be aligned after previous segment
      segment_offset += roundup(new_phdr.p_filesz, SEGMENT_ALIGN);
    elf_in_fp2.close()

  # Embed xbl_sec image if provided
  if elf_in_file_xbl_sec != "":

    # Scan pheaders in xbl_sec for segment that contains entry point address
    entry_seg_offset = -1
    entry_addr = elf_headerxblsec.e_entry
    for i in range(elf_headerxblsec.e_phnum):
      phdr = phdr_tablexblsec[i]
      max_addr = phdr.p_vaddr + phdr.p_memsz
      if phdr.p_vaddr <= entry_addr <= max_addr:
        entry_seg_offset = phdr.p_offset
        break
    if entry_seg_offset == -1:
      print("Error: Failed to find entry point in any segment!")
      exit()
    # magical equation for program header's phys and virt addr
    phys_virt_addr = entry_addr - entry_seg_offset

    if is_out_elf_64_bit:
      # Converting from 32 to 64 elf requires no data size validation
      new_phdr = mbn_tools.Elf64_Phdr(b'\0' * ELF64_PHDR_SIZE)
      new_phdr.p_type   = 0x1
      new_phdr.p_offset = segment_offset
      new_phdr.p_vaddr  = phys_virt_addr
      new_phdr.p_paddr  = phys_virt_addr
      new_phdr.p_filesz = os.path.getsize(elf_in_file_xbl_sec)
      new_phdr.p_memsz  = new_phdr.p_filesz
      if header_version >= 5:
        new_phdr.p_flags  = (0x5 |
           (mbn_tools.MI_PBT_XBL_SEC_SEGMENT <<
            mbn_tools.MI_PBT_FLAG_SEGMENT_TYPE_SHIFT));
      else:
        new_phdr.p_flags  = 0x5
      new_phdr.p_align  = 0x1000
    else:
      # Converting from 64 to 32 elf requires data size validation
      # Don't discard the segment containing xbl_sec, simply error out
      # if the address is greater than 32 bits
      new_phdr = mbn_tools.Elf32_Phdr(b'\0' * ELF32_PHDR_SIZE)
      new_phdr.p_type   = 0x1 #
      new_phdr.p_offset = segment_offset
      if header_version >= 5:
        new_phdr.p_flags  = (0x5 |
          (mbn_tools.MI_PBT_XBL_SEC_SEGMENT <<
           mbn_tools.MI_PBT_FLAG_SEGMENT_TYPE_SHIFT));
      else:
        new_phdr.p_flags  = 0x5
      new_phdr.p_align  = 0x1000

      if phys_virt_addr > 0xFFFFFFFF:
        if zi_oob_enabled == False or curr_phdr.p_filesz != 0:
          print("ERROR: File xbl_sec VAddr or PAddr is too big for conversion.")
          exit()
      new_phdr.p_vaddr  = phys_virt_addr
      new_phdr.p_paddr  = phys_virt_addr

      if os.path.getsize(elf_in_file_xbl_sec) > 0xFFFFFFFF:
        print("ERROR: File xbl_sec Filesz is too big for conversion.")
        exit()
      new_phdr.p_filesz = os.path.getsize(elf_in_file_xbl_sec)
      new_phdr.p_memsz  = new_phdr.p_filesz


    # update output file location to next phdr location
    elf_out_fp.seek(phdr_offset)
    # increment phdr_offset to next location
    phdr_offset += out_elf_header.e_phentsize
    # Copy entire xbl_sec file, so start from byte 0
    inp_data_offset = 0

    # Output xbl_sec's phdr
    elf_in_file_xbl_sec
    if is_out_elf_64_bit == False:
      elf_out_fp.write(mbn_tools.Elf32_Phdr.getPackedData(new_phdr))
    else:
      elf_out_fp.write(mbn_tools.Elf64_Phdr.getPackedData(new_phdr))

    # Copy the ENTIRE xbl_sec image
    bytes_written = mbn_tools.file_copy_offset(elf_in_fpxblsec,
                                               inp_data_offset,
                                               elf_out_fp,
                                               new_phdr.p_offset,
                                               new_phdr.p_filesz)
    # update data segment offset to be aligned after previous segment
    # Not necessary, unless appending more pheaders after this point
    segment_offset += roundup(new_phdr.p_filesz, SEGMENT_ALIGN);

    elf_in_fpxblsec.close()

  elf_out_fp.close()

  return 0


main()
