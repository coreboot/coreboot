# Option API

The option API around the `set_option(const char *name, void *val)` and
`get_option(void *dest, const char *name)` functions deprecated in favor
of a type-safe API.

Historically, options were stored in RTC battery-backed CMOS RAM inside
the chipset on PC platforms. Nowadays, options can also be stored in the
same flash chip as the boot firmware or through some BMC interface.

The new type-safe option framework can be used by calling
`enum cb_err set_uint_option(const char *name, unsigned int value)` and
`unsigned int get_uint_option_checked(const char *name, unsigned int fallback,
struct option_constraints rule)`.

The two-argument `get_uint_option()` reads from the configured backend.
It does not restrict the consumer's accepted values; new callers should use
`get_uint_option_checked()` with explicit constraints.

Each checked read specifies the values accepted by its consumer, for example:

```c
get_uint_option_checked("hyper_threading", 1, OPTION_BOOL);
get_uint_option_checked("debug_level", BIOS_INFO, OPTION_RANGE(BIOS_EMERG, BIOS_SPEW));
get_uint_option_checked("fan_speed", 50, OPTION_RANGE_STEP(0, 100, 10));
get_uint_option_checked("port_speed", SPEED_AUTO, OPTION_RANGE(SPEED_AUTO, SPEED_GEN2));
```

`OPTION_ENUM(...)` accepts only the listed values, including sparse enums:

```c
get_uint_option_checked("touchpad_haptics", STARLABS_TOUCHPAD_HAPTICS_MEDIUM,
	OPTION_ENUM(STARLABS_TOUCHPAD_HAPTICS_LOW, STARLABS_TOUCHPAD_HAPTICS_MEDIUM,
		    STARLABS_TOUCHPAD_HAPTICS_HIGH));
```

`OPTION_ENUM_VALUES(array, count)` accepts values from an existing array of
`unsigned int`. The array must remain alive until the read completes.

Backends reject absent, unreadable or invalid representations before returning
an integer. The checked accessor then applies the consumer constraints.
The fallback is trusted firmware policy and
is returned unchanged; it may be a sentinel outside the stored-value domain.
Malformed range constraints are programming errors and trigger an assertion.
If assertions are nonfatal, reads return the fallback. Invalid stored values
do not trigger assertions. Reads do not repair or rewrite storage. Keep checks involving multiple options
or hardware capabilities at the point of use.

Constraints apply to CMOS, CBFS and EFI storage alike. CMOS checksums still
detect corrupt storage, but cannot replace validation of individual values.
The setter does not know consumer constraints; code must not rely on the
writer or setup interface having validated a stored option.

The default setting is `OPTION_BACKEND_NONE`, which disables any runtime
configurable options. If supported by a mainboard, the `USE_OPTION_TABLE`
and `USE_MAINBOARD_SPECIFIC_OPTION_BACKEND` choices are visible, and can
be selected to enable runtime configurability.

# Mainboard-specific option backend

Mainboards with a mainboard-specific (vendor-defined) method to access
options can select `HAVE_MAINBOARD_SPECIFIC_OPTION_BACKEND` to provide
implementations of the option API accessors. To allow choosing between
multiple option backends, the mainboard-specific implementation should
only be built when `USE_MAINBOARD_SPECIFIC_OPTION_BACKEND` is selected.

Where possible, using a generic, mainboard-independent mechanism should
be preferred over reinventing the wheel in mainboard-specific code. The
mainboard-specific approach should only be used when the option storage
mechanism has to satisfy externally-imposed, vendor-defined constraints.
