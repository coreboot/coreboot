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
extern int dbgp_bulk_write_x(struct ehci_debug_info *dbg_info, const char *bytes, int size);
extern int dbgp_bulk_read_x(struct ehci_debug_info *dbg_info, void *data, int size);
extern void set_ehci_base(unsigned ehci_base);
extern void set_ehci_debug(unsigned ehci_deug);
extern unsigned get_ehci_debug(void);
#endif
