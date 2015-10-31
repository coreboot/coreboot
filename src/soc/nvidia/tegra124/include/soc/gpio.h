/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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

#ifndef __SOC_NVIDIA_TEGRA124_GPIO_H__
#define __SOC_NVIDIA_TEGRA124_GPIO_H__

#include <soc/nvidia/tegra/gpio.h>
#include <soc/pinmux.h>	/* for pinmux constants in GPIO macro */
#include <stdint.h>

/* GPIO index constants. */

#define GPIO_PORT_CONSTANTS(port) \
	GPIO_##port##0_INDEX, GPIO_##port##1_INDEX, GPIO_##port##2_INDEX, \
	GPIO_##port##3_INDEX, GPIO_##port##4_INDEX, GPIO_##port##5_INDEX, \
	GPIO_##port##6_INDEX, GPIO_##port##7_INDEX

enum {
	GPIO_PORT_CONSTANTS(A),
	GPIO_PORT_CONSTANTS(B),
	GPIO_PORT_CONSTANTS(C),
	GPIO_PORT_CONSTANTS(D),
	GPIO_PORT_CONSTANTS(E),
	GPIO_PORT_CONSTANTS(F),
	GPIO_PORT_CONSTANTS(G),
	GPIO_PORT_CONSTANTS(H),
	GPIO_PORT_CONSTANTS(I),
	GPIO_PORT_CONSTANTS(J),
	GPIO_PORT_CONSTANTS(K),
	GPIO_PORT_CONSTANTS(L),
	GPIO_PORT_CONSTANTS(M),
	GPIO_PORT_CONSTANTS(N),
	GPIO_PORT_CONSTANTS(O),
	GPIO_PORT_CONSTANTS(P),
	GPIO_PORT_CONSTANTS(Q),
	GPIO_PORT_CONSTANTS(R),
	GPIO_PORT_CONSTANTS(S),
	GPIO_PORT_CONSTANTS(T),
	GPIO_PORT_CONSTANTS(U),
	GPIO_PORT_CONSTANTS(V),
	GPIO_PORT_CONSTANTS(W),
	GPIO_PORT_CONSTANTS(X),
	GPIO_PORT_CONSTANTS(Y),
	GPIO_PORT_CONSTANTS(Z),
	GPIO_PORT_CONSTANTS(AA),
	GPIO_PORT_CONSTANTS(BB),
	GPIO_PORT_CONSTANTS(CC),
	GPIO_PORT_CONSTANTS(DD),
	GPIO_PORT_CONSTANTS(EE),
	GPIO_PORT_CONSTANTS(FF)
};

#endif	/* __SOC_NVIDIA_TEGRA124_GPIO_H__ */
