#ifndef SDRAM_H
#define SDRAM_H 

void sdram_no_memory(void);
void sdram_initialize(void);
void sdram_set_registers(void);
void sdram_set_spd_registers(void);
void sdram_enable(void);
void sdram_first_normal_reference(void);
void sdram_enable_refresh(void);
void sdram_special_finishup(void);

void sdram_set_command_noop(void);
void sdram_set_command_precharge(void);
void sdram_set_command_cbr(void);
void sdram_set_command_none(void);
void sdram_assert_command(void);
void sdram_set_mode_register(void);

void sdram_initialize_ecc(void);
unsigned long sdram_get_ecc_size_bytes(void);

#endif /* SDRAM_H */
