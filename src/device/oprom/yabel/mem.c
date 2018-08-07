/******************************************************************************
 * Copyright (c) 2004, 2008 IBM Corporation
 * Copyright (c) 2009 Pattrick Hueper <phueper@hueper.net>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer
 *   in the documentation and/or other materials provided with the
 *   distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *     IBM Corporation - initial implementation
 *****************************************************************************/

#include <types.h>
#include "debug.h"
#include "device.h"
#include "x86emu/x86emu.h"
#include "biosemu.h"
#include "mem.h"
#include "compat/time.h"
#include <device/resource.h>

#if !IS_ENABLED(CONFIG_YABEL_DIRECTHW) || !IS_ENABLED(CONFIG_YABEL_DIRECTHW)

// define a check for access to certain (virtual) memory regions (interrupt handlers, BIOS Data Area, ...)
#if IS_ENABLED(CONFIG_X86EMU_DEBUG)
static u8 in_check = 0;	// to avoid recursion...

static inline void DEBUG_CHECK_VMEM_READ(u32 _addr, u32 _rval)
{
	u16 ebda_segment;
	u32 ebda_size;
	if (!((debug_flags & DEBUG_CHECK_VMEM_ACCESS) && (in_check == 0)))
		return;
	in_check = 1;
	/* determine ebda_segment and size
	* since we are using my_rdx calls, make sure, this is after setting in_check! */
	/* offset 03 in BDA is EBDA segment */
	ebda_segment = my_rdw(0x40e);
	/* first value in ebda is size in KB */
	ebda_size = my_rdb(ebda_segment << 4) * 1024;
	/* check Interrupt Vector Access (0000:0000h - 0000:0400h) */
	if (_addr < 0x400) {
		DEBUG_PRINTF_CS_IP("%s: read from Interrupt Vector %x --> %x\n",
				__func__, _addr / 4, _rval);
	}
	/* access to BIOS Data Area (0000:0400h - 0000:0500h)*/
	else if ((_addr >= 0x400) && (_addr < 0x500)) {
		DEBUG_PRINTF_CS_IP("%s: read from BIOS Data Area: addr: %x --> %x\n",
				__func__, _addr, _rval);
		/* dump registers */
		/* x86emu_dump_xregs(); */
	}
	/* access to first 64k of memory... */
	else if (_addr < 0x10000) {
		DEBUG_PRINTF_CS_IP("%s: read from segment 0000h: addr: %x --> %x\n",
				__func__, _addr, _rval);
		/* dump registers */
		/* x86emu_dump_xregs(); */
	}
	/* read from PMM_CONV_SEGMENT */
	else if ((_addr <= ((PMM_CONV_SEGMENT << 4) | 0xffff)) && (_addr >= (PMM_CONV_SEGMENT << 4))) {
		DEBUG_PRINTF_CS_IP("%s: read from PMM Segment %04xh: addr: %x --> %x\n",
				__func__, PMM_CONV_SEGMENT, _addr, _rval);
		/* HALT_SYS(); */
		/* dump registers */
		/* x86emu_dump_xregs(); */
	}
	/* read from PNP_DATA_SEGMENT */
	else if ((_addr <= ((PNP_DATA_SEGMENT << 4) | 0xffff)) && (_addr >= (PNP_DATA_SEGMENT << 4))) {
		DEBUG_PRINTF_CS_IP("%s: read from PnP Data Segment %04xh: addr: %x --> %x\n",
				__func__, PNP_DATA_SEGMENT, _addr, _rval);
		/* HALT_SYS(); */
		/* dump registers */
		/* x86emu_dump_xregs(); */
	}
	/* read from EBDA Segment */
	else if ((_addr <= ((ebda_segment << 4) | (ebda_size - 1))) && (_addr >= (ebda_segment << 4))) {
		DEBUG_PRINTF_CS_IP("%s: read from Extended BIOS Data Area %04xh, size: %04x: addr: %x --> %x\n",
				__func__, ebda_segment, ebda_size, _addr, _rval);
	}
	/* read from BIOS_DATA_SEGMENT */
	else if ((_addr <= ((BIOS_DATA_SEGMENT << 4) | 0xffff)) && (_addr >= (BIOS_DATA_SEGMENT << 4))) {
		DEBUG_PRINTF_CS_IP("%s: read from BIOS Data Segment %04xh: addr: %x --> %x\n",
				__func__, BIOS_DATA_SEGMENT, _addr, _rval);
		/* for PMM debugging */
		/*if (_addr == BIOS_DATA_SEGMENT << 4) {
			X86EMU_trace_on();
			M.x86.debug &= ~DEBUG_DECODE_NOPRINT_F;
		}*/
		/* dump registers */
		/* x86emu_dump_xregs(); */
	}
	in_check = 0;
}

static inline void DEBUG_CHECK_VMEM_WRITE(u32 _addr, u32 _val)
{
	u16 ebda_segment;
	u32 ebda_size;
	if (!((debug_flags & DEBUG_CHECK_VMEM_ACCESS) && (in_check == 0)))
		return;
	in_check = 1;
	/* determine ebda_segment and size
	 * since we are using my_rdx calls, make sure that this is after
	 * setting in_check! */
	/* offset 03 in BDA is EBDA segment */
	ebda_segment = my_rdw(0x40e);
	/* first value in ebda is size in KB */
	ebda_size = my_rdb(ebda_segment << 4) * 1024;
	/* check Interrupt Vector Access (0000:0000h - 0000:0400h) */
	if (_addr < 0x400) {
		DEBUG_PRINTF_CS_IP("%s: write to Interrupt Vector %x <-- %x\n",
				__func__, _addr / 4, _val);
	}
	/* access to BIOS Data Area (0000:0400h - 0000:0500h)*/
	else if ((_addr >= 0x400) && (_addr < 0x500)) {
		DEBUG_PRINTF_CS_IP("%s: write to BIOS Data Area: addr: %x <-- %x\n",
					__func__, _addr, _val);
		/* dump registers */
		/* x86emu_dump_xregs(); */
	}
	/* access to first 64k of memory...*/
	else if (_addr < 0x10000) {
		DEBUG_PRINTF_CS_IP("%s: write to segment 0000h: addr: %x <-- %x\n",
				__func__, _addr, _val);
	/* dump registers */
		/* x86emu_dump_xregs(); */
	}
	/* write to PMM_CONV_SEGMENT... */
	else if ((_addr <= ((PMM_CONV_SEGMENT << 4) | 0xffff)) && (_addr >= (PMM_CONV_SEGMENT << 4))) {
		DEBUG_PRINTF_CS_IP("%s: write to PMM Segment %04xh: addr: %x <-- %x\n",
				__func__, PMM_CONV_SEGMENT, _addr, _val);
		/* dump registers */
		/* x86emu_dump_xregs(); */
	}
	/* write to PNP_DATA_SEGMENT... */
	else if ((_addr <= ((PNP_DATA_SEGMENT << 4) | 0xffff)) && (_addr >= (PNP_DATA_SEGMENT << 4))) {
		DEBUG_PRINTF_CS_IP("%s: write to PnP Data Segment %04xh: addr: %x <-- %x\n",
				__func__, PNP_DATA_SEGMENT, _addr, _val);
		/* dump registers */
		/* x86emu_dump_xregs(); */
	}
	/* write to EBDA Segment... */
	else if ((_addr <= ((ebda_segment << 4) | (ebda_size - 1))) && (_addr >= (ebda_segment << 4))) {
		DEBUG_PRINTF_CS_IP("%s: write to Extended BIOS Data Area %04xh, size: %04x: addr: %x <-- %x\n",
				__func__, ebda_segment, ebda_size, _addr, _val);
	}
	/* write to BIOS_DATA_SEGMENT... */
	else if ((_addr <= ((BIOS_DATA_SEGMENT << 4) | 0xffff)) && (_addr >= (BIOS_DATA_SEGMENT << 4))) {
		DEBUG_PRINTF_CS_IP("%s: write to BIOS Data Segment %04xh: addr: %x <-- %x\n",
				__func__, BIOS_DATA_SEGMENT, _addr, _val);
		/* dump registers */
		/* x86emu_dump_xregs(); */
	}
	/* write to current CS segment... */
	else if ((_addr < ((M.x86.R_CS << 4) | 0xffff)) && (_addr > (M.x86.R_CS << 4))) {
		DEBUG_PRINTF_CS_IP("%s: write to CS segment %04xh: addr: %x <-- %x\n",
				__func__, M.x86.R_CS, _addr, _val);
		/* dump registers */
		/* x86emu_dump_xregs(); */
	}
	in_check = 0;
}
#else
static inline void DEBUG_CHECK_VMEM_READ(u32 _addr, u32 _rval) {};
static inline void DEBUG_CHECK_VMEM_WRITE(u32 _addr, u32 _val) {};
#endif

//update time in BIOS Data Area
//DWord at offset 0x6c is the timer ticks since midnight, timer is running at 18Hz
//byte at 0x70 is timer overflow (set if midnight passed since last call to interrupt 1a function 00
//cur_val is the current value, of offset 6c...
static void
update_time(u32 cur_val)
{
	//for convenience, we let the start of timebase be at midnight, we currently don't support
	//real daytime anyway...
	u64 ticks_per_day = tb_freq * 60 * 24;
	// at 18Hz a period is ~55ms, converted to ticks (tb_freq is ticks/second)
	u32 period_ticks = (55 * tb_freq) / 1000;
	u64 curr_time = get_time();
	u64 ticks_since_midnight = curr_time % ticks_per_day;
	u32 periods_since_midnight = ticks_since_midnight / period_ticks;
	// if periods since midnight is smaller than last value, set overflow
	// at BDA Offset 0x70
	if (periods_since_midnight < cur_val) {
		my_wrb(0x470, 1);
	}
	// store periods since midnight at BDA offset 0x6c
	my_wrl(0x46c, periods_since_midnight);
}

// read byte from memory
u8
my_rdb(u32 addr)
{
	unsigned long translated_addr = addr;
	u8 translated = biosemu_dev_translate_address(IORESOURCE_MEM, &translated_addr);
	u8 rval;
	if (translated != 0) {
		//translation successful, access VGA Memory (BAR or Legacy...)
		DEBUG_PRINTF_MEM("%s(%08x): access to VGA Memory\n",
				 __func__, addr);
		//DEBUG_PRINTF_MEM("%s(%08x): translated_addr: %llx\n", __func__, addr, translated_addr);
		set_ci();
		rval = *((u8 *) translated_addr);
		clr_ci();
		DEBUG_PRINTF_MEM("%s(%08x) VGA --> %02x\n", __func__, addr,
				 rval);
		return rval;
	} else if (addr > M.mem_size) {
		DEBUG_PRINTF("%s(%08x): Memory Access out of range!\n",
			     __func__, addr);
		//disassemble_forward(M.x86.saved_cs, M.x86.saved_ip, 1);
		HALT_SYS();
	} else {
		/* read from virtual memory */
		rval = *((u8 *) (M.mem_base + addr));
		DEBUG_CHECK_VMEM_READ(addr, rval);
		return rval;
	}
	return -1;
}

//read word from memory
u16
my_rdw(u32 addr)
{
	unsigned long translated_addr = addr;
	u8 translated = biosemu_dev_translate_address(IORESOURCE_MEM, &translated_addr);
	u16 rval;
	if (translated != 0) {
		//translation successful, access VGA Memory (BAR or Legacy...)
		DEBUG_PRINTF_MEM("%s(%08x): access to VGA Memory\n",
				 __func__, addr);
		//DEBUG_PRINTF_MEM("%s(%08x): translated_addr: %llx\n", __func__, addr, translated_addr);
		// check for legacy memory, because of the remapping to BARs, the reads must
		// be byte reads...
		if ((addr >= 0xa0000) && (addr < 0xc0000)) {
			//read bytes a using my_rdb, because of the remapping to BARs
			//words may not be contiguous in memory, so we need to translate
			//every address...
			rval = ((u8) my_rdb(addr)) |
			    (((u8) my_rdb(addr + 1)) << 8);
		} else {
			if ((translated_addr & (u64) 0x1) == 0) {
				// 16 bit aligned access...
				set_ci();
				rval = in16le((void *) translated_addr);
				clr_ci();
			} else {
				// unaligned access, read single bytes
				set_ci();
				rval = (*((u8 *) translated_addr)) |
				    (*((u8 *) translated_addr + 1) << 8);
				clr_ci();
			}
		}
		DEBUG_PRINTF_MEM("%s(%08x) VGA --> %04x\n", __func__, addr,
				 rval);
		return rval;
	} else if (addr > M.mem_size) {
		DEBUG_PRINTF("%s(%08x): Memory Access out of range!\n",
			     __func__, addr);
		//disassemble_forward(M.x86.saved_cs, M.x86.saved_ip, 1);
		HALT_SYS();
	} else {
		/* read from virtual memory */
		rval = in16le((void *) (M.mem_base + addr));
		DEBUG_CHECK_VMEM_READ(addr, rval);
		return rval;
	}
	return -1;
}

//read long from memory
u32
my_rdl(u32 addr)
{
	unsigned long translated_addr = addr;
	u8 translated = biosemu_dev_translate_address(IORESOURCE_MEM, &translated_addr);
	u32 rval;
	if (translated != 0) {
		//translation successful, access VGA Memory (BAR or Legacy...)
		DEBUG_PRINTF_MEM("%s(%x): access to VGA Memory\n",
				 __func__, addr);
		//DEBUG_PRINTF_MEM("%s(%08x): translated_addr: %llx\n", __func__, addr, translated_addr);
		// check for legacy memory, because of the remapping to BARs, the reads must
		// be byte reads...
		if ((addr >= 0xa0000) && (addr < 0xc0000)) {
			//read bytes a using my_rdb, because of the remapping to BARs
			//dwords may not be contiguous in memory, so we need to translate
			//every address...
			rval = ((u8) my_rdb(addr)) |
			    (((u8) my_rdb(addr + 1)) << 8) |
			    (((u8) my_rdb(addr + 2)) << 16) |
			    (((u8) my_rdb(addr + 3)) << 24);
		} else {
			if ((translated_addr & (u64) 0x3) == 0) {
				// 32 bit aligned access...
				set_ci();
				rval = in32le((void *) translated_addr);
				clr_ci();
			} else {
				// unaligned access, read single bytes
				set_ci();
				rval = (*((u8 *) translated_addr)) |
				    (*((u8 *) translated_addr + 1) << 8) |
				    (*((u8 *) translated_addr + 2) << 16) |
				    (*((u8 *) translated_addr + 3) << 24);
				clr_ci();
			}
		}
		DEBUG_PRINTF_MEM("%s(%08x) VGA --> %08x\n", __func__, addr,
				 rval);
		//HALT_SYS();
		return rval;
	} else if (addr > M.mem_size) {
		DEBUG_PRINTF("%s(%08x): Memory Access out of range!\n",
			     __func__, addr);
		//disassemble_forward(M.x86.saved_cs, M.x86.saved_ip, 1);
		HALT_SYS();
	} else {
		/* read from virtual memory */
		rval = in32le((void *) (M.mem_base + addr));
		switch (addr) {
		case 0x46c:
			//BDA Time Data, update it, before reading
			update_time(rval);
			rval = in32le((void *) (M.mem_base + addr));
			break;
		}
		DEBUG_CHECK_VMEM_READ(addr, rval);
		return rval;
	}
	return -1;
}

//write byte to memory
void
my_wrb(u32 addr, u8 val)
{
	unsigned long translated_addr = addr;
	u8 translated = biosemu_dev_translate_address(IORESOURCE_MEM, &translated_addr);
	if (translated != 0) {
		//translation successful, access VGA Memory (BAR or Legacy...)
		DEBUG_PRINTF_MEM("%s(%x, %x): access to VGA Memory\n",
				 __func__, addr, val);
		//DEBUG_PRINTF_MEM("%s(%08x): translated_addr: %llx\n", __func__, addr, translated_addr);
		set_ci();
		*((u8 *) translated_addr) = val;
		clr_ci();
	} else if (addr > M.mem_size) {
		DEBUG_PRINTF("%s(%08x): Memory Access out of range!\n",
			     __func__, addr);
		//disassemble_forward(M.x86.saved_cs, M.x86.saved_ip, 1);
		HALT_SYS();
	} else {
		/* write to virtual memory */
		DEBUG_CHECK_VMEM_WRITE(addr, val);
		*((u8 *) (M.mem_base + addr)) = val;
	}
}

void
my_wrw(u32 addr, u16 val)
{
	unsigned long translated_addr = addr;
	u8 translated = biosemu_dev_translate_address(IORESOURCE_MEM, &translated_addr);
	if (translated != 0) {
		//translation successful, access VGA Memory (BAR or Legacy...)
		DEBUG_PRINTF_MEM("%s(%x, %x): access to VGA Memory\n",
				 __func__, addr, val);
		//DEBUG_PRINTF_MEM("%s(%08x): translated_addr: %llx\n", __func__, addr, translated_addr);
		// check for legacy memory, because of the remapping to BARs, the reads must
		// be byte reads...
		if ((addr >= 0xa0000) && (addr < 0xc0000)) {
			//read bytes a using my_rdb, because of the remapping to BARs
			//words may not be contiguous in memory, so we need to translate
			//every address...
			my_wrb(addr, (u8) (val & 0x00FF));
			my_wrb(addr + 1, (u8) ((val & 0xFF00) >> 8));
		} else {
			if ((translated_addr & (u64) 0x1) == 0) {
				// 16 bit aligned access...
				set_ci();
				out16le((void *) translated_addr, val);
				clr_ci();
			} else {
				// unaligned access, write single bytes
				set_ci();
				*((u8 *) translated_addr) =
				    (u8) (val & 0x00FF);
				*((u8 *) translated_addr + 1) =
				    (u8) ((val & 0xFF00) >> 8);
				clr_ci();
			}
		}
	} else if (addr > M.mem_size) {
		DEBUG_PRINTF("%s(%08x): Memory Access out of range!\n",
			     __func__, addr);
		//disassemble_forward(M.x86.saved_cs, M.x86.saved_ip, 1);
		HALT_SYS();
	} else {
		/* write to virtual memory */
		DEBUG_CHECK_VMEM_WRITE(addr, val);
		out16le((void *) (M.mem_base + addr), val);
	}
}
void
my_wrl(u32 addr, u32 val)
{
	unsigned long translated_addr = addr;
	u8 translated = biosemu_dev_translate_address(IORESOURCE_MEM, &translated_addr);
	if (translated != 0) {
		//translation successful, access VGA Memory (BAR or Legacy...)
		DEBUG_PRINTF_MEM("%s(%x, %x): access to VGA Memory\n",
				 __func__, addr, val);
		//DEBUG_PRINTF_MEM("%s(%08x): translated_addr: %llx\n",  __func__, addr, translated_addr);
		// check for legacy memory, because of the remapping to BARs, the reads must
		// be byte reads...
		if ((addr >= 0xa0000) && (addr < 0xc0000)) {
			//read bytes a using my_rdb, because of the remapping to BARs
			//words may not be contiguous in memory, so we need to translate
			//every address...
			my_wrb(addr, (u8) (val & 0x000000FF));
			my_wrb(addr + 1, (u8) ((val & 0x0000FF00) >> 8));
			my_wrb(addr + 2, (u8) ((val & 0x00FF0000) >> 16));
			my_wrb(addr + 3, (u8) ((val & 0xFF000000) >> 24));
		} else {
			if ((translated_addr & (u64) 0x3) == 0) {
				// 32 bit aligned access...
				set_ci();
				out32le((void *) translated_addr, val);
				clr_ci();
			} else {
				// unaligned access, write single bytes
				set_ci();
				*((u8 *) translated_addr) =
				    (u8) (val & 0x000000FF);
				*((u8 *) translated_addr + 1) =
				    (u8) ((val & 0x0000FF00) >> 8);
				*((u8 *) translated_addr + 2) =
				    (u8) ((val & 0x00FF0000) >> 16);
				*((u8 *) translated_addr + 3) =
				    (u8) ((val & 0xFF000000) >> 24);
				clr_ci();
			}
		}
	} else if (addr > M.mem_size) {
		DEBUG_PRINTF("%s(%08x): Memory Access out of range!\n",
			     __func__, addr);
		//disassemble_forward(M.x86.saved_cs, M.x86.saved_ip, 1);
		HALT_SYS();
	} else {
		/* write to virtual memory */
		DEBUG_CHECK_VMEM_WRITE(addr, val);
		out32le((void *) (M.mem_base + addr), val);
	}
}
#else
u8
my_rdb(u32 addr)
{
	return rdb(addr);
}

u16
my_rdw(u32 addr)
{
	return rdw(addr);
}

u32
my_rdl(u32 addr)
{
	return rdl(addr);
}

void
my_wrb(u32 addr, u8 val)
{
	wrb(addr, val);
}

void
my_wrw(u32 addr, u16 val)
{
	wrw(addr, val);
}

void
my_wrl(u32 addr, u32 val)
{
	wrl(addr, val);
}
#endif
