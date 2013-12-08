unsigned microcode[] = {
#if !CONFIG_K8_REV_F_SUPPORT
	#include "microcode_rev_c.h"
	#include "microcode_rev_d.h"
	#include "microcode_rev_e.h"
#endif

#if CONFIG_K8_REV_F_SUPPORT
//	#include "microcode_rev_f.h"
#endif

	/*  Dummy terminator  */
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
};
