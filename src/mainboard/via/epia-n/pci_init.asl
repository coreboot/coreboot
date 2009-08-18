/*
 * Minimalist ACPI DSDT table for EPIA-N / NL
 * Basic description of PCI Interrupt Assignments.
 * This is expected to be included into _SB.PCI0 namespace
 * (C) Copyright 2009 Jon Harrison <jon.harrison@blueyonder.co.uk>
 *
 */

/* This file provides a PCI Bus Initialisation Method that sets
 * some flags for use in the interrupt link assignment
 */

Method (\_SB.PCI0._INI, 0, NotSerialized)
{

	/* Checking for ATA Interface Enabled */
	Store (0x00, ATFL)
	If (LEqual (EIDE, 0x01))
	{
    	Store (0x02, ATFL)
	}
	Else
	{
    	If (LNotEqual (\_SB.PCI0.PATA.VID, 0x1106))
    	{
        	Store (0x01, ATFL)
    	}
	}

}
