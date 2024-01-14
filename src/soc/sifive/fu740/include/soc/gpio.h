#ifndef _SOC_SIFIVE_FU740_GPIO_H_
#define _SOC_SIFIVE_FU740_GPIO_H_

#include <stdint.h>

//TODO these are mainboard specific
enum gpio {
	J29_1,
	PMICINTB,
	PMICSHDN,
	J8_1,
	J8_3,
	PCIE_PWREN, // connected to power rails of PCIe connectors
	THERM,
	UBRDG_RSTN,
	PCIE_PERSTN, // connected to PERST pin of PCIe connectors
	ULPI_RSTN,
	J8_2,
	UHUB_RSTN,
	GEMGXL_RST,
	J8_4,
	EN_VDD_SD,
	SD_CD,
};

// this is to satisfy src/include/gpio.h
typedef enum gpio gpio_t;

enum gpio_direction {
	GPIO_INPUT,
	GPIO_OUTPUT,
};

void gpio_set_direction(gpio_t gpio, enum gpio_direction gpio_dir);

#endif // _SOC_SIFIVE_FU740_GPIO_H_
