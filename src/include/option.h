#ifndef _OPTION_H_
#define _OPTION_H_

#include <types.h>

enum cb_err set_option(const char *name, void *val);
enum cb_err get_option(void *dest, const char *name);


#if !IS_ENABLED(CONFIG_USE_OPTION_TABLE)

inline enum cb_err set_option(const char *name, void *val)
{
	return CB_CMOS_OTABLE_DISABLED;
}

inline enum cb_err get_option(void *dest, const char *name)
{
	return CB_CMOS_OTABLE_DISABLED;
}

#endif

#endif /* _OPTION_H_ */
