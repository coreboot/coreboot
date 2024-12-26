# CFR - coreboot form representation

This documents the API exposed by coreboot to be consumed by
loaded OS image or payload.

## Problem Statement

As per coreboot design there's no UI present to change firmware
related options like "Hyper-Theading Enable". There's no way of
knowing what options are supported, if they are supported in the
current configuration and what they do.

The `USE_OPTION_TABLE` Kconfig allows to integrate a list of
mainboard specific options into coreboot tables when the option
API is using the *CMOS NVRAM*. It has no meaning if another option
API is being used.

## Design Proposal

Propose a new coreboot table that is independent from the option
backend. The coreboot table is generated from coreboot ramstage
code.

Every possible boot option is described by its name, the user
visible name, a help text, a default value and status flags.
All strings are in ASCII.

The boot options are grouped into forms, where each form hold
one or more options. Boot options that are not used in the current
boot flow, and are never reachable should be marked as hidden.
Dependecies between options can be defined in the code and should
be evaluated by the CFR parser/UI.

A boot option can be one of the following types:
- boolean
- number
- enum
- string

All of the information is *Position Independent Data*. That is, it is
safe to relocate any of the information without its meaning/correctness
changing.

CFR records form a tree structure. Every record starts with a `tag`
and a `size` field as generic header:

```C
struct __packed lb_cfr_header {
  uint32_t tag;
  uint32_t size;
};
```

The size of a record includes the size of its own fields plus the size of all
child records. A record can have none or multiple child records.
The record `tag` must be known by the parser to parse the record and its
sub records. If it is not known to the parser it can simply skip it by
jumping `size` bytes forward.

The coreboot table containing the CFR tree has the tag `LB_TAG_CFR`.

The public API can be found in
`src/commonlib/include/commonlib/cfr.h` and
`src/commonlib/include/commonlib/coreboot_tables.h`.

## Implementation design
### Tags
Tags identify the structure defined in `src/commonlib/include/commonlib/cfr.h`.
Every struct might be immediately followed by additional structs (so called
sub nodes), having their own tag and size field. The sum of all sub nodes size
fields plus the size of the struct itself equals the size field.

* CFR_TAG_OPTION_FORM

   Used in `struct lb_cfr_option_form` to describe a group of options. Every
   sub node is one option that should be displayed in the order found in
   memory.

   Allowed sub nodes:
    - `CFR_TAG_OPTION_ENUM`
    - `CFR_TAG_OPTION_NUMBER`
    - `CFR_TAG_OPTION_BOOL`
    - `CFR_TAG_OPTION_VARCHAR`
    - `CFR_TAG_OPTION_FORM`
    - `CFR_TAG_OPTION_COMMENT`
    - `CFR_TAG_VARCHAR_UI_NAME`

   Required sub nodes:
    - `CFR_TAG_VARCHAR_UI_NAME`

* CFR_TAG_ENUM_VALUE

   Used in `struct lb_cfr_enum_value` to describe a numeric value to be
   used in a parent `CFR_TAG_OPTION_ENUM`.

   Allowed sub nodes:
     - `CFR_TAG_VARCHAR_UI_NAME`

   Required sub nodes:
     - `CFR_TAG_VARCHAR_UI_NAME`

* CFR_TAG_OPTION_ENUM

   Used in `struct lb_cfr_numeric_option` to describe a numeric variable with
   a predefined selection of possible values in the referenced variable.

   Allowed sub nodes:
     - `CFR_TAG_VARCHAR_OPT_NAME`
     - `CFR_TAG_VARCHAR_UI_NAME`
     - `CFR_TAG_ENUM_VALUE`
     - `CFR_TAG_VARCHAR_UI_HELPTEXT`

   Required sub nodes:
     - `CFR_TAG_VARCHAR_OPT_NAME`
     - `CFR_TAG_VARCHAR_UI_NAME`
     - `CFR_TAG_ENUM_VALUE`

* CFR_TAG_OPTION_NUMBER

   Used in `struct lb_cfr_numeric_option` to describe a numeric variable with
   any possible value in the referenced variable.

   Allowed sub nodes:
     - `CFR_TAG_VARCHAR_OPT_NAME`
     - `CFR_TAG_VARCHAR_UI_NAME`
     - `CFR_TAG_VARCHAR_UI_HELPTEXT`

   Required sub nodes:
     - `CFR_TAG_VARCHAR_OPT_NAME`
     - `CFR_TAG_VARCHAR_UI_NAME`

* CFR_TAG_OPTION_BOOL

   Used in `struct lb_cfr_numeric_option` to describe a numeric variable with
   the possible values [0, 1] in the referenced variable.

   Allowed sub nodes:
     - `CFR_TAG_VARCHAR_OPT_NAME`
     - `CFR_TAG_VARCHAR_UI_NAME`
     - `CFR_TAG_VARCHAR_UI_HELPTEXT`

   Required sub nodes:
     - `CFR_TAG_VARCHAR_OPT_NAME`
     - `CFR_TAG_VARCHAR_UI_NAME`

* CFR_TAG_OPTION_VARCHAR

   Used in `struct lb_cfr_varchar_option` to describe an ASCII string
   stored in the referenced variable.

   *Example:* Linux kernel cmdline.

   Allowed sub nodes:
     - `CFR_TAG_VARCHAR_DEF_VALUE`
     - `CFR_TAG_VARCHAR_OPT_NAME`
     - `CFR_TAG_VARCHAR_UI_NAME`
     - `CFR_TAG_VARCHAR_UI_HELPTEXT`

   Required sub nodes:
     - `CFR_TAG_VARCHAR_DEF_VALUE`
     - `CFR_TAG_VARCHAR_OPT_NAME`
     - `CFR_TAG_VARCHAR_UI_NAME`

* CFR_TAG_OPTION_COMMENT

   Used in `struct lb_cfr_option_comment` to describe an ASCII string visible
   to the user, but doesn't reference a variable. Informal use only.

   Allowed sub nodes:
     - `CFR_TAG_VARCHAR_UI_NAME`
     - `CFR_TAG_VARCHAR_UI_HELPTEXT`

   Required sub nodes:
     - `CFR_TAG_VARCHAR_UI_NAME`

* CFR_TAG_VARCHAR_OPT_NAME

   Used in `struct lb_cfr_varbinary` to describe the option name used by
   coreboot's code. It thus must match what is used in code by
   `get_uint_option()`.

   Is not user visible.

* CFR_TAG_VARCHAR_UI_NAME

   Used in `struct lb_cfr_varbinary`

   User visible name of the option.

* CFR_TAG_VARCHAR_UI_HELPTEXT

   Used in `struct lb_cfr_varbinary`

   Optional user visible description what is changed by this option.

* CFR_TAG_VARCHAR_DEF_VALUE

   Used in `struct lb_cfr_varbinary`

   Default value in case the variable is not present.

### Flags

The optional flags describe the visibilty of the option and the
effect on the non-volatile variable.

* `CFR_OPTFLAG_READONLY`

   Prevents writes to the variable.

* `CFR_OPTFLAG_INACTIVE`

   Implies `READONLY`. The option is visible, but cannot be modified
   because one of the dependencies are not given. However there's a
   possibility to enable the option by changing runtime configuration.

   *For example:* Setting SATA mode, but SATA is globally disabled.

* `CFR_OPTFLAG_SUPPRESS`

   Runtime code sets this flag to indicate that the option has no effect
   and is never reachable, not even by changing runtime configuration.
   This option is never shown in the UI.

* `CFR_OPTFLAG_VOLATILE`

   Implies `READONLY`.
   The option is not backed by a non-volatile variable. This is useful
   to display the current state of a specific component, a dependency or
   a serial number. This information could be passed in a new coreboot
   table, but it not useful other than to be shown at this spot in the
   UI.

* `CFR_OPTFLAG_RUNTIME`

   The option is allowed to be changed by a post payload entity. On UEFI
   this sets the `EFI_VARIABLE_RUNTIME_ACCESS` attribute.
   It is out of scope of this specification how non runtime variables
   are protected after the payload has hand over control.

### Example

To display a boolean option with the label `Boolean`, that default value
is `true`, on a form called `test`, that modifies the variable `First`
the following structure will be generated:

```
struct lb_cfr_option_form {
  uint32_t tag;                  = CFR_TAG_OPTION_FORM
  uint32_t size;                 = sizeof(struct lb_cfr_option_form) +
                                   sizeof(struct lb_cfr_varbinary) +
                                   strlen(name) + 1 + 3 +
                                   sizeof(struct lb_cfr_numeric_option) +
                                   sizeof(struct lb_cfr_varbinary) +
                                   strlen(optname) + 1 + 2 +
                                   sizeof(struct lb_cfr_varbinary) +
                                   strlen(uiname) + 1 = 120
  uint64_t object_id;            = 1
  uint64_t dependency_id;        = 0
  uint32_t flags;                = 0
}
  struct lb_cfr_varbinary {
    uint32_t tag;                = CFR_TAG_VARCHAR_UI_NAME
    uint32_t size;               = sizeof(struct lb_cfr_varbinary) +
                                   strlen(name) + 1 + 3 = 20
    uint32_t data_length;        = strlen(name) + 1
  };
    char name[5];                = "test"
    char padding[3];
  struct lb_cfr_numeric_option {
    uint32_t tag;                = CFR_TAG_OPTION_BOOL
    uint32_t size;               = sizeof(struct lb_cfr_numeric_option) +
                                   sizeof(struct lb_cfr_varbinary) +
                                   strlen(optname) + 1 + 2 +
                                   sizeof(struct lb_cfr_varbinary) +
                                   strlen(uiname) + 1 = 72
    uint64_t object_id;          = 2
    uint64_t dependency_id;      = 0
    uint32_t flags;              = 0
    uint32_t default_value;      = true
  };
    struct lb_cfr_varbinary {
      uint32_t tag;              = CFR_TAG_VARCHAR_OPT_NAME
      uint32_t size;             = sizeof(struct lb_cfr_varbinary) +
                                   strlen(optname) + 1 + 2 = 20
      uint32_t data_length;      = strlen(optname) + 1 = 6
    };
      char optname[6];           = "First"
      char padding[2];
    struct lb_cfr_varbinary {
      uint32_t tag;              = CFR_TAG_VARCHAR_UI_NAME
      uint32_t size;             = sizeof(struct lb_cfr_varbinary) +
                                   strlen(uiname) + 1 = 20
      uint32_t data_length;      = strlen(uiname) + 1 = 8
    };
      char uiname[8];            = "Boolean"
```