#include <mem.h>

struct mem_range *
sizeram(void)
{
	static struct mem_range meminfo;

	meminfo.basek = 0;
	meminfo.sizek = 1024 * 1024; /* FIXME */

	return &meminfo;
}
