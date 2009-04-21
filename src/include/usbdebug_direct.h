#ifndef USBDEBUG_DIRECT_H
#define USBDEBUG_DIRECT_H

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
