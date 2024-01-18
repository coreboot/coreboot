# SPDX-License-Identifier: GPL-2.0-or-later

ramstage-$(CONFIG_SUPERIO_ASPEED_AST2400) += superio.c
ramstage-$(CONFIG_SUPERIO_ASPEED_AST2400) += ../../common/ssdt.c
ramstage-$(CONFIG_SUPERIO_ASPEED_AST2400) += ../../common/generic.c
