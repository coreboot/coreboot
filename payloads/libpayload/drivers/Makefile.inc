##
## This file is part of the libpayload project.
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

TARGETS-$(CONFIG_PCI) += drivers/pci.o

TARGETS-$(CONFIG_SPEAKER) += drivers/speaker.o

TARGETS-$(CONFIG_SERIAL_CONSOLE) += drivers/serial.o

TARGETS-$(CONFIG_PC_KEYBOARD) += drivers/keyboard.o

TARGETS-$(CONFIG_NVRAM) += drivers/nvram.o
TARGETS-$(CONFIG_NVRAM) += drivers/options.o

# Video console drivers
TARGETS-$(CONFIG_VIDEO_CONSOLE) += drivers/video/video.o
TARGETS-$(CONFIG_VGA_VIDEO_CONSOLE) += drivers/video/vga.o

# Geode LX console drivers
TARGETS-$(CONFIG_GEODELX_VIDEO_CONSOLE) += drivers/video/geodelx.o
TARGETS-$(CONFIG_GEODELX_VIDEO_CONSOLE) += drivers/video/font8x16.o

# coreboot generic framebuffer driver
TARGETS-$(CONFIG_COREBOOT_VIDEO_CONSOLE) += drivers/video/corebootfb.o
TARGETS-$(CONFIG_COREBOOT_VIDEO_CONSOLE) += drivers/video/font8x16.o

# USB stack
TARGETS-$(CONFIG_USB) += drivers/usb/usbinit.o
TARGETS-$(CONFIG_USB) += drivers/usb/usb.o
TARGETS-$(CONFIG_USB) += drivers/usb/usb_dev.o
TARGETS-$(CONFIG_USB) += drivers/usb/quirks.o
TARGETS-$(CONFIG_USB_HUB) += drivers/usb/usbhub.o
TARGETS-$(CONFIG_USB_UHCI) += drivers/usb/uhci.o
TARGETS-$(CONFIG_USB_UHCI) += drivers/usb/uhci_rh.o
TARGETS-$(CONFIG_USB_OHCI) += drivers/usb/ohci.o
TARGETS-$(CONFIG_USB_OHCI) += drivers/usb/ohci_rh.o
TARGETS-$(CONFIG_USB_EHCI) += drivers/usb/ehci.o
TARGETS-$(CONFIG_USB_EHCI) += drivers/usb/ehci_rh.o
TARGETS-$(CONFIG_USB_XHCI) += drivers/usb/xhci.o
TARGETS-$(CONFIG_USB_XHCI) += drivers/usb/xhci_rh.o
TARGETS-$(CONFIG_USB_HID) += drivers/usb/usbhid.o
TARGETS-$(CONFIG_USB_MSC) += drivers/usb/usbmsc.o

# used by both USB HID and keyboard
TARGETS-y += drivers/hid.o
