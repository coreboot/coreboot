#ifndef DEVICE_PATH_H
#define DEVICE_PATH_H

enum device_path_type {
	DEVICE_PATH_NONE = 0,
	DEVICE_PATH_ROOT,
	DEVICE_PATH_DEFAULT_CPU,
	DEVICE_PATH_PCI,
	DEVICE_PATH_PNP,
	DEVICE_PATH_I2C,
	DEVICE_PATH_APIC,
	DEVICE_PATH_PCI_DOMAIN,
	DEVICE_APIC_CLUSTER,
};

struct pci_domain_path
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
};

struct apic_cluster_path
{
	unsigned cluster;
};


struct device_path {
	enum device_path_type type;
	union {
		struct pci_path   pci;
		struct pnp_path   pnp;
		struct i2c_path   i2c;
		struct apic_path  apic;
		struct pci_domain_path pci_domain;
		struct apic_cluster_path apic_cluster;
	} u;
};


#define DEVICE_PATH_MAX 30

extern int path_eq(struct device_path *path1, struct device_path *path2);

#endif /* DEVICE_PATH_H */
