#ifndef I8371EB_CHIP_H
#define I8371EB_CHIP_H

struct southbridge_intel_i8371eb_config 
{
	unsigned int ide0_enable : 1;
	unsigned int ide1_enable : 1;
};

struct chip_operations;
extern struct chip_operations southbridge_intel_i8371eb_ops;

#endif /* I8371EB_CHIP_H */
