#ifndef DEVICE_PATH_H
#define DEVICE_PATH_H

enum device_path_type {
	DEVICE_PATH_NONE = 0,
	DEVICE_PATH_ROOT,
	DEVICE_PATH_PCI,
	DEVICE_PATH_PNP,
	DEVICE_PATH_I2C,
	DEVICE_PATH_APIC,
	DEVICE_PATH_DOMAIN,
	DEVICE_PATH_CPU_CLUSTER,
	DEVICE_PATH_CPU,
	DEVICE_PATH_CPU_BUS,
	DEVICE_PATH_IOAPIC,
};

struct domain_path
{
	unsigned domain;
};

struct pci_path
{
	unsigned devfn;
};

struct pnp_path
{
	unsigned port;
	unsigned device;
};

struct i2c_path
{
	unsigned device;
};

struct apic_path
{
	unsigned apic_id;
	unsigned package_id;
	unsigned node_id;
	unsigned core_id;
	unsigned thread_id;
};

struct ioapic_path
{
	unsigned ioapic_id;
};

struct cpu_cluster_path
{
	unsigned cluster;
};

struct cpu_path
{
	unsigned id;
};

struct cpu_bus_path
{
	unsigned id;
};


struct device_path {
	enum device_path_type type;
	union {
		struct pci_path		pci;
		struct pnp_path		pnp;
		struct i2c_path		i2c;
		struct apic_path	apic;
		struct ioapic_path	ioapic;
		struct domain_path	domain;
		struct cpu_cluster_path cpu_cluster;
		struct cpu_path		cpu;
		struct cpu_bus_path	cpu_bus;
	};
};


#define DEVICE_PATH_MAX 30
#define BUS_PATH_MAX (DEVICE_PATH_MAX+10)

extern int path_eq(struct device_path *path1, struct device_path *path2);

#endif /* DEVICE_PATH_H */
