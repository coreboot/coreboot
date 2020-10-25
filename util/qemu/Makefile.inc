# SPDX-License-Identifier: BSD-3-Clause

# This automatically adds a `qemu` make target if a compatible
# configuration is selected. There are some notable variables
# that can be set or adapted in `Makefile.inc` files, the make
# command line or the environment:
#
# Primarily for `Makefile.inc` use:
# QEMU-y		the QEMU executable
# QEMU_CFG-y		a QEMU config that sets the available default devices,
#			used to run more comprehensive tests by default,
#			e.g. many more PCI devices
#
# For general use:
# QEMU_ARGS		additional command line arguments (default: -serial stdio)
# QEMU_EXTRA_CFGS	additional config files that can add devices
#
# QEMU_CFG_ARGS		gathers config file related arguments,
#			can be used to override a default config (QEMU_CFG-y)
#
# Examples:
#
#   $ # Run coreboot's default config with additional command line args
#   $ make qemu QEMU_ARGS="-cdrom site-local/grml64-small_2018.12.iso"
#
#   $ # Force QEMU's built-in config
#   $ make qemu QEMU_CFG_ARGS=

QEMU-$(CONFIG_BOARD_EMULATION_QEMU_X86_I440FX)	?= qemu-system-x86_64 -M pc
QEMU-$(CONFIG_BOARD_EMULATION_QEMU_X86_Q35)	?= qemu-system-x86_64 -M q35

ifneq ($(QEMU-y),)

QEMU_ARGS ?= -serial stdio
QEMU_EXTRA_CFGS ?=

QEMU_CFG_ARGS ?= \
	$(if $(QEMU_CFG-y),-nodefaults) \
	$(addprefix -readconfig ,$(QEMU_CFG-y) $(QEMU_EXTRA_CFGS))

qemu: $(obj)/coreboot.rom
	$(QEMU-y) $(QEMU_CFG_ARGS) $(QEMU_ARGS) -bios $<

.PHONY: qemu

endif
