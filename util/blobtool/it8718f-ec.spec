# ITE IT8718F SuperIO EC registers
{
	# 00 Configuration register
	"conf00_start"		: 1,
	"conf00_smien"		: 1,
	"conf00_irqen"		: 1,
	"conf00_irqclr"		: 1,
	"conf00_ro_one"		: 1,
	"conf00_copen"		: 1,
	"conf00_vbat"		: 1,
	"conf00_initreset"	: 1,

	# 01 Interrupt Status register 1
	"irq1_maxfantac1"	: 1,
	"irq1_maxfantac2"	: 1,
	"irq1_maxfantac3"	: 1,
	"irq1_maxfantac4"	: 1,
	"irq1_copen"		: 1,
	"irq1_reserved0"	: 1,
	"irq1_maxfantac5"	: 1,
	"irq1_reserved1"	: 1,

	# 02 Interrupt Status register 2
	"irq2_limit_vin"[8]	: 1,

	# 03 Interrupt Status register 3
	"irq3_limit_temp1"	: 1,
	"irq3_limit_temp2"	: 1,
	"irq3_limit_temp3"	: 1,
	"irq3_reserved"		: 5,

	# 04 SMI Mask register 1
	"smi1_dis_fantac1"	: 1,
	"smi1_dis_fantac2"	: 1,
	"smi1_dis_fantac3"	: 1,
	"smi1_dis_fantac4"	: 1,
	"smi1_dis_copen"	: 1,
	"smi1_reserved0"	: 1,
	"smi1_dis_fantac5"	: 1,
	"smi1_reserved1"	: 1,

	# 05 SMI Mask register 2
	"smi2_dis_vin"[8]	: 1,

	# 06 SMI Mask register 3
	"smi3_dis_temp1"	: 1,
	"smi3_dis_temp2"	: 1,
	"smi3_dis_temp3"	: 1,
	"smi3_reserved"		: 5,

	# 07 Interrupt Mask register 1
	"irqmask1_fantac1"	: 1,
	"irqmask1_fantac2"	: 1,
	"irqmask1_fantac3"	: 1,
	"irqmask1_fantac4"	: 1,
	"irqmask1_copen"	: 1,
	"irqmask1_reserved0"	: 1,
	"irqmask1_fantac5"	: 1,
	"irqmask1_reserved1"	: 1,

	# 08 Interrupt Mask register 2
	"irqmask2_vin"[8]	: 1,

	# 09 Interrupt Mask register 3
	"irqmask3_temp1"	: 1,
	"irqmask3_temp2"	: 1,
	"irqmask3_temp3"	: 1,
	"irqmask3_reserved"	: 4,
	"irqmask3_extsensor"	: 1,

	# 0a Interface Selection register
	"iface_reserved"	: 4,
	"iface_extsensor_select": 3,
	"iface_pseudo_eoc"	: 1,

	# 0b Fan PWM smoothing step selection reg
	"fanpwm_reserved"	: 6,
	"fanpwm_smoothing_step"	: 2,

	# 0c Fan Tachometer 16 bit enable register
	"fantach16_en_tac1"	: 1,
	"fantach16_en_tac2"	: 1,
	"fantach16_en_tac3"	: 1,
	"fantach16_tmpin1_enh"	: 1,
	"fantach16_en_tac4"	: 1,
	"fantach16_en_tac5"	: 1,
	"fantach16_tmpin2_enh"	: 1,
	"fantach16_tmpin3_enh"	: 1,

	# 0d-0f Fan Tachmometer read registers
	"fantach_lo_counts1"	: 8,
	"fantach_lo_counts2"	: 8,
	"fantach_lo_counts3"	: 8,

	# 10-12 Fan Tachometer limit registers
	"fantach_lo_limit1"	: 8,
	"fantach_lo_limit2"	: 8,
	"fantach_lo_limit3"	: 8,

	# 13 Fan controller main control register
	"fanctlmain_mode1"	: 1,
	"fanctlmain_mode2"	: 1,
	"fanctlmain_mode3"	: 1,
	"fanctlmain_reserved0"	: 1,
	"fanctlmain_en_tac1"	: 1,
	"fanctlmain_en_tac2"	: 1,
	"fanctlmain_en_tac3"	: 1,
	"fanctlmain_reserved1"	: 1,

	# 14 FAN_CTL control register
	"fanctl_enable1"	: 1,
	"fanctl_enable2"	: 1,
	"fanctl_enable3"	: 1,
	"fanctl_minduty_sel"	: 1,
	# 000: 48Mhz (PWM Frequency 375Khz)
	# 001: 24Mhz (PWM Frequency 187.5Khz)
	# 010: 12Mhz (PWM Frequency 93.75Khz)
	# 011: 8Mhz (PWM Frequency 62.5Khz)
	# 100: 6Mhz (PWM Frequency 46.875Khz)
	# 101: 3Mhz (PWM Frequency 23.43Khz)
	# 110: 1.5Mhz (PWM Frequency 11.7Khz)
	# 111: 0.75Mhz (PWM Frequency 5.86Khz)
	"fanctl_pwm_base_clock"	: 3,
	"fanctl_allpolarity"	: 1,

	# 15 FAN_CTL1 PWM control register
	"fanctl1_tmpin_sel"	: 2,
	"fanctl1_steps"		: 5,
	"fanctl1_pwm_mode"	: 1,

	# 16 FAN_CTL2 PWM control register
	"fanctl2_tmpin_sel"	: 2,
	"fanctl2_steps"		: 5,
	"fanctl2_pwm_mode"	: 1,

	# 17 FAN_CTL3 PWM control register
	"fanctl3_tmpin_sel"	: 2,
	"fanctl3_steps"		: 5,
	"fanctl3_pwm_mode"	: 1,

	# 18-1a Fan Tachometer extended read registers
	"fantach_hi_counts1"	: 8,
	"fantach_hi_counts2"	: 8,
	"fantach_hi_counts3"	: 8,

	# 1b-1d Fan Tachometer extended limit registers
	"fantach_hi_limit1"	: 8,
	"fantach_hi_limit2"	: 8,
	"fantach_hi_limit3"	: 8,

	"reserved1e"		: 8,
	"reserved1f"		: 8,


	# 20-27 Reading registers
	"vin"[8]		: 8,

	"vbat"			: 8,
	"tmpin1"		: 8,
	"tmpin2"		: 8,
	"tmpin3"		: 8,
	"reserved2c"		: 8,
	"reserved2d"		: 8,
	"reserved2e"		: 8,
	"reserved2f"		: 8,
	"limit_hi_vin0"		: 8,
	"limit_lo_vin0"		: 8,
	"limit_hi_vin1"		: 8,
	"limit_lo_vin1"		: 8,
	"limit_hi_vin2"		: 8,
	"limit_lo_vin2"		: 8,
	"limit_hi_vin3"		: 8,
	"limit_lo_vin3"		: 8,
	"limit_hi_vin4"		: 8,
	"limit_lo_vin4"		: 8,
	"limit_hi_vin5"		: 8,
	"limit_lo_vin5"		: 8,
	"limit_hi_vin6"		: 8,
	"limit_lo_vin6"		: 8,
	"limit_hi_vin7"		: 8,
	"limit_lo_vin7"		: 8,
	"limit_hi_tmpin1"	: 8,
	"limit_lo_tmpin1"	: 8,
	"limit_hi_tmpin2"	: 8,
	"limit_lo_tmpin2"	: 8,
	"limit_hi_tmpin3"	: 8,
	"limit_lo_tmpin3"	: 8,

	"reserved46"		: 8,
	"reserved47"		: 8,
	"reserved48"		: 8,
	"reserved49"		: 8,
	"reserved4a"		: 8,
	"reserved4b"		: 8,
	"reserved4c"		: 8,
	"reserved4d"		: 8,
	"reserved4e"		: 8,
	"reserved4f"		: 8,

	# 50 ADC Voltage channel enable register
	"adc_scan_enable_vin"[8]: 1,

	# 51 ADC Temperature channel enable register
	"therm_diode_tmpin1"	: 1,
	"therm_diode_tmpin2"	: 1,
	"therm_diode_tmpin3"	: 1,
	# Mututally exclusive settings
	"therm_resistor_tmpin1"	: 1,
	"therm_resistor_tmpin2"	: 1,
	"therm_resistor_tmpin3"	: 1,
	"therm_reserved"	: 2,

	"therm_limit_tmpin1"	: 8,
	"therm_limit_tmpin2"	: 8,
	"therm_limit_tmpin3"	: 8,

	# 55 Temperature extra channel enable reg
	"therm_resistor_vin4"	: 1,
	"therm_resistor_vin5"	: 1,
	"therm_resistor_vin6"	: 1,
	"adc_fanctl2_pwm_duty"	: 1,
	# 000: 48Mhz (PWM Frequency 375Khz)
	# 001: 24Mhz (PWM Frequency 187.5Khz)
	# 010: 12Mhz (PWM Frequency 93.75Khz)
	# 011: 8Mhz (PWM Frequency 62.5Khz)
	# 100: 6Mhz (PWM Frequency 46.875Khz)
	# 101: 3Mhz (PWM Frequency 23.43Khz)
	# 110: 1.5Mhz (PWM Frequency 11.7Khz)
	# 111: 0.75Mhz (PWM Frequency 5.86Khz)
	"adc_fanctl2_pwm_bclk"	: 3,
	"adc_tmpin3_ext_select"	: 1,

	"thermal_zero_diode1"	: 8,
	"thermal_zero_diode2"	: 8,
	"ite_vendor_id"		: 8,
	"thermal_zero_diode3"	: 8,
	"reserved5a"		: 8,
	"ite_code_id"		: 8,

	"beep_fantac"		: 1,
	"beep_vin"		: 1,
	"beep_tmpin"		: 1,
	"beep_reserved"		: 1,
	# ADC clock select
	# 000: 500Khz (Default)
	# 001: 250Khz
	# 010: 125K
	# 011: 62.5Khz
	# 100: 31.25Khz
	# 101: 24Mhz
	# 110: 1Mhz
	# 111: 2Mhz
	"adc_clock_select"	: 3,
	"thermal_zero_adj_en"	: 1,

	"beep_fan_freq_div"	: 4,
	"beep_fan_tone_div"	: 4,
	"beep_volt_freq_div"	: 4,
	"beep_volt_tone_div"	: 4,
	"beep_temp_freq_div"	: 4,
	"beep_temp_tone_div"	: 4,

	# 60 SmartGuardian registers
	"sguard1_temp_lim_off"	: 8,
	"sguard1_temp_lim_fan"	: 8,
	"reserved62"		: 8,
	"sguard1_pwm_start"	: 7,
	"sguard1_pwm_slope6"	: 1,
	"sguard1_pwm_slope05"	: 6,
	"sguard1_pwm_reserved"	: 1,
	"sguard1_fan_smooth_en"	: 1,
	"sguard1_temp_interval"	: 5,
	"sguard1_temp_reserved"	: 2,
	"sguard1_temp_pwm_lin"	: 1,
	"reserved66"		: 8,
	"reserved67"		: 8,
	"sguard2_temp_lim_off"	: 8,
	"sguard2_temp_lim_fan"	: 8,
	"reserved6a"		: 8,
	"sguard2_pwm_start"	: 7,
	"sguard2_pwm_slope6"	: 1,
	"sguard2_pwm_slope05"	: 6,
	"sguard2_pwm_reserved"	: 1,
	"sguard2_fan_smooth_en"	: 1,
	"sguard2_temp_interval"	: 5,
	"sguard2_temp_reserved"	: 2,
	"sguard2_temp_pwm_lin"	: 1,
	"reserved6e"		: 8,
	"reserved6f"		: 8,
	"sguard3_temp_lim_off"	: 8,
	"sguard3_temp_lim_fan"	: 8,
	"reserved72"		: 8,
	"sguard3_pwm_start"	: 7,
	"sguard3_pwm_slope6"	: 1,
	"sguard3_pwm_slope05"	: 6,
	"sguard3_pwm_reserved"	: 1,
	"sguard3_fan_smooth_en"	: 1,
	"sguard3_temp_interval"	: 5,
	"sguard3_temp_reserved"	: 2,
	"sguard3_temp_pwm_lin"	: 1,
	"reserved76"		: 8,
	"reserved77"		: 8,
	"reserved78"		: 8,
	"reserved79"		: 8,
	"reserved7a"		: 8,
	"reserved7b"		: 8,
	"reserved7c"		: 8,
	"reserved7d"		: 8,
	"reserved7e"		: 8,
	"reserved7f"		: 8,

	# 80 Fan Tachometer 4-5 read registers
	"fantach_lo_counts4"	: 8,
	"fantach_hi_counts4"	: 8,
	"fantach_lo_counts5"	: 8,
	"fantach_hi_counts5"	: 8,
	"fantach_lo_limit4"	: 8,
	"fantach_hi_limit4"	: 8,
	"fantach_lo_limit5"	: 8,
	"fantach_hi_limit5"	: 8,

	# 88 External temperature sensor host status
	"ext_host_busy"		: 1,
	"ext_host_fnsh"		: 1,
	"ext_host_r_fcs_error"	: 1,
	"ext_host_w_fcs_error"	: 1,
	"ext_host_peci_highz"	: 1,
	"ext_host_sst_slave"	: 1,
	"ext_host_sst_bus"	: 1,
	"ext_host_data_fifo_clr": 1,

	"ext_host_target_addr"	: 8,
	"ext_host_write_length"	: 8,
	"ext_host_read_length"	: 8,
	"ext_host_cmd"		: 8,
	"ext_host_writedata"	: 8,

	"ext_hostctl_start"	: 1,
	"ext_hostctl_sst_amdsi"	: 1,
	"ext_hostctl_sst_ctl"	: 1,
	"ext_hostctl_resetfifo"	: 1,
	"ext_hostctl_fcs_abort"	: 1,
	"ext_hostctl_start_en"	: 1,
	# Auto-Start Control
	# The host will start the transaction
	# at a regular rate automatically.
	# 00: 32 Hz
	# 01: 16 Hz
	# 10: 8 Hz
	# 11: 4 Hz
	"ext_hostctl_start_ctl"	: 2,

	"ext_host_readdata"	: 8,

	"fan1_temp_limit_start"	: 8,
	"fan1_slope_pwm"	: 7,
	"fan1_temp_input_sel0"	: 1,
	"fan1_ctlmode_temp_ivl"	: 5,
	"fan1_ctlmode_target"	: 2,
	"fan1_temp_input_sel1"	: 1,
	"reserved93"		: 8,
	"fan2_temp_limit_start"	: 8,
	"fan2_slope_pwm"	: 7,
	"fan2_temp_input_sel0"	: 1,
	"fan2_ctlmode_temp_ivl"	: 5,
	"fan2_ctlmode_target"	: 2,
	"fan2_temp_input_sel1"	: 1
}
