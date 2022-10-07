/* SPDX-License-Identifier: GPL-2.0-only */

#include <northbridge/intel/haswell/haswell.h>
#include <northbridge/intel/haswell/raminit.h>
#include <southbridge/intel/lynxpoint/pch.h>

void mainboard_config_rcba(void)
{
}

void mb_get_spd_map(struct spd_info *spdi)
{
	spdi->addresses[0] = 0x50;
	spdi->addresses[1] = 0x51;
	spdi->addresses[2] = 0x52;
	spdi->addresses[3] = 0x53;
}

const struct usb2_port_config mainboard_usb2_ports[MAX_USB2_PORTS] = {
	{ 0x0110, 1, USB_OC_PIN_SKIP, USB_PORT_FRONT_PANEL }, /* USB3_4_5   */
	{ 0x0110, 1, USB_OC_PIN_SKIP, USB_PORT_FRONT_PANEL }, /* USB3_4_5   */
	{ 0x0110, 1, USB_OC_PIN_SKIP, USB_PORT_FRONT_PANEL }, /* USB3_6_7   */
	{ 0x0110, 1, USB_OC_PIN_SKIP, USB_PORT_FRONT_PANEL }, /* USB3_6_7   */
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_FLEX        }, /* ASM1074    */
	{ 0x0110, 1, USB_OC_PIN_SKIP, USB_PORT_FRONT_PANEL }, /* USB2_3     */
	{ 0x0110, 1, USB_OC_PIN_SKIP, USB_PORT_FRONT_PANEL }, /* USB2_3     */
	{ 0x0110, 1, USB_OC_PIN_SKIP, USB_PORT_FRONT_PANEL }, /* USB4_5     */
	{ 0x0110, 1, USB_OC_PIN_SKIP, USB_PORT_FRONT_PANEL }, /* USB4_5     */
	{ 0x0110, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL  }, /* USB1       */
	{ 0x0110, 1, USB_OC_PIN_SKIP, USB_PORT_MINI_PCIE   }, /* MINI_PCIE1 */
	{ 0x0000, 0, USB_OC_PIN_SKIP, USB_PORT_SKIP        },
	{ 0x0110, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL  }, /* Can be used if ASM1042 */
	{ 0x0110, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL  }, /* has not been installed */
};

const struct usb3_port_config mainboard_usb3_ports[MAX_USB3_PORTS] = {
	{ 1, USB_OC_PIN_SKIP }, /* USB3_4_5 */
	{ 1, USB_OC_PIN_SKIP }, /* USB3_4_5 */
	{ 1, USB_OC_PIN_SKIP }, /* USB3_6_7 */
	{ 1, USB_OC_PIN_SKIP }, /* USB3_6_7 */
	{ 1, USB_OC_PIN_SKIP }, /* ASM1074  */
	{ 0, USB_OC_PIN_SKIP }, /* N/A, GbE */
};
