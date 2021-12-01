/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef EC_CLEVO_IT5570E_SMM_H
#define EC_CLEVO_IT5570E_SMM_H

/* SMI handler */
void ec_smi_apmc(uint8_t apmc);
void ec_smi_sleep(uint8_t slp_type);

#endif /* EC_CLEVO_IT5570E_SMM_H */
