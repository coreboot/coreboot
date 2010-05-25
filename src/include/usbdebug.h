/*
 * This file is part of the coreboot project.
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#ifndef USBDEBUG_H
#define USBDEBUG_H

struct ehci_debug_info {
        void *ehci_caps;
        void *ehci_regs;
        void *ehci_debug;
        unsigned devnum;
        unsigned endpoint_out;
        unsigned endpoint_in;
};

int dbgp_bulk_write_x(struct ehci_debug_info *dbg_info, const char *bytes, int size);
int dbgp_bulk_read_x(struct ehci_debug_info *dbg_info, void *data, int size);
void set_ehci_base(unsigned ehci_base);
void set_ehci_debug(unsigned ehci_deug);
unsigned get_ehci_debug(void);
void set_debug_port(unsigned port);

#endif
