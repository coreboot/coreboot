/* by yhlu 2005.10 */
static void hard_reset(struct sys_info *sysinfo)
{
        device_t dev;
	
        /* Find the device */
        dev = PCI_DEV(sysinfo->sbbusn, sysinfo->sbdn+1, 3);

        set_bios_reset();

        /* enable cf9 */
        pci_write_config8(dev, 0x41, 0xf1);
        /* reset */
        outb(0x0e, 0x0cf9);
}

static void enable_fid_change_on_sb(struct sys_info *sysinfo)
{
        device_t dev;
        /* Find the device */
        dev = PCI_DEV(sysinfo->sbbusn, sysinfo->sbdn+1, 3);

        pci_write_config8(dev, 0x74, 4);

        /* set VFSMAF ( VID/FID System Management Action Field) to 2 */
        pci_write_config32(dev, 0x70, 2<<12);

}

static void soft_reset(struct sys_info *sysinfo)
{
        device_t dev;
        
	/* Find the device */
        dev = PCI_DEV(sysinfo->sbbusn, sysinfo->sbdn+1, 0);

        set_bios_reset();
        pci_write_config8(dev, 0x47, 1);
}


