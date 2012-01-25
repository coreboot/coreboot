/*
 * dump mmcr of Elan520 uController (incomplete, see 22005b pg23+).
 *
 * Copyright 2005 Ronald G. Minnich
 * Copyright 2006 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>
#include "../../src/include/cpu/amd/sc520.h"

#define val(x,y) (x->y)

int print_mmcr(struct mmcr *mmcr)
{
	int i;
	printf("revid is 0x%x\n\n", val(mmcr, revid));
	printf("cpucontrol is 0x%x\n\n", val(mmcr, cpucontrol));

	printf("\n");
	printf("drcctl is 0x%x\n", val(mmcr, memregs.drcctl));
	printf("drctmctl is 0x%x\n", val(mmcr, memregs.drctmctl));
	printf("drccfg is 0x%x\n", val(mmcr, memregs.drccfg));
	printf("bendaddr is 0x%02x%02x%02x%02x\n",
			val(mmcr, memregs.drcbendadr[3]),
			val(mmcr, memregs.drcbendadr[2]),
			val(mmcr, memregs.drcbendadr[1]),
			val(mmcr, memregs.drcbendadr[0]));
	printf("eccctl is 0x%x\n", val(mmcr, memregs.eccctl));
	printf("eccsta is 0x%x\n", val(mmcr, memregs.eccsta));
	printf("ckbpos is 0x%x\n", val(mmcr, memregs.eccckbpos));
	printf("cktest is 0x%x\n", val(mmcr, memregs.ecccktest));
	printf("sbadd is 0x%lx\n", val(mmcr, memregs.eccsbadd));
	printf("mbadd is 0x%lx\n", val(mmcr, memregs.eccmbadd));

	printf("\n");
	printf("dbctl is 0x%x\n", val(mmcr, dbctl.dbctl));

	printf("\n");
	printf("bootcs is 0x%x\n", val(mmcr, romregs.bootcs));
	printf("romcs1 is 0x%x\n", val(mmcr, romregs.romcs1));
	printf("romcs2 is 0x%x\n", val(mmcr, romregs.romcs2));

	printf("\n");
	printf("hbctl is 0x%x\n",  val(mmcr, hostbridge.ctl));
	printf("hbtgtirqctl is 0x%x\n",  val(mmcr, hostbridge.tgtirqctl));
	printf("hbtgtirqsta is 0x%x\n",  val(mmcr, hostbridge.tgtirqsta));
	printf("hbmstirqctl is 0x%x\n",  val(mmcr, hostbridge.mstirqctl));
	printf("hbmstirqsta is 0x%x\n",  val(mmcr, hostbridge.mstirqsta));
	printf("mstintadd is 0x%lx\n",  val(mmcr, hostbridge.mstintadd));


	printf("\n");
	printf("sysarbctl is 0x%x\n", val(mmcr, sysarb.ctl));
	printf("pciarbsta is 0x%x\n", val(mmcr, sysarb.sta));
	printf("sysarbmenb is 0x%x\n", val(mmcr, sysarb.menb));
	printf("arbprictl is 0x%lx\n", val(mmcr, sysarb.prictl));

	printf("\n");
	printf("adddecctl is 0x%x\n", val(mmcr, sysmap.adddecctl));
	printf("wpvsta is 0x%x\n", val(mmcr, sysmap.wpvsta));
	for (i=0; i<16; i++)
		printf("par %d is 0x%lx\n", i, val(mmcr, sysmap.par[i]));

	printf("\n");
	printf("gpecho is 0x%x\n", val(mmcr, gpctl.gpecho));
	printf("gpcsdw is 0x%x\n", val(mmcr, gpctl.gpcsdw));
	printf("gpcsqual is 0x%x\n", val(mmcr, gpctl.gpcsqual));
	printf("gpcsrt is 0x%x\n", val(mmcr, gpctl.gpcsrt));
	printf("gpcspw is 0x%x\n", val(mmcr, gpctl.gpcspw));
	printf("gpcsoff is 0x%x\n", val(mmcr, gpctl.gpcsoff));
	printf("gprdw is 0x%x\n", val(mmcr, gpctl.gprdw));
	printf("gprdoff is 0x%x\n", val(mmcr, gpctl.gprdoff));
	printf("gpwrw is 0x%x\n", val(mmcr, gpctl.gpwrw));
	printf("gpwroff is 0x%x\n", val(mmcr, gpctl.gpwroff));
	printf("gpalew is 0x%x\n", val(mmcr, gpctl.gpalew));
	printf("gpaleoff is 0x%x\n", val(mmcr, gpctl.gpaleoff));

	printf("\n");

	printf("piopfs15_0 is 0x%x\n",  val(mmcr, pio.pfs15_0));
	printf("piopfs31_16 is 0x%x\n",  val(mmcr, pio.pfs31_16));
	printf("cspfs is 0x%x\n",  val(mmcr, pio.cspfs));
	printf("clksel is 0x%x\n",  val(mmcr, pio.clksel));
	printf("dsctl is 0x%x\n",  val(mmcr, pio.dsctl));
	printf("piodir15_0 is 0x%x\n",  val(mmcr, pio.dir15_0));
	printf("piodir31_16 is 0x%x\n",  val(mmcr, pio.dir31_16));
	printf("piodata15_0 is 0x%x\n",  val(mmcr, pio.data15_0));
	printf("piodata31_16 is 0x%x\n",  val(mmcr, pio.data31_16));
	printf("pioset15_0 is 0x%x\n",  val(mmcr, pio.set15_0));
	printf("pioset31_16 is 0x%x\n",  val(mmcr, pio.set31_16));
	printf("pioclr15_0 is 0x%x\n",  val(mmcr, pio.clr15_0));
	printf("pioclr31_16 is 0x%x\n",  val(mmcr, pio.clr31_16));

	printf("swtmrmilli is 0x%x\n", val(mmcr, swtmr.swtmrmilli));
	printf("swtmrmicro is 0x%x\n", val(mmcr, swtmr.swtmrmicro));
	printf("swtmrcfg is 0x%x\n", val(mmcr, swtmr.swtmrcfg));

	printf("status  is 0x%x\n", val(mmcr, gptimers.status));
	printf("pad  is 0x%x\n", val(mmcr, gptimers.pad));

	printf("timers[0].ctl is 0x%x\n",  val(mmcr, gptimers.timer[0].ctl));
	printf("timers[0].cnt is 0x%x\n",  val(mmcr, gptimers.timer[0].cnt));
	printf("timers[0].maxcmpa is 0x%x\n",  val(mmcr, gptimers.timer[0].maxcmpa));
	printf("timers[0].maxcmpb is 0x%x\n",  val(mmcr, gptimers.timer[0].maxcmpb));

	printf("timers[1].ctl is 0x%x\n",  val(mmcr, gptimers.timer[1].ctl));
	printf("timers[1].cnt is 0x%x\n",  val(mmcr, gptimers.timer[1].cnt));
	printf("timers[1].maxcmpa is 0x%x\n",  val(mmcr, gptimers.timer[1].maxcmpa));
	printf("timers[1].maxcmpb is 0x%x\n",  val(mmcr, gptimers.timer[1].maxcmpb));
	printf("timers[2].ctl is 0x%x\n",  val(mmcr, gptimers.ctl2));
	printf("timers[2].cnt is 0x%x\n",  val(mmcr, gptimers.cnt2));
	printf("timers[2].maxcmpa is 0x%x\n",  val(mmcr, gptimers.maxcmpa2));

	printf("ctl  is 0x%x\n",  val(mmcr, watchdog.ctl));
	printf("cntll  is 0x%x\n",  val(mmcr, watchdog.cntll));
	printf("cntlh  is 0x%x\n",  val(mmcr, watchdog.cntlh));

	printf("uart 1 ctl is 0x%x\n", val(mmcr, uarts.uart[0].ctl));
	printf("uart 1 sta is 0x%x\n", val(mmcr, uarts.uart[0].sta));
	printf("uart 1 fcrshad is 0x%x\n", val(mmcr, uarts.uart[0].fcrshad));
	printf("uart 2 ctl is 0x%x\n", val(mmcr, uarts.uart[1].ctl));
	printf("uart 2 sta is 0x%x\n", val(mmcr, uarts.uart[1].sta));
	printf("uart 2 fcrshad is 0x%x\n", val(mmcr, uarts.uart[1].fcrshad));

	printf("ssi ctl is 0x%x\n", val(mmcr, ssi.ctl));
	printf("ssi xmit is 0x%x\n", val(mmcr, ssi.xmit));
	printf("ssi cmd is 0x%x\n", val(mmcr, ssi.cmd));
	printf("ssi sta is 0x%x\n", val(mmcr, ssi.sta));
	printf("ssi rcv is 0x%x\n", val(mmcr, ssi.rcv));

	printf("pcicr is 0x%x\n", val(mmcr, pic.pcicr));
	printf("mpicmode is 0x%x\n", val(mmcr, pic.mpicmode));
	printf("sl1picmode is 0x%x\n", val(mmcr, pic.sl1picmode));
	printf("sl2picmode is 0x%x\n", val(mmcr, pic.sl2picmode));
	printf("swint16_1 is 0x%x\n", val(mmcr, pic.swint16_1));
	printf("swint22_17 is 0x%x\n", val(mmcr, pic.swint22_17));
	printf("intpinpol is 0x%x\n", val(mmcr, pic.intpinpol));
	printf("pichostmap is 0x%x\n", val(mmcr, pic.pichostmap));
	printf("eccmap is 0x%x\n", val(mmcr, pic.eccmap));
	printf("gptmr0map is 0x%x\n", val(mmcr, pic.gptmr0map));
	printf("gptmr1map is 0x%x\n", val(mmcr, pic.gptmr1map));
	printf("gptmr2map is 0x%x\n", val(mmcr, pic.gptmr2map));
	printf("pit0map is 0x%x\n", val(mmcr, pic.pit0map));
	printf("pit1map is 0x%x\n", val(mmcr, pic.pit1map));
	printf("pit2map is 0x%x\n", val(mmcr, pic.pit2map));
	printf("uart1map is 0x%x\n", val(mmcr, pic.uart1map));
	printf("uart2map is 0x%x\n", val(mmcr, pic.uart2map));
	printf("pciintamap is 0x%x\n", val(mmcr, pic.pciintamap));
	printf("pciintbmap is 0x%x\n", val(mmcr, pic.pciintbmap));
	printf("pciintcmap is 0x%x\n", val(mmcr, pic.pciintcmap));
	printf("pciintdmap is 0x%x\n", val(mmcr, pic.pciintdmap));
	printf("dmabcintmap is 0x%x\n", val(mmcr, pic.dmabcintmap));
	printf("ssimap is 0x%x\n", val(mmcr, pic.ssimap));
	printf("wdtmap is 0x%x\n", val(mmcr, pic.wdtmap));
	printf("rtcmap is 0x%x\n", val(mmcr, pic.rtcmap));
	printf("wpvmap is 0x%x\n", val(mmcr, pic.wpvmap));
	printf("icemap is 0x%x\n", val(mmcr, pic.icemap));
	printf("ferrmap is 0x%x\n", val(mmcr, pic.ferrmap));
	printf("gp0imap is 0x%x\n", val(mmcr, pic.gp0imap));
	printf("gp1imap is 0x%x\n", val(mmcr, pic.gp1imap));
	printf("gp2imap is 0x%x\n", val(mmcr, pic.gp2imap));
	printf("gp3imap is 0x%x\n", val(mmcr, pic.gp3imap));
	printf("gp4imap is 0x%x\n", val(mmcr, pic.gp4imap));
	printf("gp5imap is 0x%x\n", val(mmcr, pic.gp5imap));
	printf("gp6imap is 0x%x\n", val(mmcr, pic.gp6imap));
	printf("gp7imap is 0x%x\n", val(mmcr, pic.gp7imap));
	printf("gp8imap is 0x%x\n", val(mmcr, pic.gp8imap));
	printf("gp9imap is 0x%x\n", val(mmcr, pic.gp9imap));
	printf("gp10imap is 0x%x\n", val(mmcr, pic.gp10imap));

	printf("sysinfo is 0x%x\n", val(mmcr, reset.sysinfo));
	printf("rescfg is 0x%x\n", val(mmcr, reset.rescfg));
	printf("ressta is 0x%x\n", val(mmcr, reset.ressta));


	printf("ctl is 0x%x\n", val(mmcr, dmacontrol.ctl));
	printf("mmio is 0x%x\n", val(mmcr, dmacontrol.mmio));
	printf("extchanmapa is 0x%x\n", val(mmcr, dmacontrol.extchanmapa));
	printf("extchanmapb is 0x%x\n", val(mmcr, dmacontrol.extchanmapb));
	printf("extpg0 is 0x%x\n", val(mmcr, dmacontrol.extpg0));
	printf("extpg1 is 0x%x\n", val(mmcr, dmacontrol.extpg1));
	printf("extpg2 is 0x%x\n", val(mmcr, dmacontrol.extpg2));
	printf("extpg3 is 0x%x\n", val(mmcr, dmacontrol.extpg3));
	printf("extpg5 is 0x%x\n", val(mmcr, dmacontrol.extpg5));
	printf("extpg6 is 0x%x\n", val(mmcr, dmacontrol.extpg6));
	printf("extpg7 is 0x%x\n", val(mmcr, dmacontrol.extpg7));
	printf("exttc3 is 0x%x\n", val(mmcr, dmacontrol.exttc3));
	printf("exttc5 is 0x%x\n", val(mmcr, dmacontrol.exttc5));
	printf("exttc6 is 0x%x\n", val(mmcr, dmacontrol.exttc6));
	printf("exttc7 is 0x%x\n", val(mmcr, dmacontrol.exttc7));
	printf("bcctl is 0x%x\n", val(mmcr, dmacontrol.bcctl));
	printf("bcsta is 0x%x\n", val(mmcr, dmacontrol.bcsta));
	printf("bsintenb is 0x%x\n", val(mmcr, dmacontrol.bsintenb));
	printf("bcval is 0x%x\n", val(mmcr, dmacontrol.bcval));
	printf("nxtaddl3 is 0x%x\n", val(mmcr, dmacontrol.nxtaddl3));
	printf("nxtaddh3 is 0x%x\n", val(mmcr, dmacontrol.nxtaddh3));
	printf("nxtaddl5 is 0x%x\n", val(mmcr, dmacontrol.nxtaddl5));
	printf("nxtaddh5 is 0x%x\n", val(mmcr, dmacontrol.nxtaddh5));
	printf("nxtaddl6 is 0x%x\n", val(mmcr, dmacontrol.nxtaddl6));
	printf("nxtaddh6 is 0x%x\n", val(mmcr, dmacontrol.nxtaddh6));
	printf("nxtaddl7 is 0x%x\n", val(mmcr, dmacontrol.nxtaddl7));
	printf("nxtaddh7 is 0x%x\n", val(mmcr, dmacontrol.nxtaddh7));
	printf("nxtttcl3 is 0x%x\n", val(mmcr, dmacontrol.nxtttcl3));
	printf("nxtttch3 is 0x%x\n", val(mmcr, dmacontrol.nxtttch3));
	printf("nxtttcl5 is 0x%x\n", val(mmcr, dmacontrol.nxtttcl5));
	printf("nxtttch5 is 0x%x\n", val(mmcr, dmacontrol.nxtttch5));
	printf("nxtttcl6 is 0x%x\n", val(mmcr, dmacontrol.nxtttcl6));
	printf("nxtttch6 is 0x%x\n", val(mmcr, dmacontrol.nxtttch6));
	printf("nxtttcl7 is 0x%x\n", val(mmcr, dmacontrol.nxtttcl7));
	printf("nxtttch7 is 0x%x\n", val(mmcr, dmacontrol.nxtttch7));

	return 0;
}

int map_mmcr(void)
{
	int fd_mem;
	volatile uint8_t *mmcr;
	unsigned long size=4096;

	if ((fd_mem = open("/dev/mem", O_RDWR)) < 0) {
		perror("Can not open /dev/mem");
		exit(1);
	}

	if (getpagesize() > size) {
		size = getpagesize();
	}

	mmcr = mmap(0, size, PROT_WRITE | PROT_READ, MAP_SHARED,
		    fd_mem, (off_t) (0xFFFEF000));

	if (mmcr == MAP_FAILED) {
		perror("Error MMAP /dev/mem");
		exit(1);
	}


	print_mmcr((struct mmcr *)mmcr);
#if 0

	printf("ElanSC520 uC Rev. ID  : %04x\n",*(uint16_t *)mmcr);
	printf("Am5x86 CPU Control    : %04x\n",*(uint16_t *)(mmcr+2));
	printf("\n");
	printf("SDRAM Control         : %04x\n",*(uint16_t *)(mmcr+0x10));
	printf("SDRAM Timing Control  : %04x\n",*(uint16_t *)(mmcr+0x12));
	printf("SDRAM Bank Config     : %04x\n",*(uint16_t *)(mmcr+0x14));
	printf("SDRAM Bank 0-3 Ending : %04x\n",*(uint16_t *)(mmcr+0x18));
	printf("ECC Control           : %04x\n",*(uint16_t *)(mmcr+0x20));
	printf("ECC Status            : %04x\n",*(uint16_t *)(mmcr+0x21));
	printf("ECC Check Bit Position: %04x\n",*(uint16_t *)(mmcr+0x22));
	printf("ECC Check Code Test   : %04x\n",*(uint16_t *)(mmcr+0x23));
	printf("ECC Single Bit ErrAddr: %04x\n",*(uint16_t *)(mmcr+0x24));
	printf("ECC Multi Bit ErrAddr : %04x\n",*(uint16_t *)(mmcr+0x28));
	printf("\n");
	printf("SDRAM Buffer Control  : %04x\n",*(uint16_t *)(mmcr+0x40));
	printf("\n");
	printf("BOOTCS Control        : %04x\n",*(uint16_t *)(mmcr+0x50));
	printf("BOOTCS1 Control       : %04x\n",*(uint16_t *)(mmcr+0x54));
	printf("BOOTCS2 Control       : %04x\n",*(uint16_t *)(mmcr+0x56));
	printf("\n");

	printf("Adr Decode Control    : %02x\n",*(uint8_t *)(mmcr+0x80));
	printf("WrProt Violation Stat.: %04x\n",*(uint16_t *)(mmcr+0x82));
	printf("PAR 0                 : %08x\n",*(uint32_t *)(mmcr+0x88));
	printf("PAR 1                 : %08x\n",*(uint32_t *)(mmcr+0x8C));
	printf("PAR 2                 : %08x\n",*(uint32_t *)(mmcr+0x90));
	printf("PAR 3                 : %08x\n",*(uint32_t *)(mmcr+0x94));
	printf("PAR 4                 : %08x\n",*(uint32_t *)(mmcr+0x98));
	printf("PAR 5                 : %08x\n",*(uint32_t *)(mmcr+0x9C));
	printf("PAR 6                 : %08x\n",*(uint32_t *)(mmcr+0xA0));
	printf("PAR 7                 : %08x\n",*(uint32_t *)(mmcr+0xA4));
	printf("PAR 8                 : %08x\n",*(uint32_t *)(mmcr+0xA8));
	printf("PAR 9                 : %08x\n",*(uint32_t *)(mmcr+0xAC));
	printf("PAR 10                : %08x\n",*(uint32_t *)(mmcr+0xB0));
	printf("PAR 11                : %08x\n",*(uint32_t *)(mmcr+0xB4));
	printf("PAR 12                : %08x\n",*(uint32_t *)(mmcr+0xB8));
	printf("PAR 13                : %08x\n",*(uint32_t *)(mmcr+0xBC));
	printf("PAR 14                : %08x\n",*(uint32_t *)(mmcr+0xC0));
	printf("PAR 15                : %08x\n",*(uint32_t *)(mmcr+0xC4));
#endif
	munmap((void *) mmcr, size);
	return 0;
}

int main(int argc, char *argv[])
{
	map_mmcr();
	return 0;
}
