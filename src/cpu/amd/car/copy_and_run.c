/* by yhlu 6.2005 
	moved from nrv2v.c and some lines from crt0.S
   2006/05/02 - stepan: move nrv2b to an extra file.
*/

#if CONFIG_CBFS == 1
void cbfs_and_run_core(char*, unsigned ebp);

static void copy_and_run(void)
{
# if CONFIG_USE_FALLBACK_IMAGE == 1
	cbfs_and_run_core("fallback/coreboot_ram", 0);
# else
      	cbfs_and_run_core("normal/coreboot_ram", 0);
# endif
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

#else
void copy_and_run_core(u8 *src, u8 *dst, unsigned long ilen, unsigned ebp);

extern u8 _liseg, _iseg, _eiseg;

static void copy_and_run(void)
{
	uint8_t *src, *dst; 
        unsigned long ilen;

	src = &_liseg;
	dst = &_iseg;
	ilen = &_eiseg - dst;

	copy_and_run_core(src, dst, ilen, 0);
}

#if CONFIG_AP_CODE_IN_CAR == 1

extern u8 _liseg_apc, _iseg_apc, _eiseg_apc;

static void copy_and_run_ap_code_in_car(unsigned ret_addr)
{
        uint8_t *src, *dst;
        unsigned long ilen;

	src = &_liseg_apc;
	dst = &_iseg_apc;
	ilen = &_eiseg_apc - dst;

	copy_and_run_core(src, dst, ilen, ret_addr);
}
#endif
#endif
