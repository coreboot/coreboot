/* checkpir.c : This software is released under GPL
  For Linuxbios use only
  Aug 26 2001 , Nikolai Vladychevski, <niko@isl.net.mx>
*/

#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "pci.h"
#include "pirq_routing.h"
#include "types.h"
#include "kmalloc.h"


#define PIRQ_SIGNATURE  (('$' << 0) + ('P' << 8) + ('I' << 16) + ('R' << 24))
#define PIRQ_VERS 0x0100

//extern struct irq_routing_table intel_irq_routing_table;

struct irq_info se_arr[50];
struct irq_routing_table *rt;

int calc_checksum(struct irq_routing_table *rt) {
long sum=0,i;
u8 *addr,sum2=0;
    
    addr= (u8 *) rt;
    for (i=0;i<rt->size;i++) sum2 += addr[i];
    return(sum2);
}	

main () {
u8 sum,newsum;
    rt=(struct irq_routing_table*) &intel_irq_routing_table;
    sum=calc_checksum(rt);
    printf("Validating checksum, file: irq_tables.c that was in ./ at compile time...\n");
    printf("(no other tests are done)\n");
    if (!sum) { printf("Checksum for IRQ Routing table is ok. You can use it in LinuxBios now\n"); }
    else {
	newsum=rt->checksum-sum;
    	printf("BAD CHECKSUM for IRQ Routing table !!!!\n");
	printf("If you want to make it valid, change the checksum to: %#x\n",newsum);
    }
}
