/* handy dandy stuff for the sc520 MMCR */
/* default location of the MMCR */
#define MMCR 0xfffef000

/* the PAR register struct definition, the location in memory,
  * and a handy pointer for you to use
  */

struct parreg {
	unsigned long reg[16];
};

#define PARREG (struct parreg *)0xfffef088
//static volatile struct parreg *par = PARREG;


#define MMCRPIC (struct mmcrpic *) 0xfffefd00
//static volatile struct mmcrpic *pic = MMCRPIC;

#define M_GINT_MODE     1
#define M_S1_MODE       2
#define M_S2_MODE       4


/* here is the real mmcr struct */

struct memregs {
	/* make these shorts, we are lsb and the hardware seems to like it
	 * better
	 */
	unsigned short drcctl;
	unsigned short drctmctl;
	unsigned short drccfg;
	unsigned char pad1[2];
	unsigned char drcbendadr[4];
	unsigned char pad2[4];
	unsigned char eccctl;
	unsigned char eccsta;
	unsigned char eccckbpos;
	unsigned char ecccktest;
	unsigned long eccsbadd;
	unsigned long eccmbadd;
	unsigned char pad3[18];
};
struct dbctl {
	unsigned char dbctl;
	unsigned char pad4[15];
};

struct romregs {
	unsigned char bootcs;
	unsigned char pad5[3];
	unsigned char romcs1;
	unsigned char pad6[1];
	unsigned char romcs2;
	unsigned char pad7[6];
};


struct hostbridge {
	unsigned short ctl;
	unsigned short tgtirqctl;
	unsigned short tgtirqsta;
	unsigned short mstirqctl;
	unsigned short mstirqsta;
	unsigned char pad8[2];
	unsigned long mstintadd;
};

struct sysarb {
	unsigned char ctl;
	unsigned char sta;
	unsigned char menb;
	unsigned long prictl;
	unsigned char pad9[8];
};

struct sysmap {
	unsigned char adddecctl;
	unsigned char pada[1];
	unsigned short wpvsta;
	unsigned char padb[4];
	unsigned long par[16];
	unsigned char padc[0xb38];
};

struct gpctl {
	unsigned char gpecho;
	unsigned char gpcsdw;
	unsigned char gpcsqual;
	unsigned char padd[5];
	unsigned char gpcsrt;
	unsigned char gpcspw;
	unsigned char gpcsoff;
	unsigned char gprdw;
	unsigned char gprdoff;
	unsigned char gpwrw;
	unsigned char gpwroff;
	unsigned char gpalew;
	unsigned char gpaleoff;
	unsigned char pade[15];
};

struct pio {
	unsigned short pfs15_0;
	unsigned short pfs31_16;
	unsigned char cspfs;
	unsigned char padf[1];
	unsigned char clksel;
	unsigned char padg[1];
	unsigned short dsctl;
	unsigned short dir15_0;
	unsigned short dir31_16;
	unsigned short padh;
	unsigned short data15_0;
	unsigned short data31_16;
	unsigned short set15_0;
	unsigned short set31_16;
	unsigned short clr15_0;
	unsigned short clr31_16;
	unsigned char padi[0x24];
};

struct swtmr {
	unsigned short swtmrmilli;
	unsigned short swtmrmicro;
	unsigned char swtmrcfg;
	unsigned char padj[0xb];
};

struct gptmr {
	unsigned short ctl;
	unsigned short cnt;
	unsigned short maxcmpa, maxcmpb;
};

struct gptimers {
	unsigned char status;
	unsigned char pad;
	struct gptmr timer[2];
	/* yes, they REALLY DID make timer 2 different. */
	/* yikes */
	unsigned short ctl2;
	unsigned short cnt2;
	unsigned char paddumb[8];
	unsigned short maxcmpa2;

	unsigned char pada[0x20];
};

struct watchdog {
	unsigned short ctl;
	unsigned short cntll;
	unsigned short cntlh;
	unsigned char pad[10];
};

struct uart {
	unsigned char ctl, sta, fcrshad, pad;
};

struct uarts {
	struct uart uart[2];
	unsigned char pad[8];
};

struct ssi {
	unsigned char ctl, xmit, cmd, sta, rcv;
	unsigned char pad[0x2b];
};


/* interrupt control registers */
/* defined this way for portability. Shame we can't just use plan 9 c. */
struct pic {
	unsigned char pcicr;
	unsigned char pad1;
	unsigned char mpicmode;
	unsigned char sl1picmode;
	unsigned char sl2picmode;
	unsigned char pad2[3];
	unsigned char swint16_1;
	unsigned char pad3;
	unsigned char swint22_17;
	unsigned char pad4[5];
	unsigned short intpinpol;
	unsigned char pad5[2];
	unsigned char pichostmap;
	unsigned char pad6[3];
	unsigned char eccmap;
	unsigned char pad7[1];
	unsigned char gptmr0map;
	unsigned char gptmr1map;
	unsigned char gptmr2map;
	unsigned char pad8[3];
	unsigned char pit0map;
	unsigned char pit1map;
	unsigned char pit2map;
	unsigned char pad9[5];
	unsigned char uart1map;
	unsigned char uart2map;
	unsigned char pad99[6];
	unsigned char pciintamap;
	unsigned char pciintbmap;
	unsigned char pciintcmap;
	unsigned char pciintdmap;
	unsigned char pad10[12];
	unsigned char dmabcintmap;
	unsigned char ssimap;
	unsigned char wdtmap;
	unsigned char rtcmap;
	unsigned char wpvmap;
	unsigned char icemap;
	unsigned char ferrmap;
	unsigned char pad11[9];
	unsigned char gp0imap;
	unsigned char gp1imap;
	unsigned char gp2imap;
	unsigned char gp3imap;
	unsigned char gp4imap;
	unsigned char gp5imap;
	unsigned char gp6imap;
	unsigned char gp7imap;
	unsigned char gp8imap;
	unsigned char gp9imap;
	unsigned char gp10imap;
	unsigned char padend[0x14];
};

struct reset {
	unsigned char sysinfo;
	unsigned char pad1;
	unsigned char rescfg;
	unsigned char pad2;
	unsigned char ressta;
	unsigned char pad3[0xb];
};

struct dmacontrol {
	unsigned char ctl;
	unsigned char mmio;
	unsigned short extchanmapa;
	unsigned short extchanmapb;
	unsigned char extpg0;
	unsigned char extpg1;
	unsigned char extpg2;
	unsigned char extpg3;
	unsigned char extpg5;
	unsigned char extpg6;
	unsigned char extpg7;
	unsigned char pad[3];
	unsigned char exttc3;
	unsigned char exttc5;
	unsigned char exttc6;
	unsigned char exttc7;
	unsigned char pad1[4];
	unsigned char bcctl;
	unsigned char bcsta;
	unsigned char bsintenb;
	unsigned char bcval;
	unsigned char pad2[4];
	unsigned short nxtaddl3;
	unsigned short nxtaddh3;
	unsigned short nxtaddl5;
	unsigned short nxtaddh5;
	unsigned short nxtaddl6;
	unsigned short nxtaddh6;
	unsigned short nxtaddl7;
	unsigned short nxtaddh7;
	unsigned short nxtttcl3;
	unsigned char nxtttch3;
	unsigned char pad3;
	unsigned short nxtttcl5;
	unsigned char nxtttch5;
	unsigned char pad5;
	unsigned short nxtttcl6;
	unsigned char nxtttch6;
	unsigned char pad6;
	unsigned short nxtttcl7;
	unsigned char nxtttch7;
	unsigned char pad7;
};




struct mmcr {
	unsigned short revid;
	unsigned char cpucontrol;
	unsigned char pad1[11];
	struct memregs memregs;
	struct dbctl dbctl;
	struct romregs romregs;
	struct hostbridge hostbridge;
	struct sysarb sysarb;
	struct sysmap sysmap;
	struct gpctl gpctl;
	struct pio pio;
	struct swtmr swtmr;
	struct gptimers gptimers;
	struct watchdog watchdog;
	struct uarts uarts;
	struct ssi ssi;
	struct pic pic;
	struct reset reset;
	struct dmacontrol dmacontrol;
	unsigned char padend[0x23d];
};


#define MMCRDEFAULT (struct mmcr *) 0xfffef000
