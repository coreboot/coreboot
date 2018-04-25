#include <arch/acpi_device.h>

/*
 * Dialog Semiconductor DA7219 Audio Codec devicetree bindings
 * linux/Documentation/devicetree/bindings/sound/da7219.txt
 */
struct drivers_i2c_da7219_config {
	/* Interrupt configuration */
	struct acpi_irq irq;

	/* Use GPIO based interrupt instead of PIRQ */
	struct acpi_gpio irq_gpio;

	/* I2C Bus Frequency in Hertz (default 400kHz) */
	unsigned bus_speed;

	/*
	 * micbias-lvl : Voltage (mV) for Mic Bias
	 * [<1600>, <1800>, <2000>, <2200>, <2400>, <2600>]
	 */
	unsigned micbias_lvl;

	/*
	 * mic-amp-in-sel : Mic input source type
	 * ["diff", "se_p", "se_n"]
	 */
	const char *mic_amp_in_sel;

	/*
	 * Properties for da7219_aad child node
	 */

	/*
	 * micbias-pulse-lvl : Mic bias higher voltage pulse level (mV)
	 * [<2800>, <2900>]
	 */
	unsigned micbias_pulse_lvl;

	/*
	 * micbias-pulse-time : Mic bias higher voltage pulse duration (ms)
	 */
	unsigned micbias_pulse_time;

	/*
	 * btn-cfg : Periodic button press measurements for 4-pole jack (ms)
	 * [<2>, <5>, <10>, <50>, <100>, <200>, <500>]
	 */
	unsigned btn_cfg;

	/*
	 * mic-det-thr : Impedance threshold for mic detection measurement (Ohms)
	 * [<200>, <500>, <750>, <1000>]
	 */
	unsigned mic_det_thr;

	/*
	 * jack-ins-deb : Debounce time for jack insertion (ms)
	 * [<5>, <10>, <20>, <50>, <100>, <200>, <500>, <1000>]
	 */
	unsigned jack_ins_deb;

	/*
	 * jack-det-rate : Jack type detection latency (3/4 pole)
	 * ["32ms_64ms", "64ms_128ms", "128ms_256ms", "256ms_512ms"]
	 */
	const char *jack_det_rate;

	/*
	 * jack-rem-deb : Debounce time for jack removal (ms)
	 * [<1>, <5>, <10>, <20>]
	 */
	unsigned jack_rem_deb;

	/*
	 * a-d-btn-thr : Impedance threshold between buttons A and D
	 * [0x0 - 0xFF]
	 */
	unsigned a_d_btn_thr;

	/*
	 * d-b-btn-thr : Impedance threshold between buttons D and B
	 * [0x0 - 0xFF]
	 */
	unsigned d_b_btn_thr;

	/*
	 * b-c-btn-thr : Impedance threshold between buttons B and C
	 * [0x0 - 0xFF]
	 */
	unsigned b_c_btn_thr;

	/*
	 * c-mic-btn-thr : Impedance threshold between button C and Mic
	 * [0x0 - 0xFF]
	 */
	unsigned c_mic_btn_thr;

	/*
	 * btn-avg : Number of 8-bit readings for averaged button measurement
	 * [<1>, <2>, <4>, <8>]
	 */
	unsigned btn_avg;

	/*
	 * adc-1bit-rpt : Repeat count for 1-bit button measurement
	 * [<1>, <2>, <4>, <8>]
	 */
	unsigned adc_1bit_rpt;

	/*
	 * mclk-name : Pass the system clk to da7219
	 * ["name of mclk" ]
	 */
	const char *mclk_name;
};
