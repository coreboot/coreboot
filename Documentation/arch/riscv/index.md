# RISC-V architecture documentation

This section contains documentation about coreboot on RISC-V architecture.

## Mode usage
All stages run in M mode.

Payloads have a choice of managing M mode activity: they can control
everything or nothing.

Payloads run from the romstage (i.e. rampayloads) are started in M mode.
The payload must, for example, prepare and install its own SBI.

Payloads run from the ramstage are started in S mode, and trap delegation
will have been done. These payloads rely on the SBI and can not replace it.

## Stage handoff protocol
On entry to a stage or payload (including SELF payloads),
* all harts are running.
* A0 is the hart ID.
* A1 is the pointer to the Flattened Device Tree (FDT).
* A2 contains the additional program calling argument:
  - cbmem_top for ramstage
  - the address of the payload for opensbi

## Additional payload handoff requirements
The location of cbmem should be placed in a node in the FDT.

## OpenSBI
In case the payload doesn't install it's own SBI, like the [RISCV-PK] does,
[OpenSBI] can be used instead.
It's loaded into RAM after coreboot has finished loading the payload.
coreboot then will jump to OpenSBI providing a pointer to the real payload,
which OpenSBI will jump to once the SBI is installed.

Besides providing SBI it also sets protected memory regions and provides
a platform independent console.

The OpenSBI code is always run in M mode.

## Trap delegation
Traps are delegated to the payload.

## SMP within a stage
At the beginning of each stage, all harts save 0 are spinning in a loop on
a semaphore.  At the end of the stage harts 1..max are released by changing
the semaphore.

A possible way to do this is to have a pointer to a struct containing
variables, e.g.

```c
struct blocker {
	void (*fn)(); // never returns
}
```

The hart blocks until fn is non-null, and then calls it.  If fn returns, we
will panic if possible, but behavior is largely undefined.

Only hart 0 runs through most of the code in each stage.

[RISCV-PK]: https://github.com/riscv/riscv-pk
[OpenSBI]: https://github.com/riscv/opensbi
