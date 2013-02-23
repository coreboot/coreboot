/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef SUPERIO_NSC_PC87384_H
#define SUPERIO_NSC_PC87384_H

#define PC87384_PP   0x01
#define PC87384_SP2  0x02
#define PC87384_SP1  0x03
#define PC87384_GPIO 0x07

#define PC87384_GPIO_PIN_OE 0x01
#define PC87384_GPIO_PIN_TYPE_PUSH_PULL 0x02
#define PC87384_GPIO_PIN_PULLUP 0x04
#define PC87384_GPIO_PIN_LOCK 0x08
#define PC87384_GPIO_PIN_TRIG_LEVEL 0x10
#define PC87384_GPIO_PIN_TRIG_LOW 0x20
#define PC87384_GPIO_PIN_DEBOUNCE 0x40

#define PC87384_GPIO_PIN_TRIGGERS_IRQ 0x01

#endif
