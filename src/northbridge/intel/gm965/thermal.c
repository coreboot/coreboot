/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Intel GM965 (Crestline) Northbridge - Thermal Sensor Initialization
 * Reverse-engineered from ThinkPad X61 Phoenix BIOS
 *
 * BIOS functions:
 *   FFF027F0 (thermal_sensor_init_all)     - Iterates DIMMs, POST 0xFF42
 *   FFF02709 (thermal_sensor_init_per_dimm) - Per-DIMM TSE2004 config
 *   FFF02628 (smbus_word_read)              - SMBus word read protocol
 *   FFF0269F (smbus_word_write)             - SMBus word write protocol
 *   FFFF56C6 (raminit3_thermal_init)        - RAMINIT3 copy
 *   FFFF55DF (raminit3_thermal_per_dimm)    - RAMINIT3 copy
 *
 * TSE2004-compatible thermal sensors are present on DDR2 DIMMs at
 * SMBus addresses 0x18-0x1B (7-bit) = 0x30-0x36 (8-bit with R/W).
 *
 * Reference: coreboot GM45 thermal.c + JEDEC TSE2004 spec
 */

#include <stdint.h>
#include <device/smbus_host.h>
#include <console/console.h>
#include <northbridge/intel/gm965/gm965.h>

/*
 * smbus_write_word() wrapper - coreboot's smbus_host.h does not provide
 * a static inline for smbus_write_word, only do_smbus_write_word.
 */
static inline int smbus_write_word(u8 device, u8 address, u16 data)
{
	uintptr_t base = smbus_base();
	return do_smbus_write_word(base, device, address, data);
}

/* TSE2004 thermal sensor registers */
#define TSE2004_CAPABILITY  0x00
#define TSE2004_CONFIG      0x01
#define TSE2004_ALARM_HIGH  0x02
#define TSE2004_ALARM_LOW   0x03
#define TSE2004_CRITICAL    0x04
#define TSE2004_TEMPERATURE 0x05
#define TSE2004_MANUF_ID    0x06
#define TSE2004_DEVICE_ID   0x07

/* TSE2004 SMBus base addresses (7-bit: 0x18-0x1B for 4 DIMM slots) */
#define TSE2004_SLAVE_BASE  0x18

/*
 * thermal_sensor_init_dimm() - Configure a single DIMM thermal sensor
 *
 * Reads the capability register to verify a TSE2004 sensor is present,
 * then programs alarm thresholds and enables continuous monitoring.
 *
 * BIOS function: FFF02709 / FFFF55DF (231 bytes)
 * The BIOS sets:
 *   - Alarm high: +85C (0x0A80)
 *   - Critical:   +100C (0x0C80)
 *   - Config: continuous conversion, comparator mode
 */
static void thermal_sensor_init_dimm(int slot)
{
	uint8_t slave = TSE2004_SLAVE_BASE + slot;
	int cap;

	/* Check if sensor is present by reading capability register */
	cap = smbus_read_word(slave, TSE2004_CAPABILITY);
	if (cap < 0)
		return;  /* No sensor on this DIMM */

	/*
	 * Program alarm thresholds.
	 * TSE2004 temperature format: sign(15), 4-bit integer(14:11),
	 * 4-bit fraction(10:7), 4 reserved(6:3), 3 flags(2:0)
	 *
	 * High alarm = +85C = 0x0A80
	 * Critical   = +100C = 0x0C80
	 * Low alarm  = +0C = 0x0000
	 */
	smbus_write_word(slave, TSE2004_ALARM_HIGH, 0x0A80);
	smbus_write_word(slave, TSE2004_CRITICAL,   0x0C80);
	smbus_write_word(slave, TSE2004_ALARM_LOW,  0x0000);

	/*
	 * Configure sensor:
	 *   Bit 10: EVENT output control = active-low (0)
	 *   Bit 9:  Tcrit lock = 0 (not locked)
	 *   Bit 8:  window lock = 0 (not locked)
	 *   Bits 6:5: resolution = 11 (0.0625 C, ~250ms conversion)
	 *   Bit 4:  interrupt clear = 0
	 *   Bit 3:  alarm window = 0 (comparator mode)
	 *   Bit 2:  critical only = 0
	 *   Bit 1:  SHDN = 0 (continuous conversion)
	 *   Bit 0:  hyst enable = 0
	 *
	 * Value: 0x0060 (resolution = max, continuous, comparator)
	 */
	smbus_write_word(slave, TSE2004_CONFIG, 0x0060);
}

/*
 * gm965_thermal_init() - Initialize thermal sensors on all populated DIMMs
 *
 * BIOS function: FFF027F0 (72 bytes, POST 0xFF42)
 *
 * The vendor's RAMINIT copy (actual execution path, FFF0xxxx) never
 * calls the thermal sensor init - FFF027F0 has zero call sites in
 * the entire BIOS image.  The RAMINIT3 copy (FFFF3xxx, not executed)
 * does call its equivalent at FFFF56C6 from FFFF53C5.
 *
 * The vendor function only programs TSE2004 DIMM thermal sensors via
 * SMBus, plus a single conditional MCHBAR write: THERMAL_ENABLE = 0xD0
 * if any sensor was found.  It does NOT write any PM_CH / PM_THRT
 * registers - those are programmed exclusively by dram_power_mgmt().
 *
 * Previous code had GM45-derived MCHBAR writes (0x1070, 0x1080,
 * 0x1001-0x105c, 0x1290/0x12b4, 0x11d0/0x11d4) that destructively
 * overwrote the values set by dram_power_mgmt().  Removed to match
 * the vendor's RAMINIT copy which never does these writes.
 */
void gm965_thermal_init(sysinfo_t *si)
{
	int slot;
	int found = 0;

	/*
	 * Initialize TSE2004 thermal sensors on each populated DIMM.
	 * X61 has 2 DIMM slots (1 per channel), mapped to:
	 *   Slot 0 = Ch0 DIMM0 = TSE2004 at SMBus 0x18
	 *   Slot 1 = Ch1 DIMM0 = TSE2004 at SMBus 0x19
	 *
	 * Vendor FFF027F0 iterates all 4 DIMM slots (7-byte stride
	 * through the per-DIMM array), calls FFF02709 per present DIMM.
	 */
	for (slot = 0; slot < 4; slot++) {
		if (si->dimms[slot].present) {
			thermal_sensor_init_dimm(slot);
			found = 1;
		}
	}

	/*
	 * Vendor FFF027F0 line 29: if any sensor init succeeded,
	 * write 0xD0 to THERMAL_ENABLE (MCHBAR + 0x10EF).
	 * Byte write, conditional on sensor presence.
	 */
	if (found)
		mchbar_write8(THERMAL_ENABLE, 0xd0);
}
