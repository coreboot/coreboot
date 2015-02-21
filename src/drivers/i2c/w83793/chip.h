struct drivers_i2c_w83793_config {
	u8 mfc;
	u8 fanin;
	u8 fanin_sel;
	u8 peci_agent_conf;
	u8 tcase0;			/* °C */
	u8 tcase1;			/* °C */
	u8 tcase2;			/* °C */
	u8 tcase3;			/* °C */
	u8 tr_enable;
	u8 td_mode_select;
	u8 critical_temperature;	/* °C */

	u32 vcorea_high_limit_mv;	/* mV */
	u32 vcorea_low_limit_mv;	/* mV */
	u32 vcoreb_high_limit_mv;	/* mV */
	u32 vcoreb_low_limit_mv;	/* mV */
	u32 vtt_high_limit_mv;		/* mV */
	u32 vtt_low_limit_mv;		/* mV */
	u32 vsen1_high_limit_mv;	/* mV */
	u32 vsen1_low_limit_mv;		/* mV */
	u32 vsen2_high_limit_mv;	/* mV */
	u32 vsen2_low_limit_mv;		/* mV */
	u32 vsen3_high_limit_mv;	/* mV */
	u32 vsen3_low_limit_mv;		/* mV */
	u32 vsen4_high_limit_mv;	/* mV */
	u32 vsen4_low_limit_mv;		/* mV */
	u32 vdd_high_limit_mv;		/* mV */
	u32 vdd_low_limit_mv;		/* mV */
	u32 vsb_high_limit_mv;		/* mV */
	u32 vsb_low_limit_mv;		/* mV */
	u32 vbat_high_limit_mv;		/* mV */
	u32 vbat_low_limit_mv;		/* mV */

	u8 td1_critical_temperature;	/* °C */
	u8 td1_critical_hysteresis;	/* °C */
	u8 td1_warning_temperature;	/* °C */
	u8 td1_warning_hysteresis;	/* °C */
	u8 td2_critical_temperature;	/* °C */
	u8 td2_critical_hysteresis;	/* °C */
	u8 td2_warning_temperature;	/* °C */
	u8 td2_warning_hysteresis;	/* °C */
	u8 td3_critical_temperature;	/* °C */
	u8 td3_critical_hysteresis;	/* °C */
	u8 td3_warning_temperature;	/* °C */
	u8 td3_warning_hysteresis;	/* °C */
	u8 td4_critical_temperature;	/* °C */
	u8 td4_critical_hysteresis;	/* °C */
	u8 td4_warning_temperature;	/* °C */
	u8 td4_warning_hysteresis;	/* °C */
	u8 tr1_critical_temperature;	/* °C */
	u8 tr1_critical_hysteresis;	/* °C */
	u8 tr1_warning_temperature;	/* °C */
	u8 tr1_warning_hysteresis;	/* °C */
	u8 tr2_critical_temperature;	/* °C */
	u8 tr2_critical_hysteresis;	/* °C */
	u8 tr2_warning_temperature;	/* °C */
	u8 tr2_warning_hysteresis;	/* °C */

	u8 fanctrl1;
	u8 fanctrl2;
	u8 first_valid_fan_number;

	u8 td1_fan_select;
	u8 td2_fan_select;
	u8 td3_fan_select;
	u8 td4_fan_select;

	u8 tr1_fan_select;
	u8 tr2_fan_select;

	u8 fan1_nonstop;		/* % of full speed (0-100) */
	u8 fan2_nonstop;		/* % of full speed (0-100) */
	u8 fan3_nonstop;		/* % of full speed (0-100) */
	u8 fan4_nonstop;		/* % of full speed (0-100) */
	u8 fan5_nonstop;		/* % of full speed (0-100) */
	u8 fan6_nonstop;		/* % of full speed (0-100) */
	u8 fan7_nonstop;		/* % of full speed (0-100) */
	u8 fan8_nonstop;		/* % of full speed (0-100) */

	u8 default_speed;		/* % of full speed (0-100) */

	u8 fan1_duty;			/* % of full speed (0-100) */
	u8 fan2_duty;			/* % of full speed (0-100) */
	u8 fan3_duty;			/* % of full speed (0-100) */
	u8 fan4_duty;			/* % of full speed (0-100) */
	u8 fan5_duty;			/* % of full speed (0-100) */
	u8 fan6_duty;			/* % of full speed (0-100) */
	u8 fan7_duty;			/* % of full speed (0-100) */
	u8 fan8_duty;			/* % of full speed (0-100) */
};
