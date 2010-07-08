/* Convert to C by yhlu */
#define MCH_DRC 0x7c
#define DRC_DONE (1 << 29)

/* If I have already booted once skip a bunch of initialization */
/* To see if I have already booted I check to see if memory
 * has been enabled.
 */
int bios_reset_detected(void)
{
	uint32_t dword;

	dword = pci_read_config32(PCI_DEV(0, 0, 0), MCH_DRC);

	if( (dword & DRC_DONE) != 0 ) {
		return 1;
	}

	return 0;
}
