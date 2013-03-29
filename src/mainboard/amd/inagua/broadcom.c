/*
 * Initialize Broadcom 5785 GbE MAC embedded in AMD A55E (Hudson-E1) Southbridge
 * by uploading a Selfboot Patch to the A55E's shadow ROM area.  The patch
 * itself supports the Broadcom 50610(M) PHY on the AMD Inagua.  It is
 * equivalent to Broadcom's SelfBoot patch V1.11 (sb5785m1.11).
 * A modified variant, selected by CONFIG_BOARD_LIPPERT_FRONTRUNNER_AF supports
 * the Micrel KSZ9021 PHY that was used on LiPPERT FrontRunner-AF (CFR-AF)
 * revision 0v0, the first prototype.  The board is history and this code now
 * serves only to document the proprietary Selfboot Patch format and how to
 * adapt it to a PHY unsupported by Broadcom.
 *
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 LiPPERT ADLINK Technology GmbH
 * (Written by Jens Rottmann <JRottmann@LiPPERTembedded.de>)
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

#include <types.h>
#include <arch/byteorder.h>
#include <console/console.h>
#include <device/device.h>	//Coreboot device access
#include <device/pci.h>
#include <delay.h>

void broadcom_init(void);

#define be16(x)		cpu_to_be16(x)	//a little easier to type
#define be(x)		cpu_to_be32(x)	//this is used a lot!

/* C forces us to specify these before defining struct selfboot_patch  :-( */
#if !CONFIG_BOARD_LIPPERT_FRONTRUNNER_AF
#define INIT1_LENGTH		9
#define INIT2_LENGTH		10
#define INIT3_LENGTH		3
#define INIT4_LENGTH		7	//this one may be 0
#define PWRDN_LENGTH		5
#else
#define INIT1_LENGTH		13
#define INIT2_LENGTH		6
#define INIT3_LENGTH		3
#define INIT4_LENGTH		11	//this one may be 0
#define PWRDN_LENGTH		4
#endif


/* The AMD A55E (Hudson-E1) Southbridge contains an integrated Gigabit Ethernet
 * MAC, however AMD's documentation merely defines the related balls (without
 * fully describing their function) and states that only Broadcom 50610(M) PHYs
 * will be supported, that's all. The Hudson register reference skips all MAC
 * registers entirely, even AMD support doesn't seem to know more about it.
 *
 * As Broadcom refused to sell us any 50610 chips or provide any docs (or indeed
 * even a price list) below $100K expected sales we had to figure out everything
 * by ourselves. *Everything* below is the result of months of detective work,
 * documented here lest it get lost:
 *
 * The AMD A55E's GbE MAC is a Broadcom 5785, which AMD obviously licensed as IP
 * core. It uses a standard RGMII/MII interface and the Broadcom drivers will
 * recognize it by its unchanged PCI ID 14E4:1699, however there are some
 * specialties.
 *
 * The 5785 MAC can detect the link with 4 additional inputs, "phy_status[3:0]",
 * 'snooping' on the PHY's LED outputs. Interpretation of the LEDs' patterns is
 * programmed with register 0x5A4 of the MAC. AMD renamed them to "GBE_STAT" and
 * won't say anything about their purpose. Appearently hardware designers are
 * expected to blindly copy the Inagua reference schematic: GBE_STAT2:
 * 0=activity; GBE_STAT[1:0]: 11=no link, 10=10Mbit, 01=100Mbit, 00=1Gbit.
 *
 * For package processing the 5785 also features a MIPS-based RISC CPU, booting
 * from an internal ROM. The firmware loads config data and supplements (e.g. to
 * support specific PHYs), named "Selfboot Patches", via the "NVRAM Interface",
 * usually from an external EEPROM. The A55E doesn't have any balls for an ext.
 * EEPROM, instead AMD added a small internal RAM. The BIOS is expected to copy
 * the correct contents into this RAM (which only supports byte access!) upon
 * each powerup. The A55E can trigger an SMI upon writes, enabling the BIOS to
 * forward any changes to an actually 'NV' location, e.g. the BIOS's SPI flash,
 * behind the scenes. AMD calls it "GEC shadow ROM", not describing what it's
 * for nor mentioning the term "NVRAM". broadcom_init() below documents a
 * procedure how to upload the patch. No SMI magic is installed, therefore
 * 'NV'RAM writes won't be persistent.
 *
 * The "Selfboot Patch" can execute simple commands at various points during
 * main firmware execution. This can be used to change config registers,
 * initialize a specific PHY or work around firmware bugs. Broadcom provides
 * suitable Patches only for their AC131 and 50610 PHYs (as binary blobs). I
 * found them in DOS\sb_patch\5785\*\sb5785*.* in Driver_14_6_4_2.zip. (Note
 * that every 32bit-word of these files must be byte-swapped before uploading
 * them to the A55E.)
 *
 * Below is a derived Patch supporting the Micrel KSZ9021 PHY used on the
 * LiPPERT CFR-AF PC/104 SBC instead, with detailled description of the format.
 * (Here in correct order for upload.)
 *
 * This Patch made Ethernet work with Linux 3.3 - without having to modify the
 * tg3.ko driver. Broadcom's Windows-Drivers still fail with "Code 10" however;
 * disassembly showed they check the PHY ID and abort, because the Micrel PHY is
 * not supported.
 */

static struct selfboot_patch {		//Watch out: all values are *BIG-ENDIAN*!

	struct {	/* Global header */
		u8 signature;		//0xA5
		u8 format;		//bits 7-3: patch format; 2-0: revision
		u8 mac_addr[6];
		u16 subsys_device;	//IDs will be loaded into PCI config space
		u16 subsys_vendor;
		u16 pci_device;		//PCI device ID; vendor is always Broadcom (0x14E4)
		u8 unknown1[8];		//?, noticed no effect
		u16 basic_config;	//?, see below
		u8 checksum;		//byte sum of header == 0
		u8 unknown2;		//?, patch rejected if changed
		u16 patch_version;	//10-8: major; 7-0: minor; 15-11: variant (1=a, 2=b, ...)
	} header;

	struct {	/* Init code */
		u8 checksum;		//byte sum of init == 0
		u8 unknown;		//?, looks unused
		u8 num_hunks;		//0x60 = 3 hunks, 0x80 = 4 hunks, other values not supported
		u8 size;		//total size of all hunk#_code[] in bytes
		u8 hunk1_when;		//mark when hunk1_code gets executed
		u8 hunk1_size;		//sizeof(hunk1_code)
		u8 hunk2_when;
		u8 hunk2_size;
		u8 hunk3_when;
		u8 hunk3_size;
		u8 hunk4_when;		//0x00 (padding) if only 3 hunks
		u8 hunk4_size;		//dito
		u32 hunk1_code[INIT1_LENGTH]; //actual commands, see below
		u32 hunk2_code[INIT2_LENGTH];
		u32 hunk3_code[INIT3_LENGTH];
		u32 hunk4_code[INIT4_LENGTH]; //missing (zero length) if only 3 hunks
	} init;

	struct {	/* Power down code */
		u8 checksum;		//byte sum of powerdown == 0
		u8 unknown;		//?, looks unused
		u8 num_hunks;		//0x20 = 1 hunk, other values not supported
		u8 size;		//total size of all hunk#_code[] in bytes
		u8 hunk1_when;		//mark when hunk1_code gets executed
		u8 hunk1_size;		//sizeof(hunk1_code)
		u16 padding;		//0x0000, hunk2 is not supported
		u32 hunk1_code[PWRDN_LENGTH]; //commands, see below
	} powerdown;

} selfboot_patch = {

/* Keep the following invariant for valid Selfboot patches */
	.header.signature = 0xA5,
	.header.format = 0x23,		//format 1 revision 3
	.header.unknown1 = { 0x61, 0xB1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	.header.checksum = 0,		//calculated later
	.header.unknown2 = 0x30,
	.init.checksum = 0,		//calculated later
	.init.unknown = 0x00,
	.init.num_hunks = sizeof(selfboot_patch.init.hunk4_code) ? 0x80 : 0x60,
	.init.size = sizeof(selfboot_patch.init.hunk1_code)
	           + sizeof(selfboot_patch.init.hunk2_code)
	           + sizeof(selfboot_patch.init.hunk3_code)
	           + sizeof(selfboot_patch.init.hunk4_code),
	.init.hunk1_size = sizeof(selfboot_patch.init.hunk1_code),
	.init.hunk2_size = sizeof(selfboot_patch.init.hunk2_code),
	.init.hunk3_size = sizeof(selfboot_patch.init.hunk3_code),
	.init.hunk4_size = sizeof(selfboot_patch.init.hunk4_code),
	.powerdown.checksum = 0,	//calculated later
	.powerdown.unknown = 0x00,
	.powerdown.num_hunks = 0x20,
	.powerdown.size = sizeof(selfboot_patch.powerdown.hunk1_code),
	.powerdown.hunk1_size = sizeof(selfboot_patch.powerdown.hunk1_code),
	.powerdown.padding = be16(0x0000),

/* Only the lines below may be adapted to your needs ... */
#if !CONFIG_BOARD_LIPPERT_FRONTRUNNER_AF
	.header.mac_addr = { 0x00, 0x10, 0x18, 0x00, 0x00, 0x00 }, //Broadcom
	.header.subsys_device = be16(0x1699),	//same as pci_device
	.header.subsys_vendor = be16(0x14E4),	//Broadcom
#else
	.header.mac_addr = { 0x00, 0x20, 0x9D, 0x00, 0x00, 0x00 }, //LiPPERT
	.header.subsys_device = be16(0x1699),	//simply kept this
	.header.subsys_vendor = be16(0x121D),	//LiPPERT
#endif
	.header.pci_device = be16(0x1699),	//Broadcom 5785 with GbE PHY
#if !CONFIG_BOARD_LIPPERT_FRONTRUNNER_AF
	.header.patch_version = be16(0x010B),	//1.11 (Broadcom's sb5785m1.11)
#else
	.header.patch_version = be16(0x110B),	//1.11b, i.e. hacked  :-)
#endif
	/* Bitfield enabling general features/codepaths in the firmware or
	 * selecting support for one of several supported PHYs?
	 * Bits not listed had no appearent effect:
	 * 14-11: any bit 1=firmware execution seemed delayed
	 * 10: 0=firmware execution seemed delayed
	 * 9,2,0: select PHY type, affects these registers, probably more
	 *  9 2 0 | reg 0x05A4  PHY reg 31  PHY 23,24,28        Notes
	 * -------+----------------------------------------------------------
	 *  0 0 0 | 0x331C71C1      -         changed   Inband Status enabled
	 *  0 1 0 | 0x3210C500      -         changed             -
	 *  0 X 1 | 0x33FF66C0   changed         -         10/100 Mbit only
	 *  1 X 0 | 0x330C5180      -            -                -
	 *  1 X 1 | 0x391C6140      -            -                -
	 */
#if !CONFIG_BOARD_LIPPERT_FRONTRUNNER_AF
	.header.basic_config = be16(0x0404),	//original for B50610
#else
	.header.basic_config = be16(0x0604),	//bit 9 set so not to mess up PHY regs, kept other bits unchanged
#endif

	/* Tag that defines when / on what occasion the commands are interpreted.
	 * Bits 2-0 = 0 i.e. possible values are 0x00, 08, 10, ..., F8.
	 * On a RISC CPU reset every tag except 0x38, A0, F0, F8 is used. 0x38
	 * seems to be run before a reset is performed(?), the other 3 I have
	 * never seen used. Generally, lower values appear to be run earlier.
	 * An "ifconfig up" with Linux' "tg3" driver causes the tags 0x50, 60,
	 * 68, 20, 70, 80 to be interpreted in this order.
	 * All tests were performed with .basic_config=0x0604.
	 */
	.init.hunk1_when = 0x10,	//only once at RISC CPU reset?
	/* Instructions are obviously a specialized bytecode interpreted by the
	 * main firmware, rather than MIPS machine code. Commands consist of 1-3
	 * 32-bit words. In the following, 0-9,A-F = hex literals, a-z,_ = variable
	 * parts, each character = 4 bits.
	 * 0610offs newvalue: write (32-bit) <newvalue> to 5785-internal shared mem at <offs>
	 * 08rgvalu: write <valu> to PHY register, <rg> = 0x20 + register number
	 * C610rgnr newvalue: write <newvalue> to MAC register <rgnr>
	 * C1F0rgnr andvalue or_value: modify MAC register <rgnr> by ANDing with <andvalue> and then ORing with <or_value>
	 * C4btrgnr: clear bit in 32-bit MAC register <rgnr>, <bt> = bit number << 3
	 * C3btrgnr: set bit, see C4...; example: command 0xC3200454 sets bit 4 of 32-bit register 0x0454
	 * CBbtrgnr: run next command only if bit (see C4...) == 1 (so far only seen before F7F0...)
	 * F7F0skip: unconditional jump i.e. skip next <skip> code bytes (only seen small positive <skip>)
	 * F7Fxaddr: call function at <addr> in main firmware? <x> = 3 or 4, bool parameter?? Wild guess!
	 * F7FFFadr somvalue: also call func. at <adr>, but with <somvalue> as parameter?? More guessing!
	 * More commands probably exist, but all code I've ever seen was kept
	 * included below, commented out if not suitable for the CFR-AF. v1.xx
	 * is Broadcom's Selfboot patch version sb5785m1.xx where the command
	 * was added, for reference see Broadcom's changelog.
	 */
	.init.hunk1_code = {
#if CONFIG_BOARD_LIPPERT_FRONTRUNNER_AF
		be(0x082B8104),					//CFR-AF: PHY0B: KSZ9021 select PHY104
		be(0x082CF0F0),					//CFR-AF: PHY0C: KSZ9021 clk/ctl skew (advised by Micrel)
		be(0x082B8105),					//CFR-AF: PHY0B: KSZ9021 select PHY105
		be(0x082C3333),					//CFR-AF: PHY0C: KSZ9021 RX data skew (empirical)
#endif
		be(0xC1F005A0), be(0xFEFFEFFF), be(0x01001000),	//v1.05 : 5A0.24,12=1: auto-clock-switch
		be(0x06100D34), be(0x00000000),			//v1.03 : MemD34: clear config vars
		be(0x06100D38), be(0x00000000),			//v1.03 :    -  |
		be(0x06100D3C), be(0x00000000),			//v1.03 : MemD3F|
	}, //-->INIT1_LENGTH!

	.init.hunk2_when = 0x30,	//after global reset, PHY reset
	.init.hunk2_code = {
#if !CONFIG_BOARD_LIPPERT_FRONTRUNNER_AF
		be(0x08370F08),					//v1.06 : PHY17: B50610 select reg. 08
		be(0x08350001),					//v1.06 : PHY15: B50610 slow link fix
		be(0x08370F00),					//v1.06 : PHY17: B50610 disable reg. 08
		be(0x083C2C00),					//v1.11 : PHY1C: B50610 Shadow 0B
#endif
		be(0xF7F301E6),					//v1.09+: ?: subroutine calls to
		be(0xF7FFF0B6), be(0x0000FFE7),			//v1.09+: ?| restore Port Mode ???
		be(0xF7FFF0F6), be(0x00008000),			//v1.09+: ?|
		be(0xF7F401E6),					//v1.09+: ?|
	}, //-->INIT2_LENGTH!

	.init.hunk3_when = 0xA8,	//?, I'd guess quite late
	.init.hunk3_code = {
		be(0xC1F03604), be(0xFFE0FFFF), be(0x00110000),	//v1.08 : 3604.20-16: 10Mb clock = 12.5MHz
	}, //-->INIT3_LENGTH!

#if !CONFIG_BOARD_LIPPERT_FRONTRUNNER_AF
	.init.hunk4_when = 0xD8,	//original for B50610
#else
	.init.hunk4_when = 0x80,	//run last, after Linux' "ifconfig up"
#endif
	.init.hunk4_code = {
#if CONFIG_BOARD_LIPPERT_FRONTRUNNER_AF
		be(0x083F4300),					//CFR-AF: PHY1F: IRQ active high
		be(0x083C0000),					//CFR-AF: PHY1C: revert driver writes
		be(0x08380000),					//CFR-AF: PHY18|
		be(0x083C0000),					//CFR-AF: PHY1C|
#endif
		be(0xCB0005A4), be(0xF7F0000C),			//v1.01 : if 5A4.0==1 -->skip next 12 bytes
#if !CONFIG_BOARD_LIPPERT_FRONTRUNNER_AF
		be(0xC61005A4), be(0x3210C500),			//v1.01 : 5A4: PHY LED mode
#else
		be(0xC61005A4), be(0x331C71CE),			//CFR-AF: 5A4: fake LED mode
#endif
		be(0xF7F00008),					//v1.01 : -->skip next 8 bytes
		be(0xC61005A4), be(0x331C71C1),			//v1.01 : 5A4: inband LED mode
		//be(0xC3200454),				//CFR-AF: 454.4: auto link polling
	}, //-->INIT4_LENGTH!

	.powerdown.hunk1_when = 0x50,	//prior to IDDQ MAC
	.powerdown.hunk1_code = {
#if !CONFIG_BOARD_LIPPERT_FRONTRUNNER_AF
		be(0x083CB001),					//v1.10 : PHY1C: IDDQ B50610 PHY
#endif
		be(0xF7F30116),					//        IDDQ PHY
		be(0xC40005A0),					//v1.09 : 5A0.0=0: Port Mode = MII
		be(0xC4180400),					//v1.09 : 400.3=0|
		be(0xC3100400),					//v1.09 : 400.2=1|
	}, //-->PWRDN_LENGTH!

};

/* Upload 'NV'RAM contents for BCM5785 GbE MAC integrated in A55E.
 * Call this from mainboard.c.
 */
void broadcom_init(void)
{
	volatile u32 *gec_base;	//Gigabit Ethernet Controller base addr
	u8 *gec_shadow;		//base addr of shadow 'NV'RAM for GbE MAC in A55E
	u8 sum;
	int i;

	gec_base = (u32*)(long)dev_find_slot(0, PCI_DEVFN(0x14, 6))->resource_list->base;
	gec_shadow = (u8*)(pci_read_config32(dev_find_slot(0, PCI_DEVFN(0x14, 3)), 0x9C) & 0xFFFFFC00);
	printk(BIOS_DEBUG, "Upload GbE 'NV'RAM contents @ 0x%08lx\n", (unsigned long)gec_shadow);

	/* Halt RISC CPU before uploading the firmware patch */
	for (i=10000; i > 0; i--) {
		gec_base[0x5004/4] = 0xFFFFFFFF; //clear CPU state
		gec_base[0x5000/4] |= (1<<10);   //issue RISC halt
		if (gec_base[0x5000/4] | (1<<10))
			break;
		udelay(10);
	}
	if (!i)
		printk(BIOS_ERR, "Failed to halt RISC CPU!\n");

	/* Calculate checksums (standard byte sum) */
	for (sum = 0, i = 0; i < sizeof(selfboot_patch.header); i++)
		sum -= ((u8*)&selfboot_patch.header)[i];
	selfboot_patch.header.checksum = sum;
	for (sum = 0, i = 0; i < sizeof(selfboot_patch.init); i++)
		sum -= ((u8*)&selfboot_patch.init)[i];
	selfboot_patch.init.checksum = sum;
	for (sum = 0, i = 0; i < sizeof(selfboot_patch.powerdown); i++)
		sum -= ((u8*)&selfboot_patch.powerdown)[i];
	selfboot_patch.powerdown.checksum = sum;

	/* Upload firmware patch to shadow 'NV'RAM */
	for (i = 0; i < sizeof(selfboot_patch); i++)
		gec_shadow[i] = ((u8*)&selfboot_patch)[i]; //access byte-wise!

	/* Restart BCM5785's CPU */
	gec_base[0x5004/4] = 0xFFFFFFFF; //clear CPU state
	gec_base[0x5000/4] = 0x00000001; //reset RISC processor
	//usually we'd have to wait for the reset bit to clear again ...
}
