#include <stdio.h>
#include "../../../include/cpu/amd/sc520.h"

#define offsetof(s,m) (size_t)(unsigned long)&(((s *)0)->m)
#define val(s,m) (size_t)(unsigned long)&(((s))->m)

main(){
struct mmcr *mmcr = (struct mmcr *) 0xfffef000;
printf("val of revid is 0x%x\n\n", val(mmcr, revid));
printf("val of cpucontrol is 0x%x\n\n", val(mmcr, cpucontrol));

/*
printf("val of sysarb is 0x%x\n\n", val(mmcr, sysarb));
printf("val of sysmap is 0x%x\n\n", val(mmcr, sysmap));
printf("val of gpctl is 0x%x\n\n", val(mmcr, gpctl));
printf("val of pio is 0x%x\n\n", val(mmcr, pio));
printf("val of swtmr is 0x%x\n\n", val(mmcr, swtmr));
printf("val of gptimers is 0x%x\n\n", val(mmcr, gptimers));
printf("val of watchdog is 0x%x\n\n", val(mmcr, watchdog));
printf("val of uarts is 0x%x\n\n", val(mmcr, uarts));
printf("val of ssi is 0x%x\n\n", val(mmcr, ssi));
printf("val of pic is 0x%x\n\n", val(mmcr, pic));
printf("val of reset is 0x%x\n\n", val(mmcr, reset));
printf("val of dmacontrol is 0x%x\n\n", val(mmcr, dmacontrol));
 */

printf("\n");
printf("val of drcctl is 0x%x\n", val(mmcr, memregs.drcctl));
printf("val of drctmctl is 0x%x\n", val(mmcr, memregs.drctmctl));
printf("val of drccfg is 0x%x\n", val(mmcr, memregs.drccfg));
printf("val of bendaddr is 0x%x\n", val(mmcr, memregs.drcbendadr));
printf("val of eccctl is 0x%x\n", val(mmcr, memregs.eccctl));
printf("val of eccsta is 0x%x\n", val(mmcr, memregs.eccsta));
printf("val of ckbpos is 0x%x\n", val(mmcr, memregs.eccckbpos));
printf("val of cktest is 0x%x\n", val(mmcr, memregs.ecccktest));
printf("val of sbadd is 0x%x\n", val(mmcr, memregs.eccsbadd));
printf("val of mbadd is 0x%x\n", val(mmcr, memregs.eccmbadd));

printf("\n");
printf("val of dbctl is 0x%x\n", val(mmcr, dbctl.dbctl));

printf("\n");
printf("val of bootcs is 0x%x\n", val(mmcr, romregs.bootcs));
printf("val of romcs1 is 0x%x\n", val(mmcr, romregs.romcs1));
printf("val of romcs2 is 0x%x\n", val(mmcr, romregs.romcs2));

printf("\n");
printf("val of hbctl is 0x%x\n",  val(mmcr, hostbridge.ctl));
printf("val of hbtgtirqctl is 0x%x\n",  val(mmcr, hostbridge.tgtirqctl));
printf("val of hbtgtirqsta is 0x%x\n",  val(mmcr, hostbridge.tgtirqsta));
printf("val of hbmstirqctl is 0x%x\n",  val(mmcr, hostbridge.mstirqctl));
printf("val of hbmstirqsta is 0x%x\n",  val(mmcr, hostbridge.mstirqsta));
printf("val of mstintadd is 0x%x\n",  val(mmcr, hostbridge.mstintadd));


printf("\n");
printf("val of sysarbctl is 0x%x\n", val(mmcr, sysarb.ctl));
printf("val of pciarbsta is 0x%x\n", val(mmcr, sysarb.sta));
printf("val of sysarbmenb is 0x%x\n", val(mmcr, sysarb.menb));
printf("val of arbprictl is 0x%x\n", val(mmcr, sysarb.prictl));

printf("\n");
printf("val of adddecctl is 0x%x\n", val(mmcr, sysmap.adddecctl));
printf("val of wpvsta is 0x%x\n", val(mmcr, sysmap.wpvsta));
printf("val of par is 0x%x\n", val(mmcr, sysmap.par));

printf("\n");
printf("val of gpecho is 0x%x\n", val(mmcr, gpctl.gpecho));
printf("val of gpcsdw is 0x%x\n", val(mmcr, gpctl.gpcsdw));
printf("val of gpcsqual is 0x%x\n", val(mmcr, gpctl.gpcsqual));
printf("val of gpcsrt is 0x%x\n", val(mmcr, gpctl.gpcsrt));
printf("val of gpcspw is 0x%x\n", val(mmcr, gpctl.gpcspw));
printf("val of gpcsoff is 0x%x\n", val(mmcr, gpctl.gpcsoff));
printf("val of gprdw is 0x%x\n", val(mmcr, gpctl.gprdw));
printf("val of gprdoff is 0x%x\n", val(mmcr, gpctl.gprdoff));
printf("val of gpwrw is 0x%x\n", val(mmcr, gpctl.gpwrw));
printf("val of gpwroff is 0x%x\n", val(mmcr, gpctl.gpwroff));
printf("val of gpalew is 0x%x\n", val(mmcr, gpctl.gpalew));
printf("val of gpaleoff is 0x%x\n", val(mmcr, gpctl.gpaleoff));

printf("\n");

printf("val of piopfs15_0 is 0x%x\n",  val(mmcr, pio.pfs15_0));
printf("val of piopfs31_16 is 0x%x\n",  val(mmcr, pio.pfs31_16));
printf("val of cspfs is 0x%x\n",  val(mmcr, pio.cspfs));
printf("val of clksel is 0x%x\n",  val(mmcr, pio.clksel));
printf("val of dsctl is 0x%x\n",  val(mmcr, pio.dsctl));
printf("val of piodir15_0 is 0x%x\n",  val(mmcr, pio.dir15_0));
printf("val of piodir31_16 is 0x%x\n",  val(mmcr, pio.dir31_16));
printf("val of piodata15_0 is 0x%x\n",  val(mmcr, pio.data15_0));
printf("val of piodata31_16 is 0x%x\n",  val(mmcr, pio.data31_16));
printf("val of pioset15_0 is 0x%x\n",  val(mmcr, pio.set15_0));
printf("val of pioset31_16 is 0x%x\n",  val(mmcr, pio.set31_16));
printf("val of pioclr15_0 is 0x%x\n",  val(mmcr, pio.clr15_0));
printf("val of pioclr31_16 is 0x%x\n",  val(mmcr, pio.clr31_16));

printf("val of swtmrmilli is 0x%x\n", val(mmcr, swtmr.swtmrmilli));
printf("val of swtmrmicro is 0x%x\n", val(mmcr, swtmr.swtmrmicro));
printf("val of swtmrcfg is 0x%x\n", val(mmcr, swtmr.swtmrcfg));

printf("val of status  is 0x%x\n", val(mmcr, gptimers.status));
printf("val of pad  is 0x%x\n", val(mmcr, gptimers.pad));
printf("val of timer  is 0x%x\n", val(mmcr, gptimers.timer));

printf("val of timers[0].ctl is 0x%x\n",  val(mmcr, gptimers.timer[0].ctl));
printf("val of timers[0].cnt is 0x%x\n",  val(mmcr, gptimers.timer[0].cnt));
printf("val of timers[0].maxcmpa is 0x%x\n",  val(mmcr, gptimers.timer[0].maxcmpa));
printf("val of timers[0].maxcmpb is 0x%x\n",  val(mmcr, gptimers.timer[0].maxcmpb));

printf("val of timers[1].ctl is 0x%x\n",  val(mmcr, gptimers.timer[1].ctl));
printf("val of timers[1].cnt is 0x%x\n",  val(mmcr, gptimers.timer[1].cnt));
printf("val of timers[1].maxcmpa is 0x%x\n",  val(mmcr, gptimers.timer[1].maxcmpa));
printf("val of timers[1].maxcmpb is 0x%x\n",  val(mmcr, gptimers.timer[1].maxcmpb));
printf("val of timers[2].ctl is 0x%x\n",  val(mmcr, gptimers.ctl2));
printf("val of timers[2].cnt is 0x%x\n",  val(mmcr, gptimers.cnt2));
printf("val of timers[2].maxcmpa is 0x%x\n",  val(mmcr, gptimers.maxcmpa2));

printf("val of ctl  is 0x%x\n",  val(mmcr, watchdog.ctl));
printf("val of cntll  is 0x%x\n",  val(mmcr, watchdog.cntll));
printf("val of cntlh  is 0x%x\n",  val(mmcr, watchdog.cntlh));

printf("val of uart 1 ctl is 0x%x\n", val(mmcr, uarts.uart[0].ctl));
printf("val of uart 1 sta is 0x%x\n", val(mmcr, uarts.uart[0].sta));
printf("val of uart 1 fcrshad is 0x%x\n", val(mmcr, uarts.uart[0].fcrshad));
printf("val of uart 2 ctl is 0x%x\n", val(mmcr, uarts.uart[1].ctl));
printf("val of uart 2 sta is 0x%x\n", val(mmcr, uarts.uart[1].sta));
printf("val of uart 2 fcrshad is 0x%x\n", val(mmcr, uarts.uart[1].fcrshad));

printf("val of ssi ctl is 0x%x\n", val(mmcr, ssi.ctl));
printf("val of ssi xmit is 0x%x\n", val(mmcr, ssi.xmit));
printf("val of ssi cmd is 0x%x\n", val(mmcr, ssi.cmd));
printf("val of ssi sta is 0x%x\n", val(mmcr, ssi.sta));
printf("val of ssi rcv is 0x%x\n", val(mmcr, ssi.rcv));

printf("val of pcicr is 0x%x\n", val(mmcr, pic.pcicr));
printf("val of mpicmode is 0x%x\n", val(mmcr, pic.mpicmode));
printf("val of sl1picmode is 0x%x\n", val(mmcr, pic.sl1picmode));
printf("val of sl2picmode is 0x%x\n", val(mmcr, pic.sl2picmode));
printf("val of swint16_1 is 0x%x\n", val(mmcr, pic.swint16_1));
printf("val of swint22_17 is 0x%x\n", val(mmcr, pic.swint22_17));
printf("val of intpinpol is 0x%x\n", val(mmcr, pic.intpinpol));
printf("val of pichostmap is 0x%x\n", val(mmcr, pic.pichostmap));
printf("val of eccmap is 0x%x\n", val(mmcr, pic.eccmap));
printf("val of gptmr0map is 0x%x\n", val(mmcr, pic.gptmr0map));
printf("val of gptmr1map is 0x%x\n", val(mmcr, pic.gptmr1map));
printf("val of gptmr2map is 0x%x\n", val(mmcr, pic.gptmr2map));
printf("val of pit0map is 0x%x\n", val(mmcr, pic.pit0map));
printf("val of pit1map is 0x%x\n", val(mmcr, pic.pit1map));
printf("val of pit2map is 0x%x\n", val(mmcr, pic.pit2map));
printf("val of uart1map is 0x%x\n", val(mmcr, pic.uart1map));
printf("val of uart2map is 0x%x\n", val(mmcr, pic.uart2map));
printf("val of pciintamap is 0x%x\n", val(mmcr, pic.pciintamap));
printf("val of pciintbmap is 0x%x\n", val(mmcr, pic.pciintbmap));
printf("val of pciintcmap is 0x%x\n", val(mmcr, pic.pciintcmap));
printf("val of pciintdmap is 0x%x\n", val(mmcr, pic.pciintdmap));
printf("val of dmabcintmap is 0x%x\n", val(mmcr, pic.dmabcintmap));
printf("val of ssimap is 0x%x\n", val(mmcr, pic.ssimap));
printf("val of wdtmap is 0x%x\n", val(mmcr, pic.wdtmap));
printf("val of rtcmap is 0x%x\n", val(mmcr, pic.rtcmap));
printf("val of wpvmap is 0x%x\n", val(mmcr, pic.wpvmap));
printf("val of icemap is 0x%x\n", val(mmcr, pic.icemap));
printf("val of ferrmap is 0x%x\n", val(mmcr, pic.ferrmap));
printf("val of gp0imap is 0x%x\n", val(mmcr, pic.gp0imap));
printf("val of gp1imap is 0x%x\n", val(mmcr, pic.gp1imap));
printf("val of gp2imap is 0x%x\n", val(mmcr, pic.gp2imap));
printf("val of gp3imap is 0x%x\n", val(mmcr, pic.gp3imap));
printf("val of gp4imap is 0x%x\n", val(mmcr, pic.gp4imap));
printf("val of gp5imap is 0x%x\n", val(mmcr, pic.gp5imap));
printf("val of gp6imap is 0x%x\n", val(mmcr, pic.gp6imap));
printf("val of gp7imap is 0x%x\n", val(mmcr, pic.gp7imap));
printf("val of gp8imap is 0x%x\n", val(mmcr, pic.gp8imap));
printf("val of gp9imap is 0x%x\n", val(mmcr, pic.gp9imap));
printf("val of gp10imap is 0x%x\n", val(mmcr, pic.gp10imap));

printf("val of sysinfo is 0x%x\n", val(mmcr, reset.sysinfo));
printf("val of rescfg is 0x%x\n", val(mmcr, reset.rescfg));
printf("val of ressta is 0x%x\n", val(mmcr, reset.ressta));


printf("val of ctl is 0x%x\n", val(mmcr, dmacontrol.ctl));
printf("val of mmio is 0x%x\n", val(mmcr, dmacontrol.mmio));
printf("val of extchanmapa is 0x%x\n", val(mmcr, dmacontrol.extchanmapa));
printf("val of extchanmapb is 0x%x\n", val(mmcr, dmacontrol.extchanmapb));
printf("val of extpg0 is 0x%x\n", val(mmcr, dmacontrol.extpg0));
printf("val of extpg1 is 0x%x\n", val(mmcr, dmacontrol.extpg1));
printf("val of extpg2 is 0x%x\n", val(mmcr, dmacontrol.extpg2));
printf("val of extpg3 is 0x%x\n", val(mmcr, dmacontrol.extpg3));
printf("val of extpg5 is 0x%x\n", val(mmcr, dmacontrol.extpg5));
printf("val of extpg6 is 0x%x\n", val(mmcr, dmacontrol.extpg6));
printf("val of extpg7 is 0x%x\n", val(mmcr, dmacontrol.extpg7));
printf("val of exttc3 is 0x%x\n", val(mmcr, dmacontrol.exttc3));
printf("val of exttc5 is 0x%x\n", val(mmcr, dmacontrol.exttc5));
printf("val of exttc6 is 0x%x\n", val(mmcr, dmacontrol.exttc6));
printf("val of exttc7 is 0x%x\n", val(mmcr, dmacontrol.exttc7));
printf("val of bcctl is 0x%x\n", val(mmcr, dmacontrol.bcctl));
printf("val of bcsta is 0x%x\n", val(mmcr, dmacontrol.bcsta));
printf("val of bsintenb is 0x%x\n", val(mmcr, dmacontrol.bsintenb));
printf("val of bcval is 0x%x\n", val(mmcr, dmacontrol.bcval));
printf("val of nxtaddl3 is 0x%x\n", val(mmcr, dmacontrol.nxtaddl3));
printf("val of nxtaddh3 is 0x%x\n", val(mmcr, dmacontrol.nxtaddh3));
printf("val of nxtaddl5 is 0x%x\n", val(mmcr, dmacontrol.nxtaddl5));
printf("val of nxtaddh5 is 0x%x\n", val(mmcr, dmacontrol.nxtaddh5));
printf("val of nxtaddl6 is 0x%x\n", val(mmcr, dmacontrol.nxtaddl6));
printf("val of nxtaddh6 is 0x%x\n", val(mmcr, dmacontrol.nxtaddh6));
printf("val of nxtaddl7 is 0x%x\n", val(mmcr, dmacontrol.nxtaddl7));
printf("val of nxtaddh7 is 0x%x\n", val(mmcr, dmacontrol.nxtaddh7));
printf("val of nxtttcl3 is 0x%x\n", val(mmcr, dmacontrol.nxtttcl3));
printf("val of nxtttch3 is 0x%x\n", val(mmcr, dmacontrol.nxtttch3));
printf("val of nxtttcl5 is 0x%x\n", val(mmcr, dmacontrol.nxtttcl5));
printf("val of nxtttch5 is 0x%x\n", val(mmcr, dmacontrol.nxtttch5));
printf("val of nxtttcl6 is 0x%x\n", val(mmcr, dmacontrol.nxtttcl6));
printf("val of nxtttch6 is 0x%x\n", val(mmcr, dmacontrol.nxtttch6));
printf("val of nxtttcl7 is 0x%x\n", val(mmcr, dmacontrol.nxtttcl7));
printf("val of nxtttch7 is 0x%x\n", val(mmcr, dmacontrol.nxtttch7));



	printf("size is 0x%x\n", sizeof(struct mmcr));
}
