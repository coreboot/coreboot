#include <arch/acpi_device.h>

#define NAU8825_MAX_BUTTONS 8

/*
 * Nuvoton NAU8825 audio codec devicetree bindings
 * linux/Documentation/devicetree/bindings/sound/nau8825.txt
 */
struct drivers_i2c_nau8825_config {
	/* Interrupt configuration */
	struct acpi_irq irq;

	/* I2C Bus Frequency in Hertz (default 400kHz) */
	unsigned bus_speed;

	/* Enable jack detection via JKDET pin */
	unsigned jkdet_enable;

	/* Enable JKDET pin pull if set, otherwise high impedance state */
	unsigned jkdet_pull_enable;

	/* Pull-up JKDET pin if set, otherwise pull down */
	unsigned jkdet_pull_up;

	/* JKDET pin polarity, 0 => active high, 1 => active low */
	unsigned jkdet_polarity;

	/*
	 * VREF Impedance selection
	 *  0 - Open
	 *  1 - 25 kOhm
	 *  2 - 125 kOhm
	 *  3 - 2.5 kOhm
	 */
	unsigned vref_impedance;

	/* Button impedance measurement hysteresis */
	unsigned sar_hysteresis;

	/*
	 * Reference voltage for button impedance measurement and micbias
	 *  0 - VDDA
	 *  1 - VDDA
	 *  2 - VDDA * 1.1
	 *  3 - VDDA * 1.2
	 *  4 - VDDA * 1.3
	 *  5 - VDDA * 1.4
	 *  6 - VDDA * 1.53
	 *  7 - VDDA * 1.53
	 */
	unsigned micbias_voltage;
	unsigned sar_voltage;

	/*
	 * SAR compare time
	 *  0 - 500 ns
	 *  1 - 1 us
	 *  2 - 2 us
	 *  3 - 4 us
	 */
	unsigned sar_compare_time;

	/*
	 * SAR sampling time
	 *  0 - 2 us
	 *  1 - 4 us
	 *  2 - 8 us
	 *  3 - 16 us
	 */
	unsigned sar_sampling_time;

	/*
	 * Button short key press debounce time
	 *  0 - 30 ms
	 *  1 - 50 ms
	 *  2 - 100 ms
	 *  3 - 30 ms
	 */
	unsigned short_key_debounce;

	/* Debounce time 2^(n+2) ms (0-7) for jack insert */
	unsigned jack_insert_debounce;

	/* Debounce time 2^(n+2) ms (0-7) for jack eject */
	unsigned jack_eject_debounce;

	/* Number of buttons supported, up to 8 */
	unsigned sar_threshold_num;

	/*
	 * Impedance threshold for each button, up to 8
	 *  SAR = 255 * micbias_voltage / sar_voltage * R / (2000 + R)
	 *  R is Button impedance
	 */
	uint64_t sar_threshold[NAU8825_MAX_BUTTONS];
};
