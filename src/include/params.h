/* ../lib/params.c */
void init_params(unsigned char *empty_zero_page);
void set_memory_size(unsigned char *empty_zero_page,
		     unsigned long ext_memory_size,
		     unsigned long alt_memory_size);
void set_root_rdonly(unsigned char *empty_zero_page);
void set_command_line(unsigned char *empty_zero_page,
		      unsigned char *cmdline);
unsigned char *get_empty_zero_page(void);
void set_display(unsigned char *empty_zero_page, int rows, int cols);

void set_initrd(unsigned char *empty_zero_page,
		unsigned long start,
		unsigned long size);
