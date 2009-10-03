/* by yhlu 6.2005 
	moved from nrv2v.c and some lines from crt0.S
   2006/05/02 - stepan: move nrv2b to an extra file.
*/

void cbfs_and_run_core(const char*, unsigned ebp);

static void copy_and_run(void)
{
#if CONFIG_USE_FALLBACK_IMAGE == 1
	cbfs_and_run_core("fallback/coreboot_ram", 0);
#else
      	cbfs_and_run_core("normal/coreboot_ram", 0);
#endif
}

#if CONFIG_AP_CODE_IN_CAR == 1

static void copy_and_run_ap_code_in_car(unsigned ret_addr)
{
# if CONFIG_USE_FALLBACK_IMAGE == 1
	cbfs_and_run_core("fallback/coreboot_apc", ret_addr);
# else
      	cbfs_and_run_core("normal/coreboot_apc", ret_addr);
# endif
}
#endif
