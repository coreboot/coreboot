/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

Scope (\_SB)
{
	Method (_SWS)
	{
		/* Index into PM1 for device that caused wake */
		Return (\PM1I)
	}
}

Scope (\_GPE)
{
	Method (_SWS)
	{
		/* Index into GPE for device that caused wake */
		Return (\GPEI)
	}
}
