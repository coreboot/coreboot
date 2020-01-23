# SPDX-License-Identifier: GPL-2.0-or-later
# This file is part of the coreboot project.

ramstage-$(CONFIG_SUPERIO_ASPEED_AST2400) += superio.c
ramstage-$(CONFIG_SUPERIO_ASPEED_AST2400) += ../../common/ssdt.c
ramstage-$(CONFIG_SUPERIO_ASPEED_AST2400) += ../../common/generic.c
