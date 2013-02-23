#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <cpu/amd/sc520.h>


#if 0
static void irqdump(void)
{
  volatile unsigned char *irq;
  void *mmcr;


  int i;
  int irqlist[] = {0xd00, 0xd02, 0xd03, 0xd04, 0xd08, 0xd0a,
	        0xd14, 0xd18, 0xd1a, 0xd1b, 0xd1c,
		0xd20, 0xd21, 0xd22, 0xd28, 0xd29,
		0xd30, 0xd31, 0xd32, 0xd33,
		0xd40, 0xd41, 0xd42, 0xd43,0xd44, 0xd45, 0xd46,
		0xd50, 0xd51, 0xd52, 0xd53,0xd54, 0xd55, 0xd56, 0xd57,0xd58, 0xd59, 0xd5a,
		-1};
  mmcr = (void *) 0xfffef000;

  printk(BIOS_ERR, "mmcr is %p\n", mmcr);
  for(i = 0; irqlist[i] >= 0; i++) {
    irq = mmcr + irqlist[i];
    printk(BIOS_ERR, "0x%x register @%p is 0x%x\n", irqlist[i], irq, *irq);
  }

}
#endif

/* TODO: finish up mmcr struct in sc520.h, and;
   - set ADDDECTL (now done in raminit.c in cpu/amd/sc520
*/
static void mainboard_enable(struct device *dev)
{
	volatile struct mmcr *mmcr = MMCRDEFAULT;

	/* currently, nothing in the device to use, so ignore it. */
	printk(BIOS_ERR, "Technologic Systems 5300 ENTER %s\n", __func__);

	/* from fuctory bios */
	/* NOTE: the following interrupt settings made interrupts work
	 * for hard drive, and serial, but not for ethernet
	 */

	printk(BIOS_ERR, "Setting up PIC\n");
	/* just do what they say and nobody gets hurt. */
	mmcr->pic.pcicr = 0 ;
	/* all ints to level */
	mmcr->pic.mpicmode = 0;
	mmcr->pic.sl1picmode = 0;
	mmcr->pic.sl2picmode = 0;

	mmcr->pic.intpinpol = 0x100;

	mmcr->pic.pit0map = 1;
	mmcr->pic.uart1map = 0x0c;
	mmcr->pic.uart2map = 0x0b;
	mmcr->pic.rtcmap  = 0x03;
	mmcr->pic.ferrmap = 0x00;
	mmcr->pic.intpinpol = 0x100;

	mmcr->pic.gp0imap = 0x00;
	mmcr->pic.gp1imap = 0x02;
	mmcr->pic.gp2imap = 0x07;
	mmcr->pic.gp3imap = 0x05;
	mmcr->pic.gp4imap = 0x06;
	mmcr->pic.gp5imap = 0x0d;
	mmcr->pic.gp6imap = 0x15;
	mmcr->pic.gp7imap = 0x16;
	mmcr->pic.gp8imap = 0x3;
	mmcr->pic.gp9imap = 0x4;
	mmcr->pic.gp10imap = 0x9;

	// irqdump();

	printk(BIOS_ERR, "Setting up sysarb\n");
	mmcr->dbctl.dbctl = 0x01;
	mmcr->sysarb.ctl = 0x00;
	mmcr->sysarb.menb = 0x1f;
	mmcr->sysarb.prictl = 0x40000f0f;

	/* this is bios setting, depends on sysarb above */
	mmcr->hostbridge.ctl = 0x0;
	mmcr->hostbridge.tgtirqctl = 0x0;
	mmcr->hostbridge.tgtirqsta = 0xf00;
	mmcr->hostbridge.mstirqctl = 0x0;
	mmcr->hostbridge.mstirqsta = 0x708;

	printk(BIOS_ERR, "Setting up pio\n");
	/* pio */
	mmcr->pio.pfs15_0 = 0xffff;
	mmcr->pio.pfs31_16 = 0xffff;
	mmcr->pio.cspfs = 0xfe;
	mmcr->pio.clksel = 0x13;
	mmcr->pio.dsctl = 0x200;
	mmcr->pio.data15_0 = 0xde04;
	mmcr->pio.data31_16 = 0xef9f;

	printk(BIOS_ERR, "Setting up sysmap\n");
	/* system memory map */
	mmcr->sysmap.adddecctl = 0x04;
	mmcr->sysmap.wpvsta = 0x8006;
	mmcr->sysmap.par[1] = 0x340f0070;
	mmcr->sysmap.par[2] = 0x380701f0;
	mmcr->sysmap.par[3] = 0x3c0103f6;
	mmcr->sysmap.par[4] = 0x2c0f0300;
	mmcr->sysmap.par[5] = 0x447c00a0;
	mmcr->sysmap.par[6] = 0xe600000c;
	mmcr->sysmap.par[7] = 0x300046e8;
	mmcr->sysmap.par[8] = 0x500400d0;
	mmcr->sysmap.par[9] = 0x281f0140;
	mmcr->sysmap.par[13] = 0x8a07c940;
	mmcr->sysmap.par[15] = 0xee00400e;

	printk(BIOS_ERR, "Setting up gpctl\n");
	mmcr->gpctl.gpcsrt = 0x01;
	mmcr->gpctl.gpcspw = 0x09;
	mmcr->gpctl.gpcsoff = 0x01;
	mmcr->gpctl.gprdw = 0x07;
	mmcr->gpctl.gprdoff = 0x02;
	mmcr->gpctl.gpwrw = 0x07;
	mmcr->gpctl.gpwroff = 0x02;

	//mmcr->reset.sysinfo = 0xdf;
	//mmcr->reset.rescfg = 0x5;
	/* their IRQ table is wrong. Just hardwire it */
	//{
	//  char pciints[4] = {15, 15, 15, 15};
	//  pci_assign_irqs(0, 12, pciints);
	//}
	/* the assigned failed but we just noticed -- there is no
	 * dma mapping, and selftest on e100 requires that dma work
	 */
	mmcr->dmacontrol.extchanmapa = 0xf210;
	mmcr->dmacontrol.extchanmapb = 0xffff;

	printk(BIOS_ERR, "TS5300 EXIT %s\n", __func__);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};

