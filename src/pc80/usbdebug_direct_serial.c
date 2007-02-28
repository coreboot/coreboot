#include <part/fallback_boot.h>
#include "../lib/usbdebug_direct.c" 
static void early_usbdebug_direct_init(void)
{
	struct ehci_debug_info *dbg_info = 
		(struct ehci_debug_info *)(DCACHE_RAM_BASE + DCACHE_RAM_SIZE - sizeof (struct ehci_debug_info)); 
	
	usbdebug_direct_init(EHCI_BAR, EHCI_DEBUG_OFFSET, dbg_info); 
}
void usbdebug_direct_tx_byte(unsigned char data)
{
	struct ehci_debug_info *dbg_info;
	dbg_info = (struct ehci_debug_info *)(DCACHE_RAM_BASE + DCACHE_RAM_SIZE - sizeof (struct ehci_debug_info)); // in Cache 
	if (dbg_info->ehci_debug) { 
		dbgp_bulk_write_x(dbg_info, &data, 1);
	}
}
void usbdebug_direct_ram_tx_byte(unsigned char data)
{
	struct ehci_debug_info *dbg_info;
	dbg_info = (struct ehci_debug_info *)((CONFIG_LB_MEM_TOPK<<10) - sizeof (struct ehci_debug_info)); //in RAM 
	if (dbg_info->ehci_debug) {
		dbgp_bulk_write_x(dbg_info, &data, 1);
	}
}
