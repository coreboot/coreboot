# SuperIO SSTD generator

This page describes the common SSDT ACPI generator for SuperIO chips that can
be found in coreboot.

## Functional description

In order to automatically generate ACPI functions you need to add
a new `chip superio/common` and `device pnp xx.0 on` to your devicetree.

The xx denotes the hexadecimal address of the SuperIO.

Place the regular LDN pnp devices behind those two entries.

The code will automatically guess the function based on the decoded
I/O range and ISA IRQ number.

## Example devicetree.cb

This example is based on AST2400.

```
# Add a "container" for proper ACPI code generation
chip superio/common
  device pnp 2e.0 on # just for the base device, not for the LDNs
    chip superio/aspeed/ast2400
      device pnp 2e.0 off  end
      device pnp 2e.2 on # SUART1
        io 0x60 = 0x3f8
        irq 0x70 = 4
      end
      device pnp 2e.3 on # SUART2
        io 0x60 = 0x2f8
        irq 0x70 = 3
      end
      device pnp 2e.4 on # SWC
        io 0x60 = 0xa00
        io 0x62 = 0xa10
        io 0x64 = 0xa20
        io 0x66 = 0xa30
        irq 0x70 = 0
      end
    end
  end
end
```

## Automatically generated methods

The following methods are generated for each SuperIO:
## AMTX()
Acquire the global mutex and enter config mode.
It's called this at the beginning of an atomic operation to make sure
no other ACPI code messes with the config space while working on it.

## RMTX()
Exit config mode and release the global mutex.
It's called at the end of an atomic operation.

## SLDN(Arg0)
Selects the (virtual) LDN given as Arg0.
This method isn't guarded with the global mutex.

## DLDN(Arg0)
Disables the (virtual) LDN given as Arg0.
This method acquires the global mutex.

## QLDN(Arg0)
Queries the state of the (virtual) LDN given as Arg0.
This method quires the global mutex.

## TODO

1) Add ACPI HIDs to every SuperIO driver
2) Generate support methods that allow
   * Setting resource settings at runtime
   * Getting resource settings at runtime
