#ifndef SUBR_H_
#define SUBR_H_

/*
 *	Read/Write the special configuration registers on the INTEL
 */
// TODO: split this thing up. Delete the intel_ names 
// that really are not intel-specific, e.g. intel_conf_readb should
// be conf_readb -- those addresses are defined in the PCI spec. 
unsigned char intel_conf_readb(unsigned long port);
void intel_conf_writeb(unsigned long port, unsigned char value);

void intel_keyboard_on();
void intel_smpblock(void *v);
void intel_nvram_on();

void displayinit(void);
void display(char msg[]);
void printnibl(unsigned long nibl);
void printchar(unsigned char x);
void printint(unsigned long x);
void error(char errmsg[]);
void intel_post(unsigned char value);
void intel_conf_writeb(unsigned long port, unsigned char value);
unsigned char intel_conf_readb(unsigned long port);
void intel_cache_on(unsigned long base, unsigned long totalram);
void intel_interrupts_on(void);
void pc_keyboard_init(void);
void intel_mainboard_fixup(void);
unsigned long sizeram(void);

#ifdef INTEL_PPRO_MTRR
void intel_set_mtrr(unsigned long rambase, unsigned long ramsizeK);
#endif

#include <pci.h>
#include <pci-i386.h>

#define PIRQ_SIGNATURE	(('$' << 0) + ('P' << 8) + ('I' << 16) + ('R' << 24))
#define PIRQ_VERSION 0x0100

extern const struct irq_routing_table intel_irq_routing_table;

void intel_zero_irq_settings(void);
void intel_check_irq_routing_table(void);

#endif /* SUBR_H_ */
