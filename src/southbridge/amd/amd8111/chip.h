#ifndef AMD8111_CHIP_H
#define AMD8111_CHIP_H

struct southbridge_amd_amd8111_config
{
	unsigned int ide0_enable : 1;
	unsigned int ide1_enable : 1;
	unsigned int phy_lowreset : 1;
};

#endif /* AMD8111_CHIP_H */
