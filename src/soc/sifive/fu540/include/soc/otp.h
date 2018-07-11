/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Philipp Hug <philipp@hug.cx>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __SOC_SIFIVE_HIFIVE_U_OTP_H__
#define __SOC_SIFIVE_HIFIVE_U_OTP_H__

u32 otp_read_word(u16 idx);
u32 otp_read_serial(void);

#endif /* __SOC_SIFIVE_HIFIFE_U_OTP_H__ */
