# Testing ACPI changes under Windows

When testing ACPI changes in coreboot against Windows 8 or newer, beware that
during a normal boot after a clean shutdown, Windows will use the fast startup
mechanism which results in it not evaluating the changed ACPI code but instead
using some cached version which won't include the changes that were supposed to
be tested. In order for Windows to actually use the new ACPI tables, either
disable the fast startup or just tell Windows to do a reboot which will make it
read and use the ACPI tables in memory instead of an outdated cached version.
