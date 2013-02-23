#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <cpu/amd/sc520.h>


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

/* TODO: finish up mmcr struct in sc520.h, and;
   - set ADDDECTL (now done in raminit.c in cpu/amd/sc520
*/
static void mainboard_enable(struct device *dev)
{
	//volatile struct mmcrpic *pic = MMCRPIC;
	volatile struct mmcr *mmcr = MMCRDEFAULT;

	/* msm586seg has this register set to a weird value.
	 * follow the board, not the manual!
	 */

	/* currently, nothing in the device to use, so ignore it. */
	printk(BIOS_ERR, "digital logic msm586 seg ENTER %s\n", __func__);


	/* from fuctory bios */
	/* NOTE: the following interrupt settings made interrupts work
	 * for hard drive, and serial, but not for ethernet
	 */
	/* just do what they say and nobody gets hurt. */
	mmcr->pic.pcicr = 0 ; // M_GINT_MODE | M_S1_MODE | M_S2_MODE;
	/* all ints to level */
	mmcr->pic.mpicmode = 0;
	mmcr->pic.sl1picmode = 0;
	mmcr->pic.sl2picmode = 0x80;

	mmcr->pic.intpinpol = 0;

	mmcr->pic.pit0map = 1;
	mmcr->pic.uart1map = 0xc;
	mmcr->pic.uart2map = 0xb;
	mmcr->pic.rtcmap = 3;
	mmcr->pic.ferrmap = 8;
	mmcr->pic.gp0imap = 6;
	mmcr->pic.gp1imap = 2;
	mmcr->pic.gp2imap = 7;
	mmcr->pic.gp6imap = 0x15;
	mmcr->pic.gp7imap = 0x16;
	mmcr->pic.gp10imap = 0x9;
	mmcr->pic.gp9imap = 0x4;

	irqdump();
	printk(BIOS_ERR, "uart 1 ctl is 0x%x\n", *(unsigned char *) 0xfffefcc0);

	printk(BIOS_ERR, "0xc20 ctl is 0x%x\n", *(unsigned short *) 0xfffefc20);
	printk(BIOS_ERR, "0xc22 0x%x\n", *(unsigned short *) 0xfffefc22);

	/* The following block has NOT proven sufficient to get
	 * the VGA hardware to talk to us
	 */
	/* let's set some mmcr stuff per the BIOS settings */
	mmcr->dbctl.dbctl = 0x10;
	mmcr->sysarb.ctl = 6;
	mmcr->sysarb.menb = 0xf;
	mmcr->sysarb.prictl = 0xc0000f0f;
	/* this is bios setting, depends on sysarb above */
	mmcr->hostbridge.ctl = 0x108;
	printk(BIOS_ERR, "digital logic msm586 seg EXIT %s\n", __func__);

	/* pio */
	mmcr->pio.data31_16 = 0xffbf;

	/* pci stuff */
	mmcr->pic.pciintamap = 0xa;

	/* END block where vga hardware still will not talk to us */
	/* all we get from VGA I/O addresses are ffff etc.
	 */
	mmcr->sysmap.adddecctl = 0x10;

	/* VGA now talks to us, so this adddecctl was the trick.
	 * still no interrupts from enet.
	 * Let's try fixing the piodata stuff, as there may be
	 * some wire there not documented.
	 */
	mmcr->pio.data31_16 = 0xffbf;
	/* also, our sl?picmode needs to match fuctory bios */
	mmcr->pic.sl1picmode = 0x80;
	mmcr->pic.sl2picmode = 0x0;
	/* and, finally, they do set gp5imap and we don't.
	 */
	mmcr->pic.gp5imap = 0xd;
	/* remaining problem: almost certainly, the irq table is bogus
	 * NO SHOCK as it came from fuctory bios.
	 * but let's try these 4 changes for now and see what shakes.
	 */
	/* still not interrupts. */
	/* their IRQ table is wrong. Just hardwire it */
	{
	  unsigned char pciints[4] = {15, 15, 15, 15};
	  pci_assign_irqs(0, 12, pciints);
	}
	/* the assigned failed but we just noticed -- there is no
	 * dma mapping, and selftest on e100 requires that dma work
	 */
	/* follow fuctory here */
	mmcr->dmacontrol.extchanmapa = 0x3210;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};

