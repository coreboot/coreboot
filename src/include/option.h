#ifndef _OPTION_H_
#define _OPTION_H_

/*
 * FIXME: get_option() needs to be abstracted better so that other non-volatile
 * storage can be used. This will benefit machines without CMOS as well as those
 * without a battery-backed CMOS (e.g. some laptops).
 */

#if CONFIG_USE_OPTION_TABLE_CMOS

#include <pc80/mc146818rtc.h>

#elif CONFIG_USE_OPTION_TABLE_CBFS

#include <commonlib/option_cbfs.h>

#else

#endif

#endif /* _OPTION_H_ */
