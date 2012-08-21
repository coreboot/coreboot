#ifndef I82801EX_CHIP_H
#define I82801EX_CHIP_H

struct southbridge_intel_i82801ex_config
{

#define ICH5R_GPIO_USE_MASK      0x03
#define ICH5R_GPIO_USE_DEFAULT   0x00
#define ICH5R_GPIO_USE_AS_NATIVE 0x01
#define ICH5R_GPIO_USE_AS_GPIO   0x02

#define ICH5R_GPIO_SEL_MASK      0x0c
#define ICH5R_GPIO_SEL_DEFAULT   0x00
#define ICH5R_GPIO_SEL_OUTPUT    0x04
#define ICH5R_GPIO_SEL_INPUT     0x08

#define ICH5R_GPIO_LVL_MASK      0x30
#define ICH5R_GPIO_LVL_DEFAULT   0x00
#define ICH5R_GPIO_LVL_LOW       0x10
#define ICH5R_GPIO_LVL_HIGH      0x20
#define ICH5R_GPIO_LVL_BLINK     0x30

#define ICH5R_GPIO_INV_MASK      0xc0
#define ICH5R_GPIO_INV_DEFAULT   0x00
#define ICH5R_GPIO_INV_OFF       0x40
#define ICH5R_GPIO_INV_ON        0x80

	/* GPIO use select */
	unsigned char gpio[64];
	unsigned int  pirq_a_d;
	unsigned int  pirq_e_h;
};

#endif /* I82801EX_CHIP_H */

