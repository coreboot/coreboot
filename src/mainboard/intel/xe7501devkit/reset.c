#include <reset.h>

#include "southbridge/intel/i82801cx/i82801cx.h"

void hard_reset(void)
{
	i82801cx_hard_reset();
}
