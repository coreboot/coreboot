/* by yhlu 6.2005 
	moved from nrv2v.c and some lines from crt0.S
   2006/05/02 - stepan: move nrv2b to an extra file.
*/

void cbfs_and_run_core(const char*, unsigned ebp);

static void copy_and_run(void)
{
	cbfs_and_run_core(CONFIG_CBFS_PREFIX "/coreboot_ram", 0);
}

#if CONFIG_AP_CODE_IN_CAR == 1

static void copy_and_run_ap_code_in_car(unsigned ret_addr)
{
	cbfs_and_run_core(CONFIG_CBFS_PREFIX "/coreboot_apc", ret_addr);
}
#endif
