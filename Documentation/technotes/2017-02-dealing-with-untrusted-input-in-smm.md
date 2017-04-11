Dealing with Untrusted Input in SMM
===================================

Objective
---------
Intel Security recently held a talk and published
[slides](http://www.intelsecurity.com/advanced-threat-research/content/data/REConBrussels2017_BARing_the_system.pdf)
on a vulnerability in SMM handlers on x86 systems. They provide examples
on how both UEFI and coreboot are affected.

Background
----------
SMM, the System Management Mode, is a CPU mode that is configured by
firmware and survives the system’s initialization phase. On certain
events that mode can be triggered and executes code, suspending the
current processing that is going on the CPU, no matter whether it’s
in kernel or user space.

In SMM, the CPU has access to memory dedicated to that mode (SMRAM) that
is normally inaccessible, and typically some restrictions are lifted as
well (eg. in some configurations, certain flash write protection registers
are writable in SMM only).  This makes SMM a target for attacks which
seek to elevate a ring0 (kernel) exploit to something permanent.

Overview
--------
Intel Security showed several places in coreboot’s SMM handler (Slides
32+) that could be manipulated into writing data at user-chosen addresses
(SMRAM or otherwise), by modifying the BAR (Base Address Register) on
certain devices. By picking the right addresses and the right events
(and with them, mutators on the data at these addresses), it might
be possible to change the SMM handler itself to call into regular RAM
(where other code resides that then can work with elevated privileges).

Their proposed mitigations (Slide 37) revolve around making sure
that the BAR entries are reasonable, and point to a device instead of
regular memory or SMRAM. They’re not very detailed on how this could
be implemented, which is what this document discusses.

Detailed Design
---------------
The attack works because the SMM handler trusts the results of the
`pci_read_config32(dev, reg)` function, even though the value read by that
function can be modified in kernel mode.

In the general case it’s not possible to keep the cached value from
system initialization because there are legitimate modifications the
kernel can do to these values, so the only remedy is to make sure that
the value isn’t totally off.

For applications where hardware changes are limited by design (eg. no
user-modifiable PCIe slots) and where the running kernel is known,
such as Chromebooks, further efforts include caching the BAR settings
at initialization time and comparing later accesses to that.

What "totally off" means is chipset specific because it requires
knowledge of the memory map as seen by the memory controller: which
addresses are routed to devices, which are handled by the memory
controller itself?
The proposal is that in SMM, the `pci_read_config` functions (which
aren’t timing critical) _always_ validate the value read from a given
set of registers (the BARs) and fail hard (ie. cold reset, potentially
after logging the event) if they’re invalid (because that points to
a severe kernel bug or an attack).
The actual validation is done by a function implemented by the chipset code.

Another validation that can be done is to make sure that the BAR has the
appropriate bits set so it is enabled and points to memory (instead of
IO space).

In terms of implementation, this might look somewhat as follows. There
are a bunch of blanks to fill in, in particular how to handle the actual
config space access and there will be more registers that need to be
checked for correctness, both official BARs (0-4) and per-chipset
registers that need to be blacklisted in another chipset specific
function:

```c
static inline __attribute__((always_inline))
uint32_t pci_read_config32[d](pci_devfn_t dev, unsigned int where)
{
	uint32_t val = real_pci_read_config32(dev, where);
	if (IS_ENABLED(__SMM__) && (where == PCI_BASE_ADDRESS_0) &&
		is_mmio_ptr(dev, where) && !is_address_in_mmio(val)) {
			cold_reset();
	}
	return val;
}
```

`is_address_in_mmio(addr)` would be a newly introduced function to be
implemented by chipset drivers that returns true if the passed address
points into whatever is considered valid MMIO space.
`is_mmio_ptr(dev, where)` returns true for PCI config space registers that
point to BARs (allowing custom overrides because sometimes additional
registers are used to point to addresses).

For this function what is considered a legal address needs to be
documented, in accordance with the chipset design. (For example: AMD
K8 has a bunch of registers that define strictly which addresses are
"MMIO")

### Fully insured (aka “paranoid”) mode
For systems with more control over the hardware and kernel (such as
Chromebooks), it may be possible to set up the BARs in a way that the
kernel isn’t compelled to rewrite them, and store these values for
later comparison.

This avoids attacks such as setting the BAR to point to another device’s
MMIO region which the above method can’t catch. Such a configuration
would be “illegal”, but depending on the evaluation order of BARs
in the chipset, this might effectively only disable the device used for
the attack, while still fooling the SMM handler.

Since this method isn’t generalizable, it has to be an optional
compile-time feature.

Caveats
-------
This capability might need to be hidden behind a Kconfig flag
because we won’t be able to provide functional implementations of
`is_address_in_mmio()` for every chipset supported by coreboot from the
start.

Security Considerations
-----------------------
The actual exploitability of the issue is unknown, but fixing it serves
as defense in depth, similar to the
[Memory Sinkhole mitigation](https://review.coreboot.org/#/c/11519/) for
older Intel chipsets.

Testing Plan
------------
Manual testing can be conducted easily by creating a small payload that
provokes the reaction. It should test all conditions that enable the
address test (ie. the different BAR offsets if used by SMM handlers).
