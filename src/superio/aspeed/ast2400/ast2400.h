/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Edward O'Callaghan <eocallaghan@alterapraxis.com>
 * Copyright (C) 2018 Eltan B.V.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef SUPERIO_ASPEED_AST2400_H
#define SUPERIO_ASPEED_AST2400_H

#define AST2400_SUART1	0x2 /* Com1 */
#define AST2400_SUART2	0x3 /* Com2 */
#define AST2400_SWAK	0x4 /* System Wake-Up control */
#define AST2400_KBC	0x5 /* Keyboard controller */
#define AST2400_GPIO	0x7 /* GPIO */
#define AST2400_SUART3	0xB /* Com3 */
#define AST2400_SUART4	0xC /* Com4 */
#define AST2400_ILPC2AHB	0xD /* LPC 2 AHB */
#define AST2400_MAILBOX	0xE /* Mailbox */

#endif /* SUPERIO_ASPEED_AST2400_H */
