/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef DEVICE_PNP_CONF_MODE_H
#define DEVICE_PNP_CONF_MODE_H

#include <device/device.h>
#include <device/pnp.h>

/* Common enter/exit implementations */
void pnp_enter_conf_mode_55(struct device *dev);
void pnp_enter_conf_mode_6767(struct device *dev);
void pnp_enter_conf_mode_7777(struct device *dev);
void pnp_enter_conf_mode_8787(struct device *dev);
void pnp_enter_conf_mode_a0a0(struct device *dev);
void pnp_enter_conf_mode_a5a5(struct device *dev);
void pnp_exit_conf_mode_aa(struct device *dev);
void pnp_enter_conf_mode_870155aa(struct device *dev);
void pnp_exit_conf_mode_0202(struct device *dev);

extern const struct pnp_mode_ops pnp_conf_mode_55_aa;
extern const struct pnp_mode_ops pnp_conf_mode_6767_aa;
extern const struct pnp_mode_ops pnp_conf_mode_7777_aa;
extern const struct pnp_mode_ops pnp_conf_mode_8787_aa;
extern const struct pnp_mode_ops pnp_conf_mode_a0a0_aa;
extern const struct pnp_mode_ops pnp_conf_mode_a5a5_aa;
extern const struct pnp_mode_ops pnp_conf_mode_870155_aa;

#endif /* DEVICE_PNP_CONF_MODE_H */
