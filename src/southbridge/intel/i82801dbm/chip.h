#ifndef I82801DBM_CHIP_H
#define I82801DBM_CHIP_H

struct southbridge_intel_i82801dbm_config 
{
	int enable_usb;
	int enable_native_ide;
};
struct chip_operations;
extern struct chip_operations southbridge_intel_i82801dbm_ops;

#endif /* I82801DBM_CHIP_H */
