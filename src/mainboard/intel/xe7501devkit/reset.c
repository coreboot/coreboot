#include <reset.h>

void i82801cx_hard_reset(void);

void hard_reset(void)
{
	i82801cx_hard_reset();
}
