#include <cbmem.h>
#include <cpu/x86/smm.h>
#include <delay.h>
#include <rules.h>

void *cbmem_top(void)
{
	return NULL;
}

void southbridge_smi_set_eos(void)
{
}

#if ENV_BOOTBLOCK
void init_timer(void)
{
}
#endif
