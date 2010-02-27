//kind of cmos_err for ich3

#include "i82801cx.h"

static void check_cmos_failed(void) 
{
#if CONFIG_HAVE_OPTION_TABLE
	uint8_t byte = pci_read_config8(PCI_DEV(0,0x1f,0),GEN_PMCON_3);

	if( byte & RTC_BATTERY_DEAD) {
		// Set boot_option and last_boot to 'Fallback',
		// clear reboot_bits
        byte = cmos_read(RTC_BOOT_BYTE);
        byte &= 0x0c;
        byte |= CONFIG_MAX_REBOOT_CNT << 4;
        cmos_write(byte, RTC_BOOT_BYTE);
    }
#endif
}
