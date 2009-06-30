/* by yhlu 6.2005 */
/* be warned, this file will be used core 0/node 0 only */

static void __attribute__((noinline)) clear_init_ram(void)
{
	// gcc 3.4.5 will inline the copy_and_run and clear_init_ram in post_cache_as_ram
	// will reuse %edi as 0 from clear_memory for copy_and_run part, actually it is increased already
	// so noline clear_init_ram

#if CONFIG_HAVE_ACPI_RESUME == 1
	/* clear only coreboot used region of memory. Note: this may break ECC enabled boards */
	clear_memory( CONFIG_RAMBASE,  (CONFIG_LB_MEM_TOPK << 10) -  CONFIG_RAMBASE - CONFIG_DCACHE_RAM_SIZE);
#else
        clear_memory(0,  ((CONFIG_LB_MEM_TOPK<<10) - CONFIG_DCACHE_RAM_SIZE));
#endif
}

/* be warned, this file will be used by core other than core 0/node 0 or core0/node0 when cpu_reset*/
static void set_init_ram_access(void)
{
	set_var_mtrr(0, 0x00000000, CONFIG_LB_MEM_TOPK << 10, MTRR_TYPE_WRBACK);
}

