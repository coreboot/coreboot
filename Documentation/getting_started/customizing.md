# Customizing Module Behavior

A common pattern in software development to allow "more specific" modules
customize behavior of "more foundational" modules (or to hook into them) is
dynamic _dependency injection_, which many developers might be used to.

coreboot modules also vary in their degree of specialization; the following
list goes from "foundational" to "specific" and is by no means complete:
* **commonlib:** Library and helper code shared by many modules.
* **Platform code:** Code that supports a computing platform, e.g. `arch/x86`
  (including bootblock, romstage, ramstage) or `arch/arm64`.
* **drivers** **CPU, SoC, North-/Southbridge code:** Code that supports a
  specific CPU platform, like "Intel Alder Lake".
* **mainboard:** Mainboard-specific setup code, configuration data and hooks.
* **Mainboard variants:** Optional; a mainboard can have multiple variants
  which differ in details only.

In some contexts, coreboot exhibits a static variant of this design pattern
where, instead of registering function pointers at runtime, the configurability
is achieved **statically**, assisted by the linker.

## Configurability in coreboot

Various modules offer **customization points** in form of functions with
default or no-op implementations that defined as **weak symbols**. They
can be overridden by more specialized implementation as needed.  Note that
outside of certain cases, using weak functions over function pointers has its
disadvantages and is therefore discouraged. (More below.)

For example, `lib/bootblock.c` offers the definition
`__weak void bootblock_mainboard_early_init(void) { /* no-op */ }`. Mainboard
implementations are free to re-define this symbol to perform any early
initialization they need (e.g. early GPIO init).

The linker will always prefer the non-weak definition over a weak definition
and discard the latter, rather than throwing a "duplicate definition" error.
Only if no other definition of the symbol exists, the linker will take the
`__weak` implementation, rather than throwing an "undefined symbol" error.

## The role of Kconfig and Makefiles
As `git grep` reveals, lots of files inside the `mainboard/` directory
implement the `bootblock_mainboard_early_init(void)` function; usually in files
named `early_init.c` or `bootblock.c`, but this is only convention.

In order to select _which_ of these implementations is actually used, we
leverage the [build system](build_system.md). Only _one_ of these source
files implementing a particular symbol is actually built and linked into the
final result, for any given build configuration.

Note that when there is a function call into code which only gets included when
a specific Kconfig option is selected, it's preferable to also make that call
itself conditional on the Kconfig option in order to improve readability.

## Identifying customization points

Unfortunately, there is currently no exhaustive list of these customization
points. An easy way to identify them, however, is `git grep -w __weak`. This
shows their default definition in the `.c` files; looking up their declaration
in the `.h` files often reveals some additional documentation.

## Use Cases and Non-Use Cases

Using weak symbols comes with its disadvantages, e.g. bad discoverability and
surprising behavior when a wrong overridden function is included in the build
or when the default implementation is used instead of the desired override.
These problems are hard to debug because there are no build errors that could
indicate the problem.

As a rule of thumb, usages with limited or clear scope are ok, such as
mainboard variants, the SMBIOS table overrides and hooks in common code that
mainboards or SoCs can override. Weak symbols enable easy hooking here without
requiring RAM to be functional already.

Outside of those cases, it is usually better to resort to other patterns, like
e.g. function pointers. Like described above, these come with the advantage of
causing clear build errors when used incorrectly, rather than failing in
surprising ways at runtime.
