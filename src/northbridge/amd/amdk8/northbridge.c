#include <arch/io.h>
#include <stdint.h>
#include <mem.h>
#include <part/sizeram.h>

struct mem_range *sizeram(void)
{
	static struct mem_range mem[3];
	uint32_t size;
	/* Convert size in bytes to size in K */
#warning "FINISH sizeram"
	/* FIXME  hardcoded for now */
	size = 512*1024;

	mem[0].basek = 0;
	mem[0].sizek = 640;
	mem[1].basek = 960;
	mem[1].sizek = size - mem[1].basek;
	mem[2].basek = 0;
	mem[2].sizek = 0;
	return mem;
}

