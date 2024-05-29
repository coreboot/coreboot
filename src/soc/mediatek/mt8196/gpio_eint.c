/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8196_EINT_Datasheet
 * Chapter number: 1
 */

#include <console/console.h>
#include <soc/addressmap.h>
#include <soc/gpio.h>

enum {
	EINT_INVALID = 0,
	EINT_E,
	EINT_S,
	EINT_W,
	EINT_N,
	EINT_C,
};

struct eint_info {
	uint8_t instance;
	uint8_t index;
};

static struct eint_info eint_data[] = {
	/* instance, index */
	[0] = { EINT_W, 0 },
	[1] = { EINT_W, 1 },
	[2] = { EINT_W, 16 },
	[3] = { EINT_W, 17 },
	[4] = { EINT_W, 2 },
	[5] = { EINT_W, 3 },
	[6] = { EINT_W, 4 },
	[7] = { EINT_W, 5 },
	[8] = { EINT_W, 6 },
	[9] = { EINT_W, 18 },
	[10] = { EINT_W, 7 },
	[11] = { EINT_W, 8 },
	[12] = { EINT_W, 9 },
	[13] = { EINT_S, 4 },
	[14] = { EINT_E, 0 },
	[15] = { EINT_S, 5 },
	[16] = { EINT_S, 6 },
	[17] = { EINT_S, 7 },
	[18] = { EINT_S, 8 },
	[19] = { EINT_S, 9 },
	[20] = { EINT_E, 1 },
	[21] = { EINT_E, 10 },
	[22] = { EINT_E, 11 },
	[23] = { EINT_E, 12 },
	[24] = { EINT_E, 13 },
	[25] = { EINT_E, 14 },
	[26] = { EINT_E, 15 },
	[27] = { EINT_E, 2 },
	[28] = { EINT_E, 16 },
	[29] = { EINT_E, 17 },
	[30] = { EINT_E, 18 },
	[31] = { EINT_E, 3 },
	[32] = { EINT_E, 19 },
	[33] = { EINT_E, 20 },
	[34] = { EINT_E, 21 },
	[35] = { EINT_E, 22 },
	[36] = { EINT_E, 23 },
	[37] = { EINT_E, 24 },
	[38] = { EINT_E, 25 },
	[39] = { EINT_W, 10 },
	[40] = { EINT_W, 11 },
	[41] = { EINT_W, 12 },
	[42] = { EINT_W, 13 },
	[43] = { EINT_W, 14 },
	[44] = { EINT_W, 19 },
	[45] = { EINT_W, 20 },
	[46] = { EINT_W, 21 },
	[47] = { EINT_W, 22 },
	[48] = { EINT_W, 23 },
	[49] = { EINT_W, 24 },
	[50] = { EINT_W, 25 },
	[51] = { EINT_W, 26 },
	[60] = { EINT_W, 27 },
	[61] = { EINT_W, 28 },
	[62] = { EINT_W, 29 },
	[63] = { EINT_W, 30 },
	[64] = { EINT_W, 31 },
	[65] = { EINT_W, 32 },
	[70] = { EINT_W, 33 },
	[71] = { EINT_W, 34 },
	[72] = { EINT_W, 35 },
	[73] = { EINT_W, 36 },
	[74] = { EINT_W, 37 },
	[79] = { EINT_W, 38 },
	[80] = { EINT_W, 39 },
	[81] = { EINT_W, 40 },
	[82] = { EINT_W, 41 },
	[83] = { EINT_W, 42 },
	[84] = { EINT_W, 43 },
	[85] = { EINT_W, 44 },
	[86] = { EINT_W, 45 },
	[87] = { EINT_W, 46 },
	[88] = { EINT_W, 47 },
	[89] = { EINT_W, 48 },
	[90] = { EINT_W, 49 },
	[91] = { EINT_W, 50 },
	[92] = { EINT_W, 15 },
	[93] = { EINT_W, 51 },
	[94] = { EINT_W, 52 },
	[95] = { EINT_W, 53 },
	[96] = { EINT_W, 54 },
	[97] = { EINT_W, 55 },
	[98] = { EINT_W, 56 },
	[103] = { EINT_W, 57 },
	[106] = { EINT_S, 10 },
	[107] = { EINT_S, 11 },
	[108] = { EINT_S, 12 },
	[109] = { EINT_S, 13 },
	[110] = { EINT_S, 0 },
	[111] = { EINT_S, 1 },
	[112] = { EINT_S, 2 },
	[113] = { EINT_S, 3 },
	[114] = { EINT_S, 14 },
	[115] = { EINT_S, 15 },
	[116] = { EINT_S, 16 },
	[117] = { EINT_S, 17 },
	[118] = { EINT_S, 18 },
	[119] = { EINT_S, 19 },
	[120] = { EINT_S, 20 },
	[121] = { EINT_S, 21 },
	[122] = { EINT_S, 22 },
	[125] = { EINT_S, 23 },
	[126] = { EINT_S, 24 },
	[127] = { EINT_S, 25 },
	[128] = { EINT_S, 26 },
	[129] = { EINT_S, 27 },
	[130] = { EINT_S, 28 },
	[137] = { EINT_E, 26 },
	[138] = { EINT_E, 27 },
	[139] = { EINT_E, 28 },
	[140] = { EINT_E, 29 },
	[141] = { EINT_E, 30 },
	[142] = { EINT_E, 31 },
	[143] = { EINT_E, 32 },
	[144] = { EINT_E, 33 },
	[145] = { EINT_E, 34 },
	[146] = { EINT_E, 35 },
	[147] = { EINT_E, 36 },
	[148] = { EINT_E, 4 },
	[149] = { EINT_E, 37 },
	[150] = { EINT_E, 5 },
	[151] = { EINT_E, 38 },
	[152] = { EINT_E, 39 },
	[153] = { EINT_E, 40 },
	[154] = { EINT_E, 41 },
	[155] = { EINT_E, 42 },
	[156] = { EINT_E, 43 },
	[157] = { EINT_E, 44 },
	[158] = { EINT_E, 45 },
	[159] = { EINT_E, 46 },
	[160] = { EINT_E, 47 },
	[161] = { EINT_E, 48 },
	[162] = { EINT_E, 49 },
	[163] = { EINT_E, 50 },
	[164] = { EINT_E, 51 },
	[165] = { EINT_E, 52 },
	[166] = { EINT_E, 53 },
	[167] = { EINT_E, 54 },
	[168] = { EINT_E, 55 },
	[169] = { EINT_E, 56 },
	[170] = { EINT_E, 57 },
	[171] = { EINT_E, 58 },
	[172] = { EINT_E, 6 },
	[173] = { EINT_E, 7 },
	[174] = { EINT_E, 8 },
	[175] = { EINT_E, 9 },
	[178] = { EINT_E, 59 },
	[179] = { EINT_E, 60 },
	[180] = { EINT_E, 61 },
	[181] = { EINT_E, 62 },
	[182] = { EINT_E, 63 },
	[183] = { EINT_E, 64 },
	[184] = { EINT_E, 65 },
	[185] = { EINT_E, 66 },
	[186] = { EINT_N, 6 },
	[187] = { EINT_N, 7 },
	[192] = { EINT_N, 8 },
	[193] = { EINT_N, 9 },
	[196] = { EINT_N, 10 },
	[197] = { EINT_N, 11 },
	[204] = { EINT_N, 12 },
	[205] = { EINT_N, 13 },
	[206] = { EINT_N, 14 },
	[207] = { EINT_N, 0 },
	[208] = { EINT_N, 1 },
	[209] = { EINT_N, 2 },
	[210] = { EINT_N, 15 },
	[211] = { EINT_N, 3 },
	[212] = { EINT_N, 4 },
	[213] = { EINT_N, 5 },
	[216] = { EINT_N, 16 },
	[217] = { EINT_N, 17 },
	[218] = { EINT_N, 18 },
	[219] = { EINT_N, 19 },
	[220] = { EINT_N, 20 },
	[221] = { EINT_N, 21 },
	[222] = { EINT_N, 22 },
	[223] = { EINT_N, 23 },
	[224] = { EINT_N, 24 },
	[225] = { EINT_N, 25 },
	[226] = { EINT_N, 26 },
	[227] = { EINT_N, 27 },
	[228] = { EINT_N, 28 },
	[229] = { EINT_N, 29 },
	[241] = { EINT_N, 30 },
	[242] = { EINT_N, 31 },
	[243] = { EINT_N, 32 },
	[245] = { EINT_N, 45 },
	[251] = { EINT_E, 67 },
	[252] = { EINT_E, 68 },
	[253] = { EINT_E, 69 },
	[254] = { EINT_E, 70 },
	[255] = { EINT_E, 71 },
	[256] = { EINT_E, 72 },
	[257] = { EINT_E, 73 },
	[258] = { EINT_E, 74 },
	[259] = { EINT_N, 33 },
	[260] = { EINT_N, 34 },
	[261] = { EINT_N, 35 },
	[262] = { EINT_N, 36 },
	[263] = { EINT_N, 37 },
	[264] = { EINT_N, 38 },
	[265] = { EINT_N, 39 },
	[266] = { EINT_N, 40 },
	[267] = { EINT_N, 41 },
	[268] = { EINT_N, 42 },
	[269] = { EINT_N, 43 },
	[270] = { EINT_N, 44 },
	[271] = { EINT_C, 0 },
	[272] = { EINT_C, 1 },
	[273] = { EINT_C, 2 },
	[274] = { EINT_C, 3 },
	[275] = { EINT_C, 4 },
	[276] = { EINT_C, 5 },
	[277] = { EINT_C, 6 },
	[278] = { EINT_C, 7 },
	[279] = { EINT_C, 8 },
	[280] = { EINT_C, 9 },
	[281] = { EINT_C, 10 },
	[282] = { EINT_C, 11 },
	[283] = { EINT_C, 12 },
	[284] = { EINT_C, 13 },
	[285] = { EINT_C, 14 },
	[286] = { EINT_C, 15 },
	[287] = { EINT_C, 16 },
	[288] = { EINT_C, 17 },
	[289] = { EINT_C, 18 },
	[290] = { EINT_C, 19 },
	[291] = { EINT_C, 20 },
	[292] = { EINT_C, 21 },
};
_Static_assert(ARRAY_SIZE(eint_data) == 293);

void gpio_calc_eint_pos_bit(gpio_t gpio, u32 *pos, u32 *bit)
{
	uint32_t idx = gpio.id;

	*pos = 0;
	*bit = 0;

	if (idx >= ARRAY_SIZE(eint_data))
		return;

	uint8_t index = eint_data[idx].index;

	*pos = index / MAX_EINT_REG_BITS;
	*bit = index % MAX_EINT_REG_BITS;
}

struct eint_regs *gpio_get_eint_reg(gpio_t gpio)
{
	uint32_t idx = gpio.id;
	uintptr_t addr;

	if (idx >= ARRAY_SIZE(eint_data))
		return NULL;

	switch (eint_data[idx].instance) {
	case EINT_E:
		addr = EINT_E_BASE;
		break;
	case EINT_S:
		addr = EINT_S_BASE;
		break;
	case EINT_W:
		addr = EINT_W_BASE;
		break;
	case EINT_N:
		addr = EINT_N_BASE;
		break;
	case EINT_C:
		addr = EINT_C_BASE;
		break;
	default:
		printk(BIOS_ERR, "%s: Failed to look up a valid EINT base for %d\n",
		       __func__, idx);
		return NULL;
	}

	return (void *)addr;
}
