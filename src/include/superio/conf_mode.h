#ifndef DEVICE_PNP_CONF_MODE_H
#define DEVICE_PNP_CONF_MODE_H

#include <device/device.h>
#include <device/pnp.h>

/* Common enter/exit implementations */
void pnp_enter_conf_mode_55(device_t dev);
void pnp_enter_conf_mode_8787(device_t dev);
void pnp_exit_conf_mode_aa(device_t dev);

extern const struct pnp_mode_ops pnp_conf_mode_55_aa;
extern const struct pnp_mode_ops pnp_conf_mode_8787_aa;

#endif
