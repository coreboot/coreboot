#include <printk.h>
#include <pci.h>
#include <pci_ids.h>
#include <cpu/p5/io.h>

#include <types.h>

void
mainboard_fixup()
{

}

void
final_southbridge_fixup()
{

}

void
final_mainboard_fixup()
{
	final_southbridge_fixup();

	printk_info("Final mainboard fixup\n");
}

