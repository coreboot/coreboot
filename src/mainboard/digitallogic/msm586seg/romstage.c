#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/hlt.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include "cpu/x86/bist.h"

void setup_pars(void)
{
	volatile unsigned long *par;
 	/* as per the book: */
  	/* PAR register setup */
        /* set up the PAR registers as they are on the MSM586SEG */
        par = (unsigned long *) 0xfffef088;

        /* NOTE: move this to mainboard.c ASAP */
        *par++ = 0x607c00a0; /*PAR0: PCI:Base 0xa0000; size 0x1f000:*/
        *par++ = 0x480400d8; /*PAR1: GP BUS MEM:CS2:Base 0xd8, size 0x4:*/
        *par++ = 0x340100ea; /*PAR2: GP BUS IO:CS5:Base 0xea, size 0x1:*/
        *par++ = 0x380701f0; /*PAR3: GP BUS IO:CS6:Base 0x1f0, size 0x7:*/
        *par++ = 0x3c0003f6; /*PAR4: GP BUS IO:CS7:Base 0x3f6, size 0x0:*/
        *par++ = 0x35ff0400; /*PAR5: GP BUS IO:CS5:Base 0x400, size 0xff:*/
        *par++ = 0x35ff0600; /*PAR6: GP BUS IO:CS5:Base 0x600, size 0xff:*/
        *par++ = 0x35ff0800; /*PAR7: GP BUS IO:CS5:Base 0x800, size 0xff:*/
        *par++ = 0x35ff0a00; /*PAR8: GP BUS IO:CS5:Base 0xa00, size 0xff:*/
        *par++ = 0x35ff0e00; /*PAR9: GP BUS IO:CS5:Base 0xe00, size 0xff:*/
        *par++ = 0x34fb0104; /*PAR10: GP BUS IO:CS5:Base 0x104, size 0xfb:*/
        *par++ = 0x35af0200; /*PAR11: GP BUS IO:CS5:Base 0x200, size 0xaf:*/
        *par++ = 0x341f03e0; /*PAR12: GP BUS IO:CS5:Base 0x3e0, size 0x1f:*/
        *par++ = 0xe41c00c0; /*PAR13: SDRAM:code:cache:nowrite:Base 0xc0000, size 0x7000:*/
        *par++ = 0x545c00c8; /*PAR14: GP BUS MEM:CS5:Base 0xc8, size 0x5c:*/
        *par++ = 0x8a020200; /*PAR15: BOOTCS:code:nocache:write:Base 0x2000000, size 0x80000:*/
}

#include "cpu/amd/sc520/raminit.c"

struct mem_controller {
	int i;
};

static int spd_read_byte(unsigned device, unsigned address) { }

static inline void dumpmem(void){
  int i, j;
  unsigned char *l;
  unsigned char c;

  for(i = 0x4000; i < 0x5000; i += 16) {
    print_err_hex32(i); print_err(":");
    for(j = 0; j < 16; j++) {
      l = (unsigned char *)i + j;
      c = *l;
      print_err_hex8(c);
      print_err(" ");
    }
    print_err("\n");
  }
}

static inline void irqinit(void){
	volatile unsigned char *cp;
#if 0
/* these values taken from the msm board itself.
 * and they cause the board to not even come out of calibrating_delay_loop
 * if you can believe it. Our problem right now is no IDE or serial interrupts
 * So we'll try to put interrupts in, one at a time. IDE first.
 */
	cp = (volatile unsigned char *) 0xfffefd00;
	*cp =  0x11;
	cp = (volatile unsigned char *) 0xfffefd02;
	*cp =  0x02;
	cp = (volatile unsigned char *) 0xfffefd03;
	*cp =  0x00;
	cp = (volatile unsigned char *) 0xfffefd04;
	*cp =  0xf7;
	cp = (volatile unsigned char *) 0xfffefd08;
	*cp =  0xf7;
	cp = (volatile unsigned char *) 0xfffefd0a;
	*cp =  0x8b;
	cp = (volatile unsigned char *) 0xfffefd10;
	*cp =  0x18;
	cp = (volatile unsigned char *) 0xfffefd14;
	*cp =  0x09;
	cp = (volatile unsigned char *) 0xfffefd18;
	*cp =  0x88;
	cp = (volatile unsigned char *) 0xfffefd1a;
	*cp =  0x00;
	cp = (volatile unsigned char *) 0xfffefd1b;
	*cp =  0x00;
	cp = (volatile unsigned char *) 0xfffefd1c;
	*cp =  0x00;
	cp = (volatile unsigned char *) 0xfffefd20;
	*cp =  0x00;
	cp = (volatile unsigned char *) 0xfffefd21;
	*cp =  0x00;
	cp = (volatile unsigned char *) 0xfffefd22;
	*cp =  0x00;
	cp = (volatile unsigned char *) 0xfffefd28;
	*cp =  0x00;
	cp = (volatile unsigned char *) 0xfffefd29;
	*cp =  0x00;
	cp = (volatile unsigned char *) 0xfffefd30;
	*cp =  0x00;
	cp = (volatile unsigned char *) 0xfffefd31;
	*cp =  0x00;
	cp = (volatile unsigned char *) 0xfffefd32;
	*cp =  0x00;
	cp = (volatile unsigned char *) 0xfffefd33;
	*cp =  0x00;
	cp = (volatile unsigned char *) 0xfffefd40;
	*cp =  0x10;
	cp = (volatile unsigned char *) 0xfffefd41;
	*cp =  0x00;
	cp = (volatile unsigned char *) 0xfffefd42;
	*cp =  0x00;
	cp = (volatile unsigned char *) 0xfffefd43;
	*cp =  0x00;
	cp = (volatile unsigned char *) 0xfffefd44;
	*cp =  0x00;
	cp = (volatile unsigned char *) 0xfffefd45;
	*cp =  0x00;
	cp = (volatile unsigned char *) 0xfffefd46;
	*cp =  0x00;
	cp = (volatile unsigned char *) 0xfffefd50;
	*cp =  0x37;
	cp = (volatile unsigned char *) 0xfffefd51;
	*cp =  0x00;
	cp = (volatile unsigned char *) 0xfffefd52;
	*cp =  0x00;
	cp = (volatile unsigned char *) 0xfffefd53;
	*cp =  0x00;
	cp = (volatile unsigned char *) 0xfffefd54;
	*cp =  0x37;
	cp = (volatile unsigned char *) 0xfffefd55;
	*cp =  0x00;
	cp = (volatile unsigned char *) 0xfffefd56;
	*cp =  0x37;
	cp = (volatile unsigned char *) 0xfffefd57;
	*cp =  0x00;
	cp = (volatile unsigned char *) 0xfffefd58;
	*cp =  0xff;
	cp = (volatile unsigned char *) 0xfffefd59;
	*cp =  0xff;
	cp = (volatile unsigned char *) 0xfffefd5a;
	*cp =  0xff;
#endif
#if 0
	/* this fails too */
	/* IDE only ... */
	cp = (volatile unsigned char *) 0xfffefd56;
	*cp =  0xe;
#endif
}

static void main(unsigned long bist)
{
    volatile int i;
    for(i = 0; i < 100; i++)
      ;

        setupsc520();
	irqinit();
        console_init();
		for(i = 0; i < 100; i++)
	  print_err("fill usart\n");
		//		while(1)
		print_err("HI THERE!\n");
		//			sizemem();
	staticmem();
	print_err("c60 is "); print_err_hex16(*(unsigned short *)0xfffefc60);
	print_err("\n");

	//			while(1)
	print_err("STATIC MEM DONE\n");
	post_code(0xee);
	print_err("loop forever ...\n");

#if 0

	/* clear memory 1meg */
        __asm__ volatile(
			 "1: \n\t"
			 "movl %0, %%fs:(%1)\n\t"
			 "addl $4,%1\n\t"
			 "subl $4,%2\n\t"
			 "jnz 1b\n\t"
			 :
			 : "a" (0), "D" (0), "c" (1024*1024)
			 );


#endif

#if 0
	dump_pci_devices();
#endif
#if 0
	dump_pci_device(PCI_DEV(0, 0, 0));
#endif

#if 1
	{
	  volatile unsigned char *src = (unsigned char *) 0x2000000 + 0x60000;
	  volatile unsigned char *dst = (unsigned char *) 0x4000;
	  for(i = 0; i < 0x20000; i++) {
	    /*
	      print_err("Set dst "); print_err_hex32((unsigned long) dst);
	      print_err(" to "); print_err_hex32(*src); print_err("\n");
	    */
	    *dst = *src;
	    //print_err(" dst is now "); print_err_hex32(*dst); print_err("\n");
	    dst++, src++;
	    post_code(i & 0xff);
	  }
	}
	dumpmem();
	post_code(0x00);

	print_err("loop forever\n");
	post_code(0xdd);
        __asm__ volatile(
			 "movl %0, %%edi\n\t"
			 "jmp *%%edi\n\t"
			 :
			 : "a" (0x4000)
			 );

	print_err("Oh dear, I'm afraid it didn't work...\n");

	while(1);
#endif
}
