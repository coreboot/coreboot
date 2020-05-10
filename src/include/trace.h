/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __TRACE_H
#define __TRACE_H

#if !ENV_ROMSTAGE_OR_BEFORE && CONFIG(TRACE)

void __cyg_profile_func_enter(void *, void *)
				 __attribute__((no_instrument_function));

void __cyg_profile_func_exit(void *, void *)
				__attribute__((no_instrument_function));

extern volatile int trace_dis;

#define DISABLE_TRACE  do { trace_dis = 1; } while (0);
#define ENABLE_TRACE    do { trace_dis = 0; } while (0);
#define DISABLE_TRACE_ON_FUNCTION  __attribute__((no_instrument_function));

#else /* !CONFIG_TRACE */

#define DISABLE_TRACE
#define ENABLE_TRACE
#define DISABLE_TRACE_ON_FUNCTION

#endif

#endif
