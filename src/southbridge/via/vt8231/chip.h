#ifndef _SOUTHBRIDGE_VIA_VT8231
#define _SOUTHBRIDGE_VIA_VT8231

extern struct chip_control southbridge_via_vt8231_control;

struct southbridge_via_vt8231_config {
  int enable_usb;
  int enable_native_ide;
  int enable_com_ports;
  int enable_keyboard;
  int enable_nvram;
};

#endif /* _SOUTHBRIDGE_VIA_VT8231 */
