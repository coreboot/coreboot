#ifndef EC_LENOVO_PMH7_CHIP_H
#define EC_LENOVO_PMH7_CHIP_H

extern struct chip_operations ec_lenovo_pmh7_ops;

struct ec_lenovo_pmh7_config {
	int backlight_enable:1;
};
#endif
