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
// microcode.c
/*
$1.0$
*/
// Description: microcode patch support for k8
// by yhlu
//
//============================================================================
#include <stdint.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/microcode.h>
#include <cpu/x86/cache.h>

struct microcode {
	uint32_t date_code;
	uint32_t patch_id;

	uint16_t m_patch_data_id;
	uint8_t m_patch_data_len;
	uint8_t init_flag;
	
	uint32_t m_patch_data_cksum;
	
	uint32_t nb_dev_id;
	uint32_t ht_io_hub_dev_id;

	uint16_t processor_rev_id;
	uint8_t ht_io_hub_rev_id;
	uint8_t nb_rev_id;

	uint8_t bios_api_rev;
	uint8_t resv1[3];

	uint32_t match_reg[8];

	uint8_t m_patch_data[896];
	uint8_t resv2[896];
	
	uint8_t x86_code_present;
	uint8_t x86_code_entry[191];
};

static int need_apply_patch(struct microcode *m, unsigned equivalent_processor_rev_id)
{

        if (m->processor_rev_id != equivalent_processor_rev_id) return 0;

        if (m->nb_dev_id) {
                 //look at the device id, if not found return;
                 //if(m->nb_rev_id != installed_nb_rev_id) return 0;
        }

        if (m->ht_io_hub_dev_id) {
                 //look at the device id, if not found return;
                 //if(m->ht_io_hub_rev_id != installed_ht_io_bub_rev_id) return 0;
        }

        if (m->x86_code_present) {
                 //if(!x86_code_execute()) return 0;
        }

	return 1;
}


void amd_update_microcode(void *microcode_updates, unsigned equivalent_processor_rev_id)
{
	unsigned int patch_id, new_patch_id;
	struct microcode *m;
	char *c;
	msr_t msr;
	
	msr = rdmsr(0x8b);
	patch_id = msr.lo;

	printk_debug("microcode: equivalent processor rev id  = 0x%04x, patch id = 0x%08x\n", equivalent_processor_rev_id, patch_id);

	m = microcode_updates;

	for(c = microcode_updates; m->date_code;  m = (struct microcode *)c) {

		if( need_apply_patch(m, equivalent_processor_rev_id) ) {
			//apply patch

			msr.hi = 0;
			msr.lo = (uint32_t)m;

			wrmsr(0xc0010020, msr);

			printk_debug("microcode: patch id that want to apply= 0x%08x\n", m->patch_id);

                        //read the patch_id again
                        msr = rdmsr(0x8b);
                        new_patch_id = msr.lo;

			printk_debug("microcode: updated to patch id = 0x%08x %s\r\n", new_patch_id , (new_patch_id == m->patch_id)?" success":" fail" );
			break;
		}
		c += 2048;
	}

}
