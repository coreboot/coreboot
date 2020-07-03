/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * NOTE: this has to be here even when the board has no LPE audio, otherwise
 * it breaks the SOC specific ACPI code
 */
Scope (\_SB.PCI0.LPEA)
{
	Name (GBUF, ResourceTemplate () {})
}
