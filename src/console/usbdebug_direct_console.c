#include <console/console.h>
#include <usbdebug_direct.h>
#include <pc80/mc146818rtc.h>

struct ehci_debug_info dbg_info;

void set_ehci_base(unsigned ehci_base)
{
	unsigned diff;
	if(!dbg_info.ehci_debug) return;
	diff = dbg_info.ehci_caps - ehci_base;
	dbg_info.ehci_regs -= diff;
	dbg_info.ehci_debug -= diff;
	dbg_info.ehci_caps = ehci_base;
}
void set_ehci_debug(unsigned ehci_debug)
{
	dbg_info.ehci_debug = ehci_debug;
}

unsigned get_ehci_debug(void)
{
       return dbg_info.ehci_debug;
}

static void dbgp_init(void)
{
	struct ehci_debug_info *dbg_infox;
	dbg_infox = (struct ehci_debug_info *)((CONFIG_LB_MEM_TOPK<<10) - sizeof (struct ehci_debug_info)); //in RAM
	memcpy(&dbg_info, dbg_infox, sizeof(struct ehci_debug_info) );
}

static void dbgp_tx_byte(unsigned char data) 
{
	if(dbg_info.ehci_debug) 
		dbgp_bulk_write_x(&dbg_info,&data,1);
}

static unsigned char dbgp_rx_byte(void) 
{
	unsigned char data = 0xff;
	if(dbg_info.ehci_debug) 
		dbgp_bulk_read_x(&dbg_info,&data,1);
	return data;
}

static int dbgp_tst_byte(void) 
{
	return dbg_info.ehci_debug;
}

static struct console_driver usbdebug_direct_console __console = {
	.init    = dbgp_init,
	.tx_byte = dbgp_tx_byte,
	.rx_byte = dbgp_rx_byte,
	.tst_byte = dbgp_tst_byte,
};

