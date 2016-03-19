/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Rockchip Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <soc/clock.h>
#include <soc/grf.h>

struct rk3399_grf_regs * const rk3399_grf = (void *)GRF_BASE;
struct rk3399_pmugrf_regs * const rk3399_pmugrf = (void *)PMUGRF_BASE;
struct rk3399_pmusgrf_regs * const rk3399_pmusgrf = (void *)PMUSGRF_BASE;

void rkclk_configure_spi(unsigned int bus, unsigned int hz)
{
}
