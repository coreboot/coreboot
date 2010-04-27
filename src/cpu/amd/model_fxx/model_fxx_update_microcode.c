/* Copyright 2005 AMD
 * 2005.08 yhlu add microcode support
 */
/*============================================================================
Copyright 2005 ADVANCED MICRO DEVICES, INC. All Rights Reserved.
This software and any related documentation (the "Materials") are the
confidential proprietary information of AMD. Unless otherwise provided in a
software agreement specifically licensing the Materials, the Materials are
provided in confidence and may not be distributed, modified, or reproduced in
whole or in part by any means.
LIMITATION OF LIABILITY: THE MATERIALS ARE PROVIDED "AS IS" WITHOUT ANY
EXPRESS OR IMPLIED WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO
WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY
PARTICULAR PURPOSE, OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR
USAGE OF TRADE. IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY
DAMAGES WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS,
BUSINESS INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF THE USE OF OR
INABILITY TO USE THE MATERIALS, EVEN IF AMD HAS BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES. BECAUSE SOME JURISDICTIONS PROHIBIT THE EXCLUSION
OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES, THE ABOVE
LIMITATION MAY NOT APPLY TO YOU.
AMD does not assume any responsibility for any errors which may appear in the
Materials nor any responsibility to support or update the Materials. AMD
retains the right to modify the Materials at any time, without notice, and is
not obligated to provide such modified Materials to you.
NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
further information, software, technical information, know-how, or show-how
available to you.
U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with "RESTRICTED
RIGHTS." Use, duplication, or disclosure by the Government is subject to the
restrictions as set forth in FAR 52.227-14 and DFAR 252.227-7013, et seq., or
its successor. Use of the Materials by the Government constitutes
acknowledgement of AMD's proprietary rights in them.
============================================================================*/
//@DOC
// in model_fxx_update_microcode.c
/*
$1.0$
*/
// Description: microcode patch support for k8
// by yhlu
//
//============================================================================


#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>

#include <cpu/amd/microcode.h>

static uint8_t microcode_updates[] __attribute__ ((aligned(16))) = {

#if CONFIG_K8_REV_F_SUPPORT == 0
	#include "microcode_rev_c.h"
	#include "microcode_rev_d.h"
	#include "microcode_rev_e.h"
#endif

#if CONFIG_K8_REV_F_SUPPORT == 1
//	#include "microcode_rev_f.h"
#endif
        /*  Dummy terminator  */
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
};

static unsigned get_equivalent_processor_rev_id(unsigned orig_id) {
	static unsigned id_mapping_table[] = {
	#if CONFIG_K8_REV_F_SUPPORT == 0
	        0x0f48, 0x0048,
	        0x0f58, 0x0048,

	        0x0f4a, 0x004a,
	        0x0f5a, 0x004a,
	        0x0f7a, 0x004a,
	        0x0f82, 0x004a,
	        0x0fc0, 0x004a,
	        0x0ff0, 0x004a,

	        0x10f50, 0x0150,
	        0x10f70, 0x0150,
	        0x10f80, 0x0150,
	        0x10fc0, 0x0150,
	        0x10ff0, 0x0150,

	        0x20f10, 0x0210,
	        0x20f12, 0x0210,
	        0x20f32, 0x0210,
	        0x20fb1, 0x0210,
	#endif

	#if CONFIG_K8_REV_F_SUPPORT == 1

	#endif

	};


	unsigned new_id;
	int i;

	new_id = 0;

	for(i=0; i<sizeof(id_mapping_table); i+=2 ) {
		if(id_mapping_table[i]==orig_id) {
			new_id = id_mapping_table[i+1];
			break;
		}
	}

	return new_id;

}

void model_fxx_update_microcode(unsigned cpu_deviceid)
{
	unsigned equivalent_processor_rev_id;

        /* Update the microcode */
	equivalent_processor_rev_id = get_equivalent_processor_rev_id(cpu_deviceid );
	if(equivalent_processor_rev_id != 0)
	        amd_update_microcode(microcode_updates, equivalent_processor_rev_id);

}

