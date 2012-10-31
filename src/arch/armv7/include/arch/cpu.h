#ifndef ARCH_CPU_H
#define ARCH_CPU_H

#if !defined(__PRE_RAM__)
#include <device/device.h>

struct cpu_driver {
	struct device_operations *ops;
	struct cpu_device_id *id_table;
};

struct cpu_info {
	device_t cpu;
	unsigned long index;
};

struct cpuinfo_arm {
        uint8_t    arm;            /* CPU family */
        uint8_t    arm_vendor;     /* CPU vendor */
        uint8_t    arm_model;
};

#endif

#endif /* ARCH_CPU_H */
