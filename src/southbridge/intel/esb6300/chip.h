struct southbridge_intel_esb6300_config
{
#define ESB6300_GPIO_USE_MASK      0x03
#define ESB6300_GPIO_USE_DEFAULT   0x00
#define ESB6300_GPIO_USE_AS_NATIVE 0x01
#define ESB6300_GPIO_USE_AS_GPIO   0x02

#define ESB6300_GPIO_SEL_MASK      0x0c
#define ESB6300_GPIO_SEL_DEFAULT   0x00
#define ESB6300_GPIO_SEL_OUTPUT    0x04
#define ESB6300_GPIO_SEL_INPUT     0x08

#define ESB6300_GPIO_LVL_MASK      0x30
#define ESB6300_GPIO_LVL_DEFAULT   0x00
#define ESB6300_GPIO_LVL_LOW       0x10
#define ESB6300_GPIO_LVL_HIGH      0x20
#define ESB6300_GPIO_LVL_BLINK     0x30

#define ESB6300_GPIO_INV_MASK      0xc0
#define ESB6300_GPIO_INV_DEFAULT   0x00
#define ESB6300_GPIO_INV_OFF       0x40
#define ESB6300_GPIO_INV_ON        0x80

	/* GPIO use select */
	unsigned char gpio[64];
	unsigned int  pirq_a_d;
	unsigned int  pirq_e_h;
};

