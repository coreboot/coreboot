#include <assert.h>
#include "cmos_lowlevel.h"

static void mem_hal_init(void* data);
static unsigned char mem_hal_read(unsigned addr);
static void mem_hal_write(unsigned addr, unsigned char value);
static void mem_set_iopl(int level);

static unsigned char* mem_hal_data = (unsigned char*)-1;
static void mem_hal_init(void *data)
{
	mem_hal_data = data;
}

static unsigned char mem_hal_read(unsigned index)
{
	assert(mem_hal_data != (unsigned char*)-1);
	return mem_hal_data[index];
}

static void mem_hal_write(unsigned index, unsigned char value)
{
	assert(mem_hal_data != (unsigned char*)-1);
	mem_hal_data[index] = value;
}

static void mem_set_iopl(__attribute__ ((unused)) int level)
{
}

cmos_access_t memory_hal = {
	.init = mem_hal_init,
	.read = mem_hal_read,
	.write = mem_hal_write,
	.set_iopl = mem_set_iopl,
};

