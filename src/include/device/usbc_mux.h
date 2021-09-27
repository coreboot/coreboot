/* SPDX-License-Identifier: GPL-2.0-only */

/* struct to hold all USB-C mux related variables */
struct usbc_mux_info {
	bool dp; /* DP connected */
	bool usb; /* USB connected */
	bool cable; /* 0 = Passive cable, 1 = Active cable */
	bool polarity; /* Polarity of connected device. 0 = Normal, 1 = Flipped */
	bool hpd_lvl; /* HPD Level assert */
	bool hpd_irq; /* HPD IRQ assert */
	bool ufp; /* 0 = DFP, 1 = UFP */
	bool dbg_acc; /* Debug Accessory. 0 = Disable, 1 = Enable */
	uint8_t dp_pin_mode; /* DP pin assignments
				0h: Reserved.
				1h: Pin Assignment A.
				2h: Pin Assignment B.
				3h: Pin Assignment C.
				4h: Pin Assignment D.
				5h: Pin Assignment E.
				6h: Pin Assignment F.
				7-Fh: Reserved. */
};
struct usbc_mux_ops {
	/*
	 * Get mux information on a given port.
	 *
	 * Return value:
	 * -1 = error
	 *  0 = success
	 */
	 int (*get_mux_info)(int port, struct usbc_mux_info *info);
};

struct usbc_dp_ops {
	/*
	 * Wait up to `timeout_ms` for DP connection to be ready on any available port.
	 *
	 * Return value:
	 * -1 = error
	 *  0 = no DP connection
	 * <bit mask> = mask for ports that are ready in DP mode.
	 */
	int (*wait_for_connection)(long timeout_ms);

	/*
	 * Enter DP mode on a given `port`.
	 *
	 * Return value:
	 * -1 = error
	 *  0 = success
	 */
	int (*enter_dp_mode)(int port);

	/*
	 * Wait up to `timeout_ms` for HPD on a given port.
	 *
	 * Return value:
	 * -1 = timeout
	 *  0 = success
	 */
	int (*wait_for_hpd)(int port, long timeout_ms);
};

struct usbc_ops {
	struct usbc_mux_ops mux_ops;
	struct usbc_dp_ops dp_ops;
};

const struct usbc_ops *usbc_get_ops(void);
