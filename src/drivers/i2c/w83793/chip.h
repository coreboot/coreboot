struct drivers_i2c_w83793_config {
	u8 mfc;
	u8 fanin;
	u8 peci_agent_conf;
	u8 tcase0;
	u8 tcase1;
	u8 tcase2;
	u8 tcase3;
	u8 tr_enable;
	u8 critical_temperature;

	u8 td1_fan_select;
	u8 td2_fan_select;
	u8 td3_fan_select;
	u8 td4_fan_select;

	u8 tr1_fan_select;
	u8 tr2_fan_select;
};
