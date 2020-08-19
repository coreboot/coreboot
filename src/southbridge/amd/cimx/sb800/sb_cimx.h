/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _CIMX_H_
#define _CIMX_H_

/**
 * AMD South Bridge CIMx entry point wrapper
 */
void sb_Poweron_Init(void);
void sb_Before_Pci_Init(void);
void sb_After_Pci_Init(void);
void sb_Mid_Post_Init(void);
void sb_Late_Post(void);
void sb_Before_Pci_Restore_Init(void);
void sb_After_Pci_Restore_Init(void);

/**
 * CIMX not set the clock to 48Mhz until sbBeforePciInit,
 * coreboot may need to set this even more earlier
 */
void sb800_clk_output_48Mhz(void);

#endif
