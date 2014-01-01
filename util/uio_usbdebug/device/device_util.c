
#include <device/device.h>

static char g_path[] = { '\0' };

const char *dev_path(device_t dev)
{
	return g_path;
}

struct resource *find_resource(device_t dev, unsigned index)
{
	return NULL;
}

void report_resource_stored(device_t dev, struct resource *resource,
			    const char *comment)
{
}

struct device *dev_find_slot(unsigned int bus, unsigned int devfn)
{
	return NULL;
}
