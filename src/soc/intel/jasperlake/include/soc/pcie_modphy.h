/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_JASPERLAKE_PCIE_MODPHY_H_
#define _SOC_JASPERLAKE_PCIE_MODPHY_H_

struct pcie_modphy_config {
	/* TX Output Downscale Amplitude Adjustment */
	bool tx_gen1_downscale_amp_override;
	uint8_t tx_gen1_downscale_amp;
	/* TX Output Downscale Amplitude Adjustment */
	bool tx_gen2_downscale_amp_override;
	uint8_t tx_gen2_downscale_amp;
	/* TX Output Downscale Amplitude Adjustment */
	bool tx_gen3_downscale_amp_override;
	uint8_t tx_gen3_downscale_amp;
	/* TX Output -3.5dB Mode De-Emphasis Adjustment Setting */
	uint8_t tx_gen1_de_emph;
	/* TX Output -3.5dB Mode De-Emphasis Adjustment Setting */
	uint8_t tx_gen2_de_emph_3p5;
	/* TX Output -6.0dB Mode De-Emphasis Adjustment Setting */
	uint8_t tx_gen2_de_emph_6p0;
};

#endif
