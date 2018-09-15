# RISCV architecture documentation

This section contains documentation about coreboot on RISCV architecture.

## Mode usage
All stages run in M mode.

Payloads have a choice of managing M mode activity: they can control
everything or nothing.

Payloads run from the romstage (i.e. rampayloads) are started in M mode.
The payload must, for example, prepare and install its own SBI.

Payloads run from the ramstage are started in S mode, and trap delegation
will have been done. These payloads rely on the SBI and can not replace it.

## Stage handoff protocol
On entry to a stage or payload,
* all harts are running.
* A0 is the hart ID
* A1 is the pointer to the Flattened Device Tree (FDT).

## Additional payload handoff requirements
The location of cbmem should be placed in a node in the FDT.

## Trap delegation
Traps are delegated in the ramstage.

## SMP within a stage
At the beginning of each stage, all harts save 0 are spinning in a loop on a semaphore.
At the end of the stage harts 1..max are released by changing the
semaphore.

A possible way to do this is to have a pointer to a struct containing variables, e.g.

```c
struct blocker {
	void (*fn)(); // never returns
}
```

The hart blocks until fn is non-null, and then calls it.
If fn returns we will panic if possible, but behavior
is largely undefined.

Only hart 0 runs through most of the code in each stage.

