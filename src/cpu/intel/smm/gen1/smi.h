/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* These helpers are for performing SMM relocation. */
void southbridge_smm_init(void);
void southbridge_trigger_smi(void);
void southbridge_clear_smi_status(void);
u32 northbridge_get_tseg_base(void);
u32 northbridge_get_tseg_size(void);
int cpu_get_apic_id_map(int *apic_id_map);
void northbridge_write_smram(u8 smram);
