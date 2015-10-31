/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc
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

#ifndef GIC_H
#define GIC_H

#if IS_ENABLED(CONFIG_GIC)

/* Initialize the GIC on the currently processor, including GICD and GICC. */
void gic_init(void);
void gic_disable(void);
void gic_enable(void);

/* Return a pointer to the base of the GIC distributor mmio region. */
void *gicd_base(void);

/* Return a pointer to the base of the GIC cpu mmio region. */
void *gicc_base(void);

#else /* CONFIG_GIC */

static inline void gic_init(void) {}
static inline void gic_disable(void) {}

#endif /* CONFIG_GIC */

#endif /* GIC_H */
