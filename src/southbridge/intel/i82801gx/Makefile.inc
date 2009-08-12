##
## This file is part of the coreboot project.
##
## Copyright (C) 2008-2009 coresystems GmbH
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
##

driver-$(CONFIG_SOUTHBRIDGE_INTEL_I82801GX) +=  i82801gx.o
driver-$(CONFIG_SOUTHBRIDGE_INTEL_I82801GX) +=  i82801gx_ac97.o
driver-$(CONFIG_SOUTHBRIDGE_INTEL_I82801GX) +=  i82801gx_azalia.o
driver-$(CONFIG_SOUTHBRIDGE_INTEL_I82801GX) +=  i82801gx_ide.o
driver-$(CONFIG_SOUTHBRIDGE_INTEL_I82801GX) +=  i82801gx_lpc.o
driver-$(CONFIG_SOUTHBRIDGE_INTEL_I82801GX) +=  i82801gx_nic.o
driver-$(CONFIG_SOUTHBRIDGE_INTEL_I82801GX) +=  i82801gx_pci.o
driver-$(CONFIG_SOUTHBRIDGE_INTEL_I82801GX) +=  i82801gx_pcie.o
driver-$(CONFIG_SOUTHBRIDGE_INTEL_I82801GX) +=  i82801gx_sata.o
driver-$(CONFIG_SOUTHBRIDGE_INTEL_I82801GX) +=  i82801gx_smbus.o
driver-$(CONFIG_SOUTHBRIDGE_INTEL_I82801GX) +=  i82801gx_usb.o
driver-$(CONFIG_SOUTHBRIDGE_INTEL_I82801GX) +=  i82801gx_usb_ehci.o

object-$(CONFIG_SOUTHBRIDGE_INTEL_I82801GX) +=  i82801gx_reset.o
object-$(CONFIG_SOUTHBRIDGE_INTEL_I82801GX) +=  i82801gx_watchdog.o

# arg. How does the linux kconfig handle compound conditionals?
ifeq ($(CONFIG_HAVE_SMI_HANDLER),y)
	object-$(CONFIG_SOUTHBRIDGE_INTEL_I82801GX) +=  i82801gx_smi.o
	smmobj-$(CONFIG_SOUTHBRIDGE_INTEL_I82801GX) +=  i82801gx_smihandler.o
endif
