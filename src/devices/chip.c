/* chips are arbitrary chips (superio, southbridge, etc.)
 * They have private structures that define chip resources and default 
 * settings. They have four externally visible functions for control. 
 * They have a generic component which applies to all chips for 
 * path, etc. 
 */

#include <device/chip.h>

void
chip_configure(struct chip *root, enum chip_pass pass)
{
	struct chip *c;

	for (c = root; c; c = c->next) {
		if (c->control && c->control->enable)
			c->control->enable(c, pass);
	}

	for (c = root; c; c = c->next) {
		if (c->children)
			chip_configure(c->children, pass);
	}
}
