/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Damien Zammit <damien@zamaudio.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/mtrr.h>
#include <delay.h>
#include <halt.h>
#include <lib.h>
#include <northbridge/intel/x4x/x4x.h>
#include <pc80/mc146818rtc.h>
#include <spd.h>
#include <string.h>

static inline int spd_read_byte(unsigned int device, unsigned int address)
{
	return smbus_read_byte(device, address);
}

static void sdram_read_spds(struct sysinfo *s)
{
	u8 i, j, chan;
	int status = 0;
	FOR_EACH_DIMM(i) {
		if (s->spd_map[i] == 0) {
			/* Non-existent SPD address */
			s->dimms[i].card_type = 0;
			continue;
		}
		for (j = 0; j < 64; j++) {
			status = spd_read_byte(s->spd_map[i], j);
			if (status < 0) {
				/* No SPD here */
				s->dimms[i].card_type = 0;
				break;
			}
			s->dimms[i].spd_data[j] = (u8) status;
			if (j == 62)
				s->dimms[i].card_type = ((u8) status) & 0x1f;
		}
		if (status >= 0) {
			hexdump(s->dimms[i].spd_data, 64);
		}
	}

	s->spd_type = 0;
	int fail = 1;
	FOR_EACH_POPULATED_DIMM(s->dimms, i) {
		switch ((enum ddrxspd) s->dimms[i].spd_data[2]) {
			case DDR2SPD:
				if (s->spd_type == 0) {
					s->spd_type = DDR2;
				} else if (s->spd_type == DDR3) {
					die("DIMM type mismatch\n");
				}
				break;
			case DDR3SPD:
			default:
				if (s->spd_type == 0) {
					s->spd_type = DDR3;
				} else if (s->spd_type == DDR2) {
					die("DIMM type mismatch\n");
				}
				break;
		}
	}
	if (s->spd_type == DDR3) {
		FOR_EACH_POPULATED_DIMM(s->dimms, i) {
			s->dimms[i].sides = (s->dimms[i].spd_data[5] & 0x0f) + 1;
			s->dimms[i].ranks = ((s->dimms[i].spd_data[7] >> 3) & 0x7) + 1;
			s->dimms[i].chip_capacity = (s->dimms[i].spd_data[4] & 0xf);
			s->dimms[i].banks = 8;
			s->dimms[i].rows = ((s->dimms[i].spd_data[5] >> 3) & 0x7) + 12;
			s->dimms[i].cols = (s->dimms[i].spd_data[5] & 0x7) + 9;
			s->dimms[i].cas_latencies = 0xfe;
			s->dimms[i].cas_latencies &= (s->dimms[i].spd_data[14] << 1);
			if (s->dimms[i].cas_latencies == 0)
				s->dimms[i].cas_latencies = 0x40;
			s->dimms[i].tAAmin = s->dimms[i].spd_data[16];
			s->dimms[i].tCKmin = s->dimms[i].spd_data[12];
			s->dimms[i].width = s->dimms[i].spd_data[7] & 0x7;
			s->dimms[i].page_size = s->dimms[i].width * (1 << s->dimms[i].cols); // Bytes
			s->dimms[i].tRAS = ((s->dimms[i].spd_data[21] & 0xf) << 8) |
				s->dimms[i].spd_data[22];
			s->dimms[i].tRP = s->dimms[i].spd_data[20];
			s->dimms[i].tRCD = s->dimms[i].spd_data[18];
			s->dimms[i].tWR = s->dimms[i].spd_data[17];
			fail = 0;
		}
	} else if (s->spd_type == DDR2) {
		FOR_EACH_POPULATED_DIMM(s->dimms, i) {
			s->dimms[i].sides = (s->dimms[i].spd_data[5] & 0x7) + 1;
			s->dimms[i].banks = (s->dimms[i].spd_data[17] >> 2) - 1;
			s->dimms[i].chip_capacity = s->dimms[i].banks;
			s->dimms[i].rows = s->dimms[i].spd_data[3];// - 12;
			s->dimms[i].cols = s->dimms[i].spd_data[4];// - 9;
			s->dimms[i].cas_latencies = 0x78;
			s->dimms[i].cas_latencies &= s->dimms[i].spd_data[18];
			if (s->dimms[i].cas_latencies == 0)
				s->dimms[i].cas_latencies = 7;
			s->dimms[i].tAAmin = s->dimms[i].spd_data[26];
			s->dimms[i].tCKmin = s->dimms[i].spd_data[25];
			s->dimms[i].width = (s->dimms[i].spd_data[13] >> 3) - 1;
			s->dimms[i].page_size = (s->dimms[i].width+1) * (1 << s->dimms[i].cols); // Bytes
			s->dimms[i].tRAS = s->dimms[i].spd_data[30];
			s->dimms[i].tRP = s->dimms[i].spd_data[27];
			s->dimms[i].tRCD = s->dimms[i].spd_data[29];
			s->dimms[i].tWR = s->dimms[i].spd_data[36];
			s->dimms[i].ranks = s->dimms[i].sides; // XXX

			printk(BIOS_DEBUG, "DIMM %d\n", i);
			printk(BIOS_DEBUG, "  Sides     : %d\n", s->dimms[i].sides);
			printk(BIOS_DEBUG, "  Banks     : %d\n", s->dimms[i].banks);
			printk(BIOS_DEBUG, "  Ranks     : %d\n", s->dimms[i].ranks);
			printk(BIOS_DEBUG, "  Rows      : %d\n", s->dimms[i].rows);
			printk(BIOS_DEBUG, "  Cols      : %d\n", s->dimms[i].cols);
			printk(BIOS_DEBUG, "  Page size : %d\n", s->dimms[i].page_size);
			printk(BIOS_DEBUG, "  Width     : %d\n", (s->dimms[i].width+1)*8);
			fail = 0;
		}
	}
	if (fail) {
		die("No memory dimms, halt\n");
	}

	FOR_EACH_POPULATED_CHANNEL(s->dimms, chan) {
		if (s->dimms[chan>>1].sides == 0) {
			// NC
			if (s->dimms[(chan>>1) + 1].sides == 0) {
				// NC/NC
				s->dimm_config[chan] = 0;
			} else if (s->dimms[(chan>>1) + 1].sides == 1) {
				// NC/SS
				if (s->dimms[(chan>>1) + 1].width == 0) {
					// NC/8SS
					s->dimm_config[chan] = 4;
				} else {
					// NC/16SS
					s->dimm_config[chan] = 12;
				}
			} else {
				// NC/DS
				if (s->dimms[(chan>>1) + 1].width == 0) {
					// NC/8DS
					s->dimm_config[chan] = 8;
				} else {
					// NOT SUPPORTED
					die("16DS Not supported\n");
				}
			}
		} else if (s->dimms[chan>>1].sides == 1) {
			// SS
			if (s->dimms[(chan>>1) + 1].sides == 0) {
				// SS/NC
				if (s->dimms[chan>>1].width == 0) {
					// 8SS/NC
					s->dimm_config[chan] = 1;
				} else {
					// 16SS/NC
					s->dimm_config[chan] = 3;
				}
			} else if (s->dimms[(chan>>1) + 1].sides == 1) {
				// SS/SS
				if (s->dimms[chan>>1].width == 0) {
					if (s->dimms[(chan>>1) + 1].width == 0) {
						// 8SS/8SS
						s->dimm_config[chan] = 5;
					} else {
						// 8SS/16SS
						s->dimm_config[chan] = 13;
					}
				} else {
					if (s->dimms[(chan>>1) + 1].width == 0) {
						// 16SS/8SS
						s->dimm_config[chan] = 7;
					} else {
						// 16SS/16SS
						s->dimm_config[chan] = 15;
					}
				}
			} else {
				// SS/DS
				if (s->dimms[chan>>1].width == 0) {
					if (s->dimms[(chan>>1) + 1].width == 0) {
						// 8SS/8DS
						s->dimm_config[chan] = 9;
					} else {
						die("16DS not supported\n");
					}
				} else {
					if (s->dimms[(chan>>1) + 1].width == 0) {
						// 16SS/8DS
						s->dimm_config[chan] = 11;
					} else {
						die("16DS not supported\n");
					}
				}
			}
		} else {
			// DS
			if (s->dimms[(chan>>1) + 1].sides == 0) {
				// DS/NC
				if (s->dimms[chan>>1].width == 0) {
					// 8DS/NC
					s->dimm_config[chan] = 2;
				} else {
					die("16DS not supported\n");
				}
			} else if (s->dimms[(chan>>1) + 1].sides == 1) {
				// DS/SS
				if (s->dimms[chan>>1].width == 0) {
					if (s->dimms[(chan>>1) + 1].width == 0) {
						// 8DS/8SS
						s->dimm_config[chan] = 6;
					} else {
						// 8DS/16SS
						s->dimm_config[chan] = 14;
					}
				} else {
					die("16DS not supported\n");
				}
			} else {
				// DS/DS
				if (s->dimms[chan>>1].width == 0 && s->dimms[(chan>>1)+1].width == 0) {
					// 8DS/8DS
					s->dimm_config[chan] = 10;
				}
			}
		}
		printk(BIOS_DEBUG, "  Config[CH%d] : %d\n", chan, s->dimm_config[chan]);
	}
}

static u8 lsbpos(u8 val) //Forward
{
	u8 i;
	for (i = 0; (i < 8) && ((val & (1 << i)) == 0); i++);
	return i;
}

static u8 msbpos(u8 val) //Reverse
{
	u8 i;
	for (i = 7; (i >= 0) && ((val & (1 << i)) == 0); i--);
	return i;
}

static void mchinfo_ddr2(struct sysinfo *s)
{
	const u32 eax = cpuid_ext(0x04, 0).eax;
	s->cores = ((eax >> 26) & 0x3f) + 1;
	printk(BIOS_WARNING, "%d CPU cores\n", s->cores);

	u32 capid = pci_read_config16(PCI_DEV(0,0,0), 0xe8);
	if (!(capid & (1<<(79-64)))) {
		printk(BIOS_WARNING, "iTPM enabled\n");
	}

	capid = pci_read_config32(PCI_DEV(0, 0, 0), 0xe4);
	if (!(capid & (1<<(57-32)))) {
		printk(BIOS_WARNING, "ME enabled\n");
	}

	if (!(capid & (1<<(56-32)))) {
		printk(BIOS_WARNING, "AMT enabled\n");
	}

	s->max_ddr2_mhz = (capid & (1<<(53-32)))?667:800;
	printk(BIOS_WARNING, "Capable of DDR2 of %d MHz or lower\n", s->max_ddr2_mhz);

	if (!(capid & (1<<(48-32)))) {
		printk(BIOS_WARNING, "VT-d enabled\n");
	}
}

static void sdram_detect_ram_speed(struct sysinfo *s)
{
	u8 i;
	u8 commoncas = 0;
	u8 cas;
	u8 lowcas;
	u8 highcas;
	u8 maxfreq;
	u8 freq = 0;

	// Find max FSB speed
	switch (MCHBAR32(0xc00) & 0x7) {
	case 0x0:
		s->max_fsb = FSB_CLOCK_1066MHz;
		break;
	case 0x2:
		s->max_fsb = FSB_CLOCK_800MHz;
		break;
	case 0x4:
		s->max_fsb = FSB_CLOCK_1333MHz;
		break;
	default:
		s->max_fsb = FSB_CLOCK_800MHz;
		printk(BIOS_WARNING, "Can't detect FSB, setting 800MHz\n");
		break;
	}

	// Find RAM speed
	maxfreq = (u8) ((pci_read_config16(PCI_DEV(0,0,0), 0xea) >> 4) & 0x3f);
	if (s->spd_type == DDR2) {

		// Limit frequency for MCH
		maxfreq &= 0x7;
		freq = MEM_CLOCK_800MHz;
		if (maxfreq) {
			freq = maxfreq;
		}
		if (freq > MEM_CLOCK_800MHz) {
			freq = MEM_CLOCK_800MHz;
		}

		// Detect a common CAS latency
		commoncas = 0xff;
		FOR_EACH_POPULATED_DIMM(s->dimms, i) {
			commoncas &= s->dimms[i].spd_data[18];
		}
		if (commoncas == 0) {
			die("No common CAS among dimms\n");
		}

		// Start with fastest common CAS
		cas = 0;
		highcas = msbpos(commoncas);
		lowcas = lsbpos(commoncas);

		while (cas == 0 && highcas >= lowcas) {
			FOR_EACH_POPULATED_DIMM(s->dimms, i) {
				switch (freq) {
				case MEM_CLOCK_800MHz:
					if ((s->dimms[i].spd_data[9] > 0x25) ||
					    (s->dimms[i].spd_data[10] > 0x40)) {
						// CAS too fast, lower it
						highcas--;
						break;
					} else {
						cas = highcas;
					}
					break;
				case MEM_CLOCK_667MHz:
				default:
					if ((s->dimms[i].spd_data[9] > 0x30) ||
					    (s->dimms[i].spd_data[10] > 0x45)) {
						// CAS too fast, lower it
						highcas--;
						break;
					} else {
						cas = highcas;
					}
					break;
				}
			}
		}
		if (highcas < lowcas) {
			// Timings not supported by MCH, lower the frequency
			freq--;
			cas = 0;
			highcas = msbpos(commoncas);
			lowcas = lsbpos(commoncas);
			while (cas == 0 && highcas >= lowcas) {
				FOR_EACH_POPULATED_DIMM(s->dimms, i) {
					switch (freq) {
					case MEM_CLOCK_800MHz:
						if ((s->dimms[i].spd_data[23] > 0x25) ||
						    (s->dimms[i].spd_data[24] > 0x40)) {
							// CAS too fast, lower it
							highcas--;
							break;
						} else {
							cas = highcas;
						}
						break;
					case MEM_CLOCK_667MHz:
					default:
						if ((s->dimms[i].spd_data[23] > 0x30) ||
						    (s->dimms[i].spd_data[24] > 0x45)) {
							// CAS too fast, lower it
							highcas--;
							break;
						} else {
							cas = highcas;
						}
						break;
					}
				}
			}
		}
		s->selected_timings.mem_clk = freq;
		s->selected_timings.CAS = cas;

	} else { // DDR3
		// Limit frequency for MCH
		maxfreq >>= 3;
		freq = MEM_CLOCK_1333MHz;
		if (maxfreq) {
			freq = maxfreq + 2;
		}
		if (freq > MEM_CLOCK_1333MHz) {
			freq = MEM_CLOCK_1333MHz;
		}

		// Limit DDR speed to FSB speed
		switch (s->max_fsb) {
		case FSB_CLOCK_800MHz:
			if (freq > MEM_CLOCK_800MHz) {
				freq = MEM_CLOCK_800MHz;
			}
			break;
		case FSB_CLOCK_1066MHz:
			if (freq > MEM_CLOCK_1066MHz) {
				freq = MEM_CLOCK_1066MHz;
			}
			break;
		case FSB_CLOCK_1333MHz:
			if (freq > MEM_CLOCK_1333MHz) {
				freq = MEM_CLOCK_1333MHz;
			}
			break;
		default:
			die("Invalid FSB\n");
			break;
		}

		// TODO: CAS detection for DDR3
	}
}

/**
 * @param boot_path: 0 = normal, 1 = reset, 2 = resume from s3
 */
void sdram_initialize(int boot_path, const u8 *spd_map)
{
	struct sysinfo s;
	u8 reg8;

	printk(BIOS_DEBUG, "Setting up RAM controller.\n");

	pci_write_config8(PCI_DEV(0,0,0), 0xdf, 0xff);

	memset(&s, 0, sizeof(struct sysinfo));

	s.boot_path = boot_path;
	s.spd_map[0] = spd_map[0];
	s.spd_map[1] = spd_map[1];
	s.spd_map[2] = spd_map[2];
	s.spd_map[3] = spd_map[3];

	/* Detect dimms per channel */
	s.dimms_per_ch = 2;
	reg8 = pci_read_config8(PCI_DEV(0,0,0), 0xe9);
	if (reg8 & 0x10)
		s.dimms_per_ch = 1;

	printk(BIOS_DEBUG, "Dimms per channel: %d\n", s.dimms_per_ch);

	mchinfo_ddr2(&s);

	sdram_read_spds(&s);

	/* Choose Common Frequency */
	sdram_detect_ram_speed(&s);

	switch (s.spd_type) {
	case DDR2:
		raminit_ddr2(&s);
		break;
	case DDR3:
		// FIXME Add: raminit_ddr3(&s);
		break;
	default:
		die("Unknown DDR type\n");
		break;
	}

	reg8 = pci_read_config8(PCI_DEV(0, 0x1f, 0), 0xa2);
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0xa2, reg8 & ~0x80);

	reg8 = pci_read_config8(PCI_DEV(0,0,0), 0xf4);
	pci_write_config8(PCI_DEV(0,0,0), 0xf4, reg8 | 1);
	printk(BIOS_DEBUG, "RAM initialization finished.\n");
}
