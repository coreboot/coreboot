/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __PICASSO_SOC_UTIL_H__
#define __PICASSO_SOC_UTIL_H__

#include <types.h>

enum socket_type {
	SOCKET_FP5 = 0,
	SOCKET_AM4 = 2,
	SOCKET_FT5 = 3,
};

enum silicon_type {
	SILICON_RV1,
	SILICON_PCO,
	SILICON_RV2,
	SILICON_UNKNOWN,
};

enum soc_type {
	SOC_PICASSO,
	SOC_DALI,
	SOC_POLLOCK,
	SOC_UNKNOWN,
};

enum socket_type get_socket_type(void);
enum silicon_type get_silicon_type(void);
enum soc_type get_soc_type(void);

void print_socket_type(void);
void print_silicon_type(void);
void print_soc_type(void);

/* function to determine the connectivity feature set */
bool soc_is_dali(void);

/* function to determine the iGPU type */
bool soc_is_raven2(void);

#endif /* __PICASSO_SOC_UTIL_H__ */
