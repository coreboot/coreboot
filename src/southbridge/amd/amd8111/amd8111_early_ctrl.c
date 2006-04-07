/* by yhlu 2005.10 */
static unsigned get_sbdn(unsigned bus)
{
        device_t dev;

        /* Find the device.
         * There can only be one 8111 on a hypertransport chain/bus.
         */
        dev = pci_locate_device_on_bus(
                PCI_ID(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_8111_PCI),
                bus);

        return (dev>>11) & 0x1f;

}

static void hard_reset(void)
{
        device_t dev;
        unsigned bus;

        /* Find the device.
         * There can only be one 8111 on a hypertransport chain/bus.
         */
        bus = get_sbbusn(get_sblk());
        dev = pci_locate_device_on_bus(
                PCI_ID(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_8111_ACPI),
                bus);

        set_bios_reset();

        /* enable cf9 */
        pci_write_config8(dev, 0x41, 0xf1);
        /* reset */
        outb(0x0e, 0x0cf9);
}

static void enable_fid_change_on_sb(unsigned sbbusn, unsigned sbdn)
{
        device_t dev;

	dev = PCI_DEV(sbbusn, sbdn+1, 3); // ACPI

        pci_write_config8(dev, 0x74, 4);

        /* set VFSMAF ( VID/FID System Management Action Field) to 2 */
        pci_write_config32(dev, 0x70, 2<<12);

}

static void soft_reset_x(unsigned sbbusn, unsigned sbdn)
{
        device_t dev;

	dev = PCI_DEV(sbbusn, sbdn+1, 0); //ISA

        /* Reset */
        set_bios_reset();
        pci_write_config8(dev, 0x47, 1);

}

static void soft_reset(void)
{

	unsigned sblk = get_sblk();
	unsigned sbbusn = get_sbbusn(sblk);
	unsigned sbdn = get_sbdn(sbbusn);

	return soft_reset_x(sbbusn, sbdn);

}
