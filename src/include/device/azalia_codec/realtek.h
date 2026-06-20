/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef DEVICE_AZALIA_CODEC_REALTEK_H
#define DEVICE_AZALIA_CODEC_REALTEK_H

enum alc256_pin_widget {
	ALC256_DMIC12		= 0x12,
	ALC256_DMIC34		= 0x13,
	ALC256_SPEAKERS		= 0x14,	// Port-D
	ALC256_MIC2		= 0x19,	// Port-F
	ALC256_LINE1		= 0x1a,	// Port-C
	ALC256_LINE2		= 0x1b,	// Port-E
	ALC256_PC_BEEP		= 0x1d,
	ALC256_SPDIF_OUT	= 0x1e,
	ALC256_HP_OUT		= 0x21,	// Port-I
};

enum alc269_pin_widget {
	ALC269_DMIC12		= 0x12,
	ALC269_SPEAKERS		= 0x14,	// Port-D
	ALC269_VC_HP_OUT	= 0x15,	// Port-A, ALC269-VCx only
	ALC269_MONO		= 0x17,	// Port-H
	ALC269_MIC1		= 0x18,	// Port-B
	ALC269_MIC2		= 0x19,	// Port-F
	ALC269_LINE1		= 0x1a,	// Port-C
	ALC269_LINE2		= 0x1b,	// Port-E
	ALC269_PC_BEEP		= 0x1d,
	ALC269_SPDIF_OUT	= 0x1e,
	ALC269_VB_HP_OUT	= 0x21,	// Port-I, ALC269-VBx only
};

enum alc662_pin_widget {
	ALC662_FRONT		= 0x14, // Port-D
	ALC662_SURROUND		= 0x15, // Port-A
	ALC662_CENTER_LFE	= 0x16, // Port-G
	ALC662_MIC1		= 0x18, // Port-B
	ALC662_MIC2		= 0x19, // Port-F
	ALC662_LINE1		= 0x1a, // Port-C
	ALC662_LINE2		= 0x1b, // Port-E
	ALC662_CD		= 0x1c,
	ALC662_PC_BEEP		= 0x1d,
	ALC662_SPDIF_OUT1	= 0x1e,
};

enum alc887_pin_widget {
	ALC887_SPDIF_OUT2	= 0x11,
	ALC887_DMIC_LR		= 0x12,
	ALC887_FRONT		= 0x14, // Port-D
	ALC887_SURROUND		= 0x15, // Port-A
	ALC887_CENTER_LFE	= 0x16, // Port-G
	ALC887_SIDE_SURROUND	= 0x17, // Port-H
	ALC887_MIC1		= 0x18, // Port-B
	ALC887_MIC2		= 0x19, // Port-F
	ALC887_LINE1		= 0x1a, // Port-C
	ALC887_LINE2		= 0x1b, // Port-E
	ALC887_CD		= 0x1c,
	ALC887_PC_BEEP		= 0x1d,
	ALC887_SPDIF_OUT1	= 0x1e,
	ALC887_SPDIF_IN		= 0x1f,
};

enum alc892_pin_widget {
	ALC892_SPDIF_OUT2	= 0x11,
	ALC892_DMIC_LR		= 0x12,
	ALC892_FRONT		= 0x14, // Port-D
	ALC892_SURROUND		= 0x15, // Port-A
	ALC892_CENTER_LFE	= 0x16, // Port-G
	ALC892_SIDE_SURROUND	= 0x17, // Port-H
	ALC892_MIC1		= 0x18, // Port-B
	ALC892_MIC2		= 0x19, // Port-F
	ALC892_LINE1		= 0x1a, // Port-C
	ALC892_LINE2		= 0x1b, // Port-E
	ALC892_CD		= 0x1c,
	ALC892_PC_BEEP		= 0x1d,
	ALC892_SPDIF_OUT1	= 0x1e,
	ALC892_SPDIF_IN		= 0x1f,
};

enum alc1150_pin_widget {
	ALC1150_SPDIF_OUT2	= 0x11,
	ALC1150_FRONT		= 0x14, // Port-D
	ALC1150_SURROUND	= 0x15, // Port-A
	ALC1150_CENTER_LFE	= 0x16, // Port-G
	ALC1150_SIDE_SURROUND	= 0x17, // Port-H
	ALC1150_MIC1		= 0x18, // Port-B
	ALC1150_MIC2		= 0x19, // Port-F
	ALC1150_LINE1		= 0x1a, // Port-C
	ALC1150_LINE2		= 0x1b, // Port-E
	ALC1150_SPDIF_OUT1	= 0x1e,
};

enum alc1200_pin_widget {
	ALC1200_SPDIF_OUT2	= 0x11,
	ALC1200_FRONT		= 0x14, // Port-D
	ALC1200_SURROUND	= 0x15, // Port-A
	ALC1200_CENTER_LFE	= 0x16, // Port-G
	ALC1200_SIDE_SURROUND	= 0x17, // Port-H
	ALC1200_MIC1		= 0x18, // Port-B
	ALC1200_MIC2		= 0x19, // Port-F
	ALC1200_LINE1		= 0x1a, // Port-C
	ALC1200_LINE2		= 0x1b, // Port-E
	ALC1200_SPDIF_OUT1	= 0x1e,
};

#endif /* DEVICE_AZALIA_CODEC_REALTEK_H */
