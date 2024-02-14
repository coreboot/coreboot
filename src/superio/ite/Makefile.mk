# SPDX-License-Identifier: GPL-2.0-only

## include generic ite pre-ram stage driver
bootblock-$(CONFIG_SUPERIO_ITE_COMMON_PRE_RAM) += common/early_serial.c
romstage-$(CONFIG_SUPERIO_ITE_COMMON_PRE_RAM) += common/early_serial.c

## include generic ite environment controller driver
ramstage-$(CONFIG_SUPERIO_ITE_ENV_CTRL) += common/env_ctrl.c

## include generic ite driver to smm to control S3-relevant functions
smm-$(CONFIG_SUPERIO_ITE_COMMON_PRE_RAM) += common/early_serial.c

subdirs-y += it8528e
subdirs-y += it8613e
subdirs-y += it8623e
subdirs-y += it8629e
subdirs-y += it8659e
subdirs-y += it8712f
subdirs-y += it8718f
subdirs-y += it8720f
subdirs-y += it8721f
subdirs-y += it8728f
subdirs-y += it8772f
subdirs-y += it8783ef
subdirs-y += it8784e
subdirs-y += it8786e
