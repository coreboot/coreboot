//kind of cmos_err for ich5
#define RTC_FAILED    (1 <<2)
#define GEN_PMCON_3     0xa4
static void check_cmos_failed(void) 
{

                uint8_t byte;
                byte = pci_read_config8(PCI_DEV(0,0x1f,0),GEN_PMCON_3);
                if( byte & RTC_FAILED){
//clear bit 1 and bit 2
                        byte = cmos_read(RTC_BOOT_BYTE);
                        byte &= 0x0c;
                        byte |= CONFIG_MAX_REBOOT_CNT << 4;
                        cmos_write(byte, RTC_BOOT_BYTE);
                }
}
