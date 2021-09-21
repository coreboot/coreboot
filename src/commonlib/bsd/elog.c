/* SPDX-License-Identifier: BSD-3-Clause */

#include <commonlib/bsd/bcd.h>
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
	/*
	 * Pointing to the next event returns the data, since data is the first
	 * field right after the header.
	 */
	return (const void *)(&event[1]);
}

/* Populate timestamp in event header with given time. */
void elog_fill_timestamp(struct event_header *event, uint8_t sec, uint8_t min,
			 uint8_t hour, uint8_t mday, uint8_t mon, uint16_t year)
{
	event->second = bin2bcd(sec);
	event->minute = bin2bcd(min);
	event->hour = bin2bcd(hour);
	event->day = bin2bcd(mday);
	event->month = bin2bcd(mon);
	event->year = bin2bcd(year % 100);

	/* Basic check of expected ranges. */
	if (event->month > 0x12 || event->day > 0x31 || event->hour > 0x23 ||
	    event->minute > 0x59 || event->second > 0x59) {
		event->year   = 0;
		event->month  = 0;
		event->day    = 0;
		event->hour   = 0;
		event->minute = 0;
		event->second = 0;
	}
}

void elog_update_checksum(struct event_header *event, uint8_t checksum)
{
	uint8_t *event_data = (uint8_t *)event;
	event_data[event->length - 1] = checksum;
}

uint8_t elog_checksum_event(const struct event_header *event)
{
	uint8_t index, checksum = 0;
	const uint8_t *data = (const uint8_t *)event;

	for (index = 0; index < event->length; index++)
		checksum += data[index];
	return checksum;
}
