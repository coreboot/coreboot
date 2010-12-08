#include "southbridge/amd/amd8111/enable_rom.c"

static void bootblock_southbridge_init(void)
{
	amd8111_enable_rom();
}
