/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef GIC_H
#define GIC_H

#if CONFIG(GIC)

/* Initialize the GIC on the currently processor, including GICD and GICC. */
void gic_init(void);
void gic_disable(void);
void gic_enable(void);

/* Return a pointer to the base of the GIC distributor mmio region. */
void *gicd_base(void);

/* Return a pointer to the base of the GIC CPU mmio region. */
void *gicc_base(void);

#else /* CONFIG_GIC */

static inline void gic_init(void) {}
static inline void gic_disable(void) {}

#endif /* CONFIG_GIC */

#endif /* GIC_H */
