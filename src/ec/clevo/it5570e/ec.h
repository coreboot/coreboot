/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef EC_CLEVO_IT5570E_EC_H
#define EC_CLEVO_IT5570E_EC_H

#include <device/device.h>

/* PNP devices */
#define IT5570E_UART1	0x01 /* UART1 */
#define IT5570E_UART2	0x02 /* UART2 */
#define IT5570E_SWUC	0x04 /* System Wake-Up Control */
#define IT5570E_KBCM	0x05 /* PS/2 KBC Mouse */
#define IT5570E_KBCK	0x06 /* PS/2 KBC Keyboard */
#define IT5570E_CIR	0x0a /* Consumer IR */
#define IT5570E_SMFI	0x0f /* Shared Memory/Flash Interface */
#define IT5570E_RTCT	0x10 /* RTC-like Timer */
#define IT5570E_PM1	0x11 /* Power Management Channel 1 */
#define IT5570E_PM2	0x12 /* Power Management Channel 2 */
#define IT5570E_SSPI	0x13 /* Serial Peripheral Interface */
#define IT5570E_PECI	0x14 /* Platform Environment Control Interface */
#define IT5570E_PM3	0x17 /* Power Management Channel 3 */
#define IT5570E_PM4	0x18 /* Power Management Channel 4 */
#define IT5570E_PM5	0x19 /* Power Management Channel 5 */

/* SMFI registers */
#define HLPCRAMBA_15_12	0xf5
#define HLPCRAMBA_23_16	0xf6
#define HLPCRAMBA_24	0xfc

void ec_fan_curve_fill_ssdt(const struct device *dev);

#endif /* EC_CLEVO_IT5570E_EC_H */
