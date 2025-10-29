/* SPDX-License-Identifier: GPL-2.0-only */

#include <drivers/option/cfr_frontend.h>
#include <common/cfr.h>

void __weak cfr_card_reader_update(const struct sm_object *obj,
		struct sm_object *new_obj)
{
	(void)obj;
	(void)new_obj;
}
