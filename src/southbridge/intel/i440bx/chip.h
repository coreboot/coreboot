#ifndef I440BX_CHIP_H
#define I440BX_CHIP_H

struct southbridge_intel_i440bx_config 
{
	unsigned int ide0_enable : 1;
	unsigned int ide1_enable : 1;
};

struct chip_operations;
extern struct chip_operations southbridge_intel_i440bx_ops;

#endif /* I440BX_CHIP_H */
