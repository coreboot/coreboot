#include <cbmem.h>
#include <cpu/x86/smm.h>

void *cbmem_top(void)
{
	return NULL;
}

void southbridge_smi_set_eos(void)
{
}
