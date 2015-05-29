/* These helpers are for performing SMM relocation. */
void southbridge_smm_init(void);
void southbridge_trigger_smi(void);
void southbridge_clear_smi_status(void);
void northbridge_get_tseg_base_and_size(u32 *tsegmb, u32 *tseg_size);
int cpu_get_apic_id_map(int *apic_id_map);
void northbridge_write_smram(u8 smram);
