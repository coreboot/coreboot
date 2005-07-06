#define ASSEMBLY 1
#define ASM_CONSOLE_LOGLEVEL 8
#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <arch/hlt.h>
#include "pc80/mc146818rtc_early.c"
#include "pc80/serial.c"
#include "arch/i386/lib/console.c"
#include "ram/ramtest.c"
#include "cpu/x86/bist.h"
//#include "lib/delay.c"
#include "cpu/amd/sc520/raminit.c"

typedef void (*lj)(void);



struct mem_controller {
	int i;
};

static void hard_reset(void)
{
}

static void memreset_setup(void)
{
}

static void memreset(int controllers, const struct mem_controller *ctrl)
{
}



static inline void activate_spd_rom(const struct mem_controller *ctrl)
{
        /* nothing to do */
}
 
static inline int spd_read_byte(unsigned device, unsigned address)
{
//	return smbus_read_byte(device, address);
}

//#include "sdram/generic_sdram.c"

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
    print_err("\r\n");
  }
}

static void main(unsigned long bist)
{
    volatile int i;
    for(i = 0; i < 100; i++)
      ;


        setupsc520();
        uart_init();
        console_init();
		for(i = 0; i < 100; i++)
	  print_err("fill usart\r\n");
		//		while(1)
		print_err("HI THERE!\r\n");
		//			sizemem();
	staticmem();
	print_err("c60 is "); print_err_hex16(*(unsigned short *)0xfffefc60); 
	print_err("\n");
			
	//			while(1)
	print_err("STATIC MEM DONE\r\n");
	outb(0xee, 0x80);
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
	
#if 0
	print_err("RAM CHECK!\r\n");
	// Check 16MB of memory @ 0
	ram_check(0x00000000, 0x01000000);
#endif
#if 0
	print_err("RAM CHECK for 32 MB!\r\n");
	// Check 32MB of memory @ 0
	ram_check(0x00000000, 0x02000000);
#endif
#if 0
	{
	  volatile unsigned char *src = (unsigned char *) 0x2000000 + 0x70000;
	  volatile unsigned char *dst = (unsigned char *) 0x4000; 
	  for(i = 0; i < 0x10000; i++) {
	    /*
	      print_err("Set dst "); print_err_hex32((unsigned long) dst); 
	      print_err(" to "); print_err_hex32(*src); print_err("\r\n");
	    */
	    *dst = *src;
	    //print_err(" dst is now "); print_err_hex32(*dst); print_err("\r\n");
	    dst++, src++;
	    outb((unsigned char)i, 0x80);
	  }
	}
	dumpmem();
	outb(0, 0x80);
	print_err("loop forever\r\n");
	outb(0xdd, 0x80);
        __asm__ volatile(
			 "movl %0, %%edi\n\t"
			 "jmp *%%edi\n\t"
			 :
			 : "a" (0x4000)
			 ); 
	
	print_err("FUCK\r\n");
	
	while(1);
#endif
}

