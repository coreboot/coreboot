#ifndef _OPTION_H_
#define _OPTION_H_

/*
 * FIXME: get_option() needs to be abstracted better so that other non-volatile
 * storage can be used. This will benefit machines without CMOS as well as those
 * without a battery-backed CMOS (e.g. some laptops).
 */
#if CONFIG_USE_OPTION_TABLE
#include <pc80/mc146818rtc.h>
#else
#include <types.h>
static inline enum cb_err get_option(void *dest, const char *name)
{
	return CB_CMOS_OTABLE_DISABLED;
}
#endif

#endif /* _OPTION_H_ */
