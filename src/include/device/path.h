#ifndef DEVICE_PATH_H
#define DEVICE_PATH_H

enum device_path_type {
	DEVICE_PATH_NONE = 0,
	DEVICE_PATH_PCI,
	DEVICE_PATH_PNP,
	DEVICE_PATH_I2C,
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

struct device_path {
	enum device_path_type type;
	union {
		struct pci_path pci;
		struct pnp_path pnp;
		struct i2c_path i2c;
	} u;
};


#define DEVICE_PATH_MAX 30

extern int path_eq(struct device_path *path1, struct device_path *path2);

#endif /* DEVICE_PATH_H */
