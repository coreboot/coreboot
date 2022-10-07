/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <stddef.h>

struct pick_largest_state {
	struct resource *last;
	const struct device *result_dev;
	struct resource *result;
	int seen_last;
};

static void pick_largest_resource(void *gp, struct device *dev,
				  struct resource *resource)
{
	struct pick_largest_state *state = gp;
	struct resource *last;

	last = state->last;

	/* Be certain to pick the successor to last. */
	if (resource == last) {
		state->seen_last = 1;
		return;
	}
	if (resource->flags & IORESOURCE_FIXED)
		return;	/* Skip it. */
	if (last && ((last->align < resource->align) ||
		     ((last->align == resource->align) &&
		      (last->size < resource->size)) ||
		     ((last->align == resource->align) &&
		      (last->size == resource->size) && (!state->seen_last)))) {
		return;
	}
	if (!state->result ||
	    (state->result->align < resource->align) ||
	    ((state->result->align == resource->align) &&
	     (state->result->size < resource->size))) {
		state->result_dev = dev;
		state->result = resource;
	}
}

const struct device *largest_resource(struct bus *bus,
				      struct resource **result_res,
				      unsigned long type_mask,
				      unsigned long type)
{
	struct pick_largest_state state;

	state.last = *result_res;
	state.result_dev = NULL;
	state.result = NULL;
	state.seen_last = 0;

	search_bus_resources(bus, type_mask, type, pick_largest_resource,
			     &state);

	*result_res = state.result;
	return state.result_dev;
}
