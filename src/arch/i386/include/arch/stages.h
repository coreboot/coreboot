void cbfs_and_run_core(const char *filename, unsigned int ebp);
void __attribute__((regparm(0))) copy_and_run(unsigned cpu_reset);
void __attribute__((regparm(0))) copy_and_run_ap_code_in_car(unsigned ret_addr);


