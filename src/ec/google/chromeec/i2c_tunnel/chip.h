/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __EC_GOOGLE_CHROMEEC_I2C_TUNNEL__
#define __EC_GOOGLE_CHROMEEC_I2C_TUNNEL__

struct ec_google_chromeec_i2c_tunnel_config {
	/* ACPI device name */
	const char *name;
	/* ACPI _UID */
	unsigned int uid;
	/* EC I2C bus number we tunnel to on the other side. */
	unsigned int remote_bus;
};

#endif /* __EC_GOOGLE_CHROMEEC_I2C_TUNNEL__ */
