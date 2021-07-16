/* SPDX-License-Identifier: BSD-3-Clause */

#include <commonlib/bsd/elog.h>
#include <stddef.h>

/*
 * verify and validate if header is a valid coreboot Event Log header.
 * return CB_ERR if invalid, otherwise CB_SUCCESS.
 */
enum cb_err elog_verify_header(const struct elog_header *header)
{
	if (header == NULL)
		return CB_ERR;

	if (header->magic != ELOG_SIGNATURE)
		return CB_ERR;

	if (header->version != ELOG_VERSION)
		return CB_ERR;

	if (header->header_size != sizeof(*header))
		return CB_ERR;

	return CB_SUCCESS;
}

/*
 * return the next elog event.
 * return NULL if event is invalid.
 */
const struct event_header *elog_get_next_event(const struct event_header *event)
{
	if (!event)
		return NULL;

	/* Point to next event */
	return (const struct event_header *)((const void *)(event) + event->length);
}

/* return the data associated to the event_header. */
const void *event_get_data(const struct event_header *event)
{
	// Pointing to the next event returns the data, since data is the first field
	// right after the header.
	return (const void *)(&event[1]);
}
