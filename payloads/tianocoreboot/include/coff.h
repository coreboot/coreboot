/*
 * This file is part of the TianoCoreBoot project.
 *
 * Copyright (C) 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

typedef struct {
	 uint8_t  signature[2];
	 uint16_t lastsize;
	 uint16_t nblocks;
	 uint16_t nreloc;
	 uint16_t hdrsize;
	 uint16_t minalloc;
	 uint16_t maxalloc;
	 uint16_t ss;
	 uint16_t sp;
	 uint16_t checksum;
	 uint16_t ip;
	 uint16_t cs;
	 uint16_t relocpos;
	 uint16_t noverlay;
	 uint16_t reserved1[4];
	 uint16_t oem_id;
	 uint16_t oem_info;
	 uint16_t reserved2[10];
	 uint32_t e_lfanew;
} dos_header_t;

typedef struct {
	uint8_t  signature[4];
	uint16_t Machine;
	uint16_t NumberOfSections;
	uint32_t TimeDateStamp;
	uint32_t PointerToSymbolTable;
	uint32_t NumberOfSymbols;
	uint16_t SizeOfOptionalHeader;
	uint16_t Characteristics;
} coff_header_t;

typedef struct {
	uint32_t VirtualAddress;
	uint32_t Size;
} data_directory;

typedef struct {
	uint16_t signature;
	uint8_t  MajorLinkerVersion;
	uint8_t  MinorLinkerVersion;
	uint32_t SizeOfCode;
	uint32_t SizeOfInitializedData;
	uint32_t SizeOfUninitializedData;
	uint32_t AddressOfEntryPoint;
	uint32_t BaseOfCode;
	uint32_t BaseOfData;
	uint32_t ImageBase;
	uint32_t SectionAlignment;
	uint32_t FileAlignment;
	uint16_t MajorOSVersion;
	uint16_t MinorOSVersion;
	uint16_t MajorImageVersion;
	uint16_t MinorImageVersion;
	uint16_t MajorSubsystemVersion;
	uint16_t MinorSubsystemVersion;
	uint32_t Reserved;
	uint32_t SizeOfImage;
	uint32_t SizeOfHeaders;
	uint32_t Checksum;
	uint16_t Subsystem;
	uint16_t DLLCharacteristics;
	uint32_t SizeOfStackReserve;
	uint32_t SizeOfStackCommit;
	uint32_t SizeOfHeapReserve;
	uint32_t SizeOfHeapCommit;
	uint32_t LoaderFlags;
	uint32_t NumberOfRvaAndSizes;
	data_directory DataDirectory[16];
} pe_opt_header_t;

typedef struct {
	uint8_t  SectionName[8];
	uint32_t Size;
	uint32_t RVA;
	uint32_t PhysicalSizeOnDisk;
	uint32_t PhysicalLocationOnDisk;
	uint8_t  Reserved[12];
	uint32_t SectionFlags;
} pe_section_t;

typedef struct {
	uint32_t  VirtualAddress;
	uint32_t  SizeOfBlock;
} relocation_t;

