#include "../../../southbridge/amd/amd8111/amd8111_reset.c"

void hard_reset(void)
{
	amd8111_hard_reset(0, 1);
}
