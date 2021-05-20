# Option API

The option API around the `set_option(const char *name, void *val)` and
`get_option(void *dest, const char *name)` functions deprecated in favor
of a type-safe API.

Historically, options were stored in RTC battery-backed CMOS RAM inside
the chipset on PC platforms. Nowadays, options can also be stored in the
same flash chip as the boot firmware or through some BMC interface.

The new type-safe option framework can be used by calling
`enum cb_err set_uint_option(const char *name, unsigned int value)` and
`unsigned int get_uint_option(const char *name, const unsigned int fallback)`.

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
