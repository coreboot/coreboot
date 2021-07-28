##
##
## Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
## Copyright (C) 2008 Advanced Micro Devices, Inc.
## Copyright (C) 2008 coresystems GmbH
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions
## are met:
## 1. Redistributions of source code must retain the above copyright
##    notice, this list of conditions and the following disclaimer.
## 2. Redistributions in binary form must reproduce the above copyright
##    notice, this list of conditions and the following disclaimer in the
##    documentation and/or other materials provided with the distribution.
## 3. The name of the author may not be used to endorse or promote products
##    derived from this software without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
## ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
## IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
## ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
## FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
## DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
## OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
## HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
## LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
## OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
## SUCH DAMAGE.
##

libc-$(CONFIG_LP_PCI) += pci_ops.c

ifeq ($(CONFIG_LP_PCI_IO_OPS),y)
libc-$(CONFIG_LP_PCI) += pci_io_ops.c
else
libc-$(CONFIG_LP_PCI) += pci_map_bus_ops.c
endif

libc-$(CONFIG_LP_SPEAKER) += speaker.c

libc-$(CONFIG_LP_8250_SERIAL_CONSOLE) += serial/8250.c serial/serial.c
libc-$(CONFIG_LP_S5P_SERIAL_CONSOLE) += serial/s5p.c serial/serial.c
libc-$(CONFIG_LP_IPQ806X_SERIAL_CONSOLE) += serial/ipq806x.c serial/serial.c
libc-$(CONFIG_LP_IPQ40XX_SERIAL_CONSOLE) += serial/ipq40xx.c serial/serial.c
libc-$(CONFIG_LP_QCS405_SERIAL_CONSOLE) += serial/qcs405.c serial/serial.c
libc-$(CONFIG_LP_QUALCOMM_QUPV3_SERIAL_CONSOLE) += serial/qcom_qupv3_serial.c serial/serial.c
libc-$(CONFIG_LP_PC_KEYBOARD) += i8042/keyboard.c
libc-$(CONFIG_LP_PC_MOUSE) += i8042/mouse.c
libc-$(CONFIG_LP_PC_I8042) += i8042/i8042.c

libc-$(CONFIG_LP_CBMEM_CONSOLE) += cbmem_console.c

libc-$(CONFIG_LP_MOUSE_CURSOR) += mouse_cursor.c

libc-$(CONFIG_LP_NVRAM) += nvram.c
libc-$(CONFIG_LP_NVRAM) += options.c

# Timer drivers
ifneq ($(CONFIG_LP_TIMER_GENERIC_HZ),0)
libc-y += timer/generic.c
endif
libc-$(CONFIG_LP_TIMER_RDTSC) += timer/rdtsc.c
libc-$(CONFIG_LP_TIMER_ARM64_ARCH) += timer/arm64_arch_timer.c

# Video console drivers
libc-$(CONFIG_LP_VIDEO_CONSOLE) += video/video.c
libc-$(CONFIG_LP_VGA_VIDEO_CONSOLE) += video/vga.c

# Geode LX console drivers
libc-$(CONFIG_LP_GEODELX_VIDEO_CONSOLE) += video/geodelx.c
libc-$(CONFIG_LP_GEODELX_VIDEO_CONSOLE) += video/font8x16.c
libc-$(CONFIG_LP_GEODELX_VIDEO_CONSOLE) += video/font.c

# coreboot generic framebuffer driver
libc-$(CONFIG_LP_COREBOOT_VIDEO_CONSOLE) += video/corebootfb.c
libc-$(CONFIG_LP_COREBOOT_VIDEO_CONSOLE) += video/font8x16.c
libc-$(CONFIG_LP_COREBOOT_VIDEO_CONSOLE) += video/font.c

# cbgfx: coreboot graphics library
libc-y += video/graphics.c

# AHCI/ATAPI driver
libc-$(CONFIG_LP_STORAGE) += storage/storage.c
libc-$(CONFIG_LP_STORAGE_AHCI) += storage/ahci.c
libc-$(CONFIG_LP_STORAGE_AHCI) += storage/ahci_common.c
libc-$(CONFIG_LP_STORAGE_NVME) += storage/nvme.c
ifeq ($(CONFIG_LP_STORAGE_ATA),y)
libc-$(CONFIG_LP_STORAGE_ATA) += storage/ata.c
libc-$(CONFIG_LP_STORAGE_ATA) += storage/ahci_ata.c
endif
ifeq ($(CONFIG_LP_STORAGE_ATAPI),y)
libc-$(CONFIG_LP_STORAGE_ATAPI) += storage/atapi.c
libc-$(CONFIG_LP_STORAGE_ATAPI) += storage/ahci_atapi.c
endif

# USB stack
libc-$(CONFIG_LP_USB) += usb/usbinit.c
libc-$(CONFIG_LP_USB) += usb/usb.c
libc-$(CONFIG_LP_USB) += usb/usb_dev.c
libc-$(CONFIG_LP_USB) += usb/quirks.c
libc-$(CONFIG_LP_USB_GEN_HUB) += usb/generic_hub.c
libc-$(CONFIG_LP_USB_HUB) += usb/usbhub.c
libc-$(CONFIG_LP_USB_UHCI) += usb/uhci.c
libc-$(CONFIG_LP_USB_UHCI) += usb/uhci_rh.c
libc-$(CONFIG_LP_USB_OHCI) += usb/ohci.c
libc-$(CONFIG_LP_USB_OHCI) += usb/ohci_rh.c
libc-$(CONFIG_LP_USB_EHCI) += usb/ehci.c
libc-$(CONFIG_LP_USB_EHCI) += usb/ehci_rh.c
libc-$(CONFIG_LP_USB_XHCI) += usb/xhci.c
libc-$(CONFIG_LP_USB_XHCI) += usb/xhci_debug.c
libc-$(CONFIG_LP_USB_XHCI) += usb/xhci_devconf.c
libc-$(CONFIG_LP_USB_XHCI) += usb/xhci_events.c
libc-$(CONFIG_LP_USB_XHCI) += usb/xhci_commands.c
libc-$(CONFIG_LP_USB_XHCI) += usb/xhci_rh.c
libc-$(CONFIG_LP_USB_HID) += usb/usbhid.c
libc-$(CONFIG_LP_USB_MSC) += usb/usbmsc.c
libc-$(CONFIG_LP_USB_DWC2) += usb/dwc2.c
libc-$(CONFIG_LP_USB_DWC2) += usb/dwc2_rh.c

# USB device stack
libc-$(CONFIG_LP_UDC) += udc/udc.c
libc-$(CONFIG_LP_UDC_CI) += udc/chipidea.c
libc-$(CONFIG_LP_UDC_DWC2) += udc/dwc2.c

# used by both USB HID and keyboard
libc-y += hid.c
