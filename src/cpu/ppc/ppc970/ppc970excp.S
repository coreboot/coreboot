
#include <ppc970.h>

/*----------------------------------------------------------------------------+
| Init_excp.  The external interrupt vector should never be called before
| io_init() is called so it can be removed from this file.
+----------------------------------------------------------------------------*/
        function_prolog(init_excp)
        .space	0x100
        b     	.init_core		    /* 0100                          */
	function_epilog(init_excp)
