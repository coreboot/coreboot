#include <memlayout.h>
#include <arch/header.ld>

SECTIONS
{
	RAMSTAGE(CONFIG_RAMBASE, CONFIG_RAMTOP - CONFIG_RAMBASE)
}
