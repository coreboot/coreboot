unsigned char microcode[] __attribute__ ((aligned(16))) = {
#if !CONFIG_K8_REV_F_SUPPORT
	#include "microcode_rev_c.h"
	#include "microcode_rev_d.h"
	#include "microcode_rev_e.h"
#endif
};
