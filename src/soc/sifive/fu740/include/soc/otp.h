/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_SIFIVE_FU740_OTP_H__
#define __SOC_SIFIVE_FU740_OTP_H__

#include <stdint.h>

u32 otp_read_word(u16 idx);
u32 otp_read_serial(void);

#endif /* __SOC_SIFIVE_FU740_OTP_H__ */
