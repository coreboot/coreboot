#include <pci.h>
#include <pci_ids.h>
#include <arch/io.h>
#include <southbridge/amd/amd766.h>


void amd766_hard_reset(void)
{
	pci_set_method();
	/* Allow the watchdog timer to reboot us, and enable 0xcf9 */
	pcibios_write_config_byte(0, (AMD766_DEV >> 8) | 3, 0x41, (1<<5)|(1<<1));
	/* Try rebooting though port 0xcf9 */
	outb((0<<3)|(1<<2)|(1<<1), 0xcf9);
	return;
}
