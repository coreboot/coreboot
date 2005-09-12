#include <stdio.h>
#include "mmcr.h"

#define offsetof(s,m) (size_t)(unsigned long)&(((s *)0)->m)

main(){
printf("offset of revid is 0x%x\n\n", offsetof(struct mmcr, revid));
printf("offset of cpucontrol is 0x%x\n\n", offsetof(struct mmcr, cpucontrol));
printf("offset of hostbridge is 0x%x\n\n", offsetof(struct mmcr, hostbridge));
printf("offset of sysarb is 0x%x\n\n", offsetof(struct mmcr, sysarb));
printf("offset of sysmap is 0x%x\n\n", offsetof(struct mmcr, sysmap));
printf("offset of gpctl is 0x%x\n\n", offsetof(struct mmcr, gpctl));
printf("offset of pio is 0x%x\n\n", offsetof(struct mmcr, pio));
printf("offset of swtmr is 0x%x\n\n", offsetof(struct mmcr, swtmr));
printf("offset of gptimers is 0x%x\n\n", offsetof(struct mmcr, gptimers));
printf("offset of watchdog is 0x%x\n\n", offsetof(struct mmcr, watchdog));
printf("offset of uarts is 0x%x\n\n", offsetof(struct mmcr, uarts));
printf("offset of ssi is 0x%x\n\n", offsetof(struct mmcr, ssi));
printf("offset of pic is 0x%x\n\n", offsetof(struct mmcr, pic));
printf("offset of reset is 0x%x\n\n", offsetof(struct mmcr, reset));
printf("offset of dmacontrol is 0x%x\n\n", offsetof(struct mmcr, dmacontrol));

printf("\n");
printf("offset of memregs is 0x%x\n\n", offsetof(struct mmcr, memregs));
printf("offset of drcctl is 0x%x\n", offsetof(struct mmcr, memregs.drcctl));
printf("offset of drctmctl is 0x%x\n", offsetof(struct mmcr, memregs.drctmctl));
printf("offset of drccfg is 0x%x\n", offsetof(struct mmcr, memregs.drccfg));
printf("offset of bendaddr is 0x%x\n", offsetof(struct mmcr, memregs.drcbendadr));
printf("offset of eccctl is 0x%x\n", offsetof(struct mmcr, memregs.eccctl));
printf("offset of eccsta is 0x%x\n", offsetof(struct mmcr, memregs.eccsta));
printf("offset of ckbpos is 0x%x\n", offsetof(struct mmcr, memregs.eccckbpos));
printf("offset of cktest is 0x%x\n", offsetof(struct mmcr, memregs.ecccktest));
printf("offset of sbadd is 0x%x\n", offsetof(struct mmcr, memregs.eccsbadd));
printf("offset of mbadd is 0x%x\n", offsetof(struct mmcr, memregs.eccmbadd));

printf("\n");
printf("offset of dbctl is 0x%x\n\n", offsetof(struct mmcr, dbctl));
printf("offset of dbctl is 0x%x\n", offsetof(struct mmcr, dbctl.dbctl));

printf("\n");
printf("offset of romregs is 0x%x\n\n", offsetof(struct mmcr, romregs));
printf("offset of bootcs is 0x%x\n", offsetof(struct mmcr, romregs.bootcs));
printf("offset of romcs1 is 0x%x\n", offsetof(struct mmcr, romregs.romcs1));
printf("offset of romcs2 is 0x%x\n", offsetof(struct mmcr, romregs.romcs2));

printf("\n");
printf("offset of hostbridge is 0x%x\n\n", offsetof(struct mmcr, hostbridge));
printf("offset of hbctl is 0x%x\n",  offsetof(struct mmcr, hostbridge.hbctl));
printf("offset of hbtgtirqctl is 0x%x\n",  offsetof(struct mmcr, hostbridge.hbtgtirqctl));
printf("offset of hbtgtirqsta is 0x%x\n",  offsetof(struct mmcr, hostbridge.hbtgtirqsta));
printf("offset of hbmstirqctl is 0x%x\n",  offsetof(struct mmcr, hostbridge.hbmstirqctl));
printf("offset of hbmstirqsta is 0x%x\n",  offsetof(struct mmcr, hostbridge.hbmstirqsta));
printf("offset of mstintadd is 0x%x\n",  offsetof(struct mmcr, hostbridge.mstintadd));


printf("\n");
printf("offset of sysarb is 0x%x\n\n", offsetof(struct mmcr, sysarb));
printf("offset of sysarbctl is 0x%x\n", offsetof(struct mmcr, sysarb.sysarbctl));
printf("offset of pciarbsta is 0x%x\n", offsetof(struct mmcr, sysarb.pciarbsta));
printf("offset of sysarbmenb is 0x%x\n", offsetof(struct mmcr, sysarb.sysarbmenb));
printf("offset of arbprictl is 0x%x\n", offsetof(struct mmcr, sysarb.arbprictl));

printf("\n");
printf("offset of sysmap is 0x%x\n\n", offsetof(struct mmcr, sysmap));
printf("offset of adddecctl is 0x%x\n", offsetof(struct mmcr, sysmap.adddecctl));
printf("offset of wpvsta is 0x%x\n", offsetof(struct mmcr, sysmap.wpvsta));
printf("offset of par is 0x%x\n", offsetof(struct mmcr, sysmap.par));

printf("\n");
printf("offset of gpctl is 0x%x\n\n", offsetof(struct mmcr, gpctl));
printf("offset of gpecho is 0x%x\n", offsetof(struct mmcr, gpctl.gpecho));
printf("offset of gpcsdw is 0x%x\n", offsetof(struct mmcr, gpctl.gpcsdw));
printf("offset of gpcsqual is 0x%x\n", offsetof(struct mmcr, gpctl.gpcsqual));
printf("offset of gpcsrt is 0x%x\n", offsetof(struct mmcr, gpctl.gpcsrt));
printf("offset of gpcspw is 0x%x\n", offsetof(struct mmcr, gpctl.gpcspw));
printf("offset of gpcsoff is 0x%x\n", offsetof(struct mmcr, gpctl.gpcsoff));
printf("offset of gprdw is 0x%x\n", offsetof(struct mmcr, gpctl.gprdw));
printf("offset of gprdoff is 0x%x\n", offsetof(struct mmcr, gpctl.gprdoff));
printf("offset of gpwrw is 0x%x\n", offsetof(struct mmcr, gpctl.gpwrw));
printf("offset of gpwroff is 0x%x\n", offsetof(struct mmcr, gpctl.gpwroff));
printf("offset of gpalew is 0x%x\n", offsetof(struct mmcr, gpctl.gpalew));
printf("offset of gpaleoff is 0x%x\n", offsetof(struct mmcr, gpctl.gpaleoff));

printf("\n");
printf("offset of pio is 0x%x\n\n", offsetof(struct mmcr, pio));

printf("offset of piopfs15_0 is 0x%x\n",  offsetof(struct mmcr, pio.piopfs15_0));
printf("offset of piopfs31_16 is 0x%x\n",  offsetof(struct mmcr, pio.piopfs31_16));
printf("offset of cspfs is 0x%x\n",  offsetof(struct mmcr, pio.cspfs));
printf("offset of clksel is 0x%x\n",  offsetof(struct mmcr, pio.clksel));
printf("offset of dsctl is 0x%x\n",  offsetof(struct mmcr, pio.dsctl));
printf("offset of piodir15_0 is 0x%x\n",  offsetof(struct mmcr, pio.piodir15_0));
printf("offset of piodir31_16 is 0x%x\n",  offsetof(struct mmcr, pio.piodir31_16));
printf("offset of piodata15_0 is 0x%x\n",  offsetof(struct mmcr, pio.piodata15_0));
printf("offset of piodata31_16 is 0x%x\n",  offsetof(struct mmcr, pio.piodata31_16));
printf("offset of pioset15_0 is 0x%x\n",  offsetof(struct mmcr, pio.pioset15_0));
printf("offset of pioset31_16 is 0x%x\n",  offsetof(struct mmcr, pio.pioset31_16));
printf("offset of pioclr15_0 is 0x%x\n",  offsetof(struct mmcr, pio.pioclr15_0));
printf("offset of pioclr31_16 is 0x%x\n",  offsetof(struct mmcr, pio.pioclr31_16));

printf("\noffset of swtmr is 0x%x\n", offsetof(struct mmcr, swtmr));
printf("offset of swtmrmilli is 0x%x\n", offsetof(struct mmcr, swtmr.swtmrmilli));
printf("offset of swtmrmicro is 0x%x\n", offsetof(struct mmcr, swtmr.swtmrmicro));
printf("offset of swtmrcfg is 0x%x\n", offsetof(struct mmcr, swtmr.swtmrcfg));

printf("\noffset of gptimers is 0x%x\n", offsetof(struct mmcr, gptimers));
printf("offset of status  is 0x%x\n", offsetof(struct mmcr, gptimers.status));
printf("offset of pad  is 0x%x\n", offsetof(struct mmcr, gptimers.pad));
printf("offset of timer  is 0x%x\n", offsetof(struct mmcr, gptimers.timer));

printf("offset of timers[0].ctl is 0x%x\n",  offsetof(struct mmcr, gptimers.timer[0].ctl));
printf("offset of timers[0].cnt is 0x%x\n",  offsetof(struct mmcr, gptimers.timer[0].cnt));
printf("offset of timers[0].maxcmpa is 0x%x\n",  offsetof(struct mmcr, gptimers.timer[0].maxcmpa));
printf("offset of timers[0].maxcmpb is 0x%x\n",  offsetof(struct mmcr, gptimers.timer[0].maxcmpb));

printf("offset of timers[1].ctl is 0x%x\n",  offsetof(struct mmcr, gptimers.timer[1].ctl));
printf("offset of timers[1].cnt is 0x%x\n",  offsetof(struct mmcr, gptimers.timer[1].cnt));
printf("offset of timers[1].maxcmpa is 0x%x\n",  offsetof(struct mmcr, gptimers.timer[1].maxcmpa));
printf("offset of timers[1].maxcmpb is 0x%x\n",  offsetof(struct mmcr, gptimers.timer[1].maxcmpb));
printf("offset of timers[2].ctl is 0x%x\n",  offsetof(struct mmcr, gptimers.ctl2));
printf("offset of timers[2].cnt is 0x%x\n",  offsetof(struct mmcr, gptimers.cnt2));
printf("offset of timers[2].maxcmpa is 0x%x\n",  offsetof(struct mmcr, gptimers.maxcmpa2));

printf("\noffset of watchdog is 0x%x\n", offsetof(struct mmcr, watchdog));
printf("offset of ctl  is 0x%x\n",  offsetof(struct mmcr, watchdog.ctl));
printf("offset of cntll  is 0x%x\n",  offsetof(struct mmcr, watchdog.cntll));
printf("offset of cntlh  is 0x%x\n",  offsetof(struct mmcr, watchdog.cntlh));

printf("\noffset of uarts is 0x%x\n", offsetof(struct mmcr, uarts));
printf("offset of uart 1 ctl is 0x%x\n", offsetof(struct mmcr, uarts.uart[0].ctl));
printf("offset of uart 1 sta is 0x%x\n", offsetof(struct mmcr, uarts.uart[0].sta));
printf("offset of uart 1 fcrshad is 0x%x\n", offsetof(struct mmcr, uarts.uart[0].fcrshad));
printf("offset of uart 2 ctl is 0x%x\n", offsetof(struct mmcr, uarts.uart[1].ctl));
printf("offset of uart 2 sta is 0x%x\n", offsetof(struct mmcr, uarts.uart[1].sta));
printf("offset of uart 2 fcrshad is 0x%x\n", offsetof(struct mmcr, uarts.uart[1].fcrshad));

printf("\noffset of ssi is 0x%x\n", offsetof(struct mmcr, ssi));
printf("offset of ssi ctl is 0x%x\n", offsetof(struct mmcr, ssi.ctl));
printf("offset of ssi xmit is 0x%x\n", offsetof(struct mmcr, ssi.xmit));
printf("offset of ssi cmd is 0x%x\n", offsetof(struct mmcr, ssi.cmd));
printf("offset of ssi sta is 0x%x\n", offsetof(struct mmcr, ssi.sta));
printf("offset of ssi rcv is 0x%x\n", offsetof(struct mmcr, ssi.rcv));

printf("\noffset of pic is 0x%x\n", offsetof(struct mmcr, pic));
printf("offset of pcicr is 0x%x\n", offsetof(struct mmcr, pic.pcicr));
printf("offset of mpicmode is 0x%x\n", offsetof(struct mmcr, pic.mpicmode));
printf("offset of sl1picmode is 0x%x\n", offsetof(struct mmcr, pic.sl1picmode));
printf("offset of sl2picmode is 0x%x\n", offsetof(struct mmcr, pic.sl2picmode));
printf("offset of swint16_1 is 0x%x\n", offsetof(struct mmcr, pic.swint16_1));
printf("offset of swint22_17 is 0x%x\n", offsetof(struct mmcr, pic.swint22_17));
printf("offset of intpinpol is 0x%x\n", offsetof(struct mmcr, pic.intpinpol));
printf("offset of pichostmap is 0x%x\n", offsetof(struct mmcr, pic.pichostmap));
printf("offset of eccmap is 0x%x\n", offsetof(struct mmcr, pic.eccmap));
printf("offset of gptmr0map is 0x%x\n", offsetof(struct mmcr, pic.gptmr0map));
printf("offset of gptmr1map is 0x%x\n", offsetof(struct mmcr, pic.gptmr1map));
printf("offset of gptmr2map is 0x%x\n", offsetof(struct mmcr, pic.gptmr2map));
printf("offset of pit0map is 0x%x\n", offsetof(struct mmcr, pic.pit0map));
printf("offset of pit1map is 0x%x\n", offsetof(struct mmcr, pic.pit1map));
printf("offset of pit2map is 0x%x\n", offsetof(struct mmcr, pic.pit2map));
printf("offset of uart1map is 0x%x\n", offsetof(struct mmcr, pic.uart1map));
printf("offset of uart2map is 0x%x\n", offsetof(struct mmcr, pic.uart2map));
printf("offset of pciintamap is 0x%x\n", offsetof(struct mmcr, pic.pciintamap));
printf("offset of pciintbmap is 0x%x\n", offsetof(struct mmcr, pic.pciintbmap));
printf("offset of pciintcmap is 0x%x\n", offsetof(struct mmcr, pic.pciintcmap));
printf("offset of pciintdmap is 0x%x\n", offsetof(struct mmcr, pic.pciintdmap));
printf("offset of dmabcintmap is 0x%x\n", offsetof(struct mmcr, pic.dmabcintmap));
printf("offset of ssimap is 0x%x\n", offsetof(struct mmcr, pic.ssimap));
printf("offset of wdtmap is 0x%x\n", offsetof(struct mmcr, pic.wdtmap));
printf("offset of rtcmap is 0x%x\n", offsetof(struct mmcr, pic.rtcmap));
printf("offset of wpvmap is 0x%x\n", offsetof(struct mmcr, pic.wpvmap));
printf("offset of icemap is 0x%x\n", offsetof(struct mmcr, pic.icemap));
printf("offset of ferrmap is 0x%x\n", offsetof(struct mmcr, pic.ferrmap));
printf("offset of gp0imap is 0x%x\n", offsetof(struct mmcr, pic.gp0imap));
printf("offset of gp1imap is 0x%x\n", offsetof(struct mmcr, pic.gp1imap));
printf("offset of gp2imap is 0x%x\n", offsetof(struct mmcr, pic.gp2imap));
printf("offset of gp3imap is 0x%x\n", offsetof(struct mmcr, pic.gp3imap));
printf("offset of gp4imap is 0x%x\n", offsetof(struct mmcr, pic.gp4imap));
printf("offset of gp5imap is 0x%x\n", offsetof(struct mmcr, pic.gp5imap));
printf("offset of gp6imap is 0x%x\n", offsetof(struct mmcr, pic.gp6imap));
printf("offset of gp7imap is 0x%x\n", offsetof(struct mmcr, pic.gp7imap));
printf("offset of gp8imap is 0x%x\n", offsetof(struct mmcr, pic.gp8imap));
printf("offset of gp9imap is 0x%x\n", offsetof(struct mmcr, pic.gp9imap));
printf("offset of gp10imap is 0x%x\n", offsetof(struct mmcr, pic.gp10imap));

printf("\noffset of reset is 0x%x\n\n", offsetof(struct mmcr, reset));
printf("offset of sysinfo is 0x%x\n", offsetof(struct mmcr, reset.sysinfo));
printf("offset of rescfg is 0x%x\n", offsetof(struct mmcr, reset.rescfg));
printf("offset of ressta is 0x%x\n", offsetof(struct mmcr, reset.ressta));


printf("\noffset of dmacontrol is 0x%x\n\n", offsetof(struct mmcr, dmacontrol));
printf("offset of ctl is 0x%x\n", offsetof(struct mmcr, dmacontrol.ctl));
printf("offset of mmio is 0x%x\n", offsetof(struct mmcr, dmacontrol.mmio));
printf("offset of extchanmapa is 0x%x\n", offsetof(struct mmcr, dmacontrol.extchanmapa));
printf("offset of extchanmapb is 0x%x\n", offsetof(struct mmcr, dmacontrol.extchanmapb));
printf("offset of extpg0 is 0x%x\n", offsetof(struct mmcr, dmacontrol.extpg0));
printf("offset of extpg1 is 0x%x\n", offsetof(struct mmcr, dmacontrol.extpg1));
printf("offset of extpg2 is 0x%x\n", offsetof(struct mmcr, dmacontrol.extpg2));
printf("offset of extpg3 is 0x%x\n", offsetof(struct mmcr, dmacontrol.extpg3));
printf("offset of extpg5 is 0x%x\n", offsetof(struct mmcr, dmacontrol.extpg5));
printf("offset of extpg6 is 0x%x\n", offsetof(struct mmcr, dmacontrol.extpg6));
printf("offset of extpg7 is 0x%x\n", offsetof(struct mmcr, dmacontrol.extpg7));
printf("offset of exttc3 is 0x%x\n", offsetof(struct mmcr, dmacontrol.exttc3));
printf("offset of exttc5 is 0x%x\n", offsetof(struct mmcr, dmacontrol.exttc5));
printf("offset of exttc6 is 0x%x\n", offsetof(struct mmcr, dmacontrol.exttc6));
printf("offset of exttc7 is 0x%x\n", offsetof(struct mmcr, dmacontrol.exttc7));
printf("offset of bcctl is 0x%x\n", offsetof(struct mmcr, dmacontrol.bcctl));
printf("offset of bcsta is 0x%x\n", offsetof(struct mmcr, dmacontrol.bcsta));
printf("offset of bsintenb is 0x%x\n", offsetof(struct mmcr, dmacontrol.bsintenb));
printf("offset of bcval is 0x%x\n", offsetof(struct mmcr, dmacontrol.bcval));
printf("offset of nxtaddl3 is 0x%x\n", offsetof(struct mmcr, dmacontrol.nxtaddl3));
printf("offset of nxtaddh3 is 0x%x\n", offsetof(struct mmcr, dmacontrol.nxtaddh3));
printf("offset of nxtaddl5 is 0x%x\n", offsetof(struct mmcr, dmacontrol.nxtaddl5));
printf("offset of nxtaddh5 is 0x%x\n", offsetof(struct mmcr, dmacontrol.nxtaddh5));
printf("offset of nxtaddl6 is 0x%x\n", offsetof(struct mmcr, dmacontrol.nxtaddl6));
printf("offset of nxtaddh6 is 0x%x\n", offsetof(struct mmcr, dmacontrol.nxtaddh6));
printf("offset of nxtaddl7 is 0x%x\n", offsetof(struct mmcr, dmacontrol.nxtaddl7));
printf("offset of nxtaddh7 is 0x%x\n", offsetof(struct mmcr, dmacontrol.nxtaddh7));
printf("offset of nxtttcl3 is 0x%x\n", offsetof(struct mmcr, dmacontrol.nxtttcl3));
printf("offset of nxtttch3 is 0x%x\n", offsetof(struct mmcr, dmacontrol.nxtttch3));
printf("offset of nxtttcl5 is 0x%x\n", offsetof(struct mmcr, dmacontrol.nxtttcl5));
printf("offset of nxtttch5 is 0x%x\n", offsetof(struct mmcr, dmacontrol.nxtttch5));
printf("offset of nxtttcl6 is 0x%x\n", offsetof(struct mmcr, dmacontrol.nxtttcl6));
printf("offset of nxtttch6 is 0x%x\n", offsetof(struct mmcr, dmacontrol.nxtttch6));
printf("offset of nxtttcl7 is 0x%x\n", offsetof(struct mmcr, dmacontrol.nxtttcl7));
printf("offset of nxtttch7 is 0x%x\n", offsetof(struct mmcr, dmacontrol.nxtttch7));



	printf("size is 0x%x\n", sizeof(struct mmcr));
}
