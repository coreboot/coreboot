# Kconfig in coreboot

## Overview
Kconfig is a tool used in coreboot, Linux, and many other projects as the main
configuration mechanism.  In coreboot, it allows a developer both to select
which platform to build and to modify various features within the platform. The
Kconfig language was developed as a way to configure the Linux kernel, and is
still maintained as a part of the Linux kernel tree. Starting in Linux 2.5.45,
the ncurses based menuconfig was added, which is still used as the main
configuration front end in coreboot today.

The official Kconfig source and documentation is kept at kernel.org:

- [Kconfig source](https://git.kernel.org/cgit/linux/kernel/git/torvalds/linux.git/tree/scripts/kconfig)
- [Kconfig Language Documentation](https://www.kernel.org/doc/Documentation/kbuild/kconfig-language.txt)

The advantage to using Kconfig is that it allows users to easily select the
high level features of the project to be enabled or disabled at build time.
Ultimately the Kconfig tool generates a list of values which are used by the
source code and Makefiles of the project.  This allows the source files to
select features, and allows the build to determine which files should be
compiled and linked to the rom.


## Kconfig targets in Make
The Kconfig program in coreboot is built from source in util/kconfig. There are
various targets in the makefile to build Kconfig in different ways. These give
the user control over how to build the platform


### Front end configuration targets
These are the make targets that would be used to update the configuration of
the platform.
- config - Text mode configuration tool, asks each configuration option in turn.
  This does actually run in coreboot, but it is recommended that this not be
  used as there is no way to save a partial config.
- gconfig - Graphical configuration tool based on GTK+ 2.0.
- menuconfig - Text mode, menu driven configuration tool.
- nconfig - Text mode, menu driven configuration tool.
- xconfig - Graphical front end based on QT.


### Targets that update config files
These options are used to update the coreboot config files, typically .config.
The target file can be changed with variables in the environment or on the make
command line.

- defconfig - This generates a config based on another config file.  Use the
  environment variable KBUILD_DEFCONFIG to specify the base config file.
- oldconfig - Displays the answers to all configuration questions as it
  generates the config.h file.  If an interactive question is found that does
  not have an answer yet, it stops and queries the user for the desired value.
- olddefconfig - Generates a config, using the default value for any symbols not
  listed in the .config file.
- savedefconfig - Creates a ‘defconfig’ file, stripping out all of the symbols
  that were left as default values.  This is very useful for debugging, and is
  how config files should be saved.


### Targets not typically used in coreboot
- localmodconfig, localnoconfig, randconfig, allyesconfig, allnoconfig - These
  are all used to generate various Kconfig files for testing.


### Environment Variables that affect Kconfig
These variables are typically set in the makefiles or on the make command line.

#### Variables added to the coreboot Kconfig source
These variables were added to Kconfig specifically for coreboot and are not
included in the Linux version.

- KCONFIG_STRICT=value. Define to enable warnings as errors.   This is enabled
  in coreboot, and should not be changed.

- KCONFIG_NEGATIVES=value. Define to show negative values in the autoconf.h file
  (build/config.h). This is enabled in coreboot, and should not be changed.


#### Variables used to control the input and output config files
- KBUILD_DEFCONFIG=inputname of the defconfig file.  This defaults to
  ‘configs/defconfig’ and is used by the ‘defconfig’ target.

- DEFCONFIG=output name of the defconfig file.  This defaults to ‘defconfig’
  and is used by ‘savedefconfig’ target as the output filename.

- DOTCONFIG=name of the .config file.  This defaults to '.config' and is used
  by most config type targets.


#### Variables used by the makefiles for controlling Kconfig
- Kconfig=root Kconfig file.  This is set to 'src/Kconfig' in the coreboot
  makefile.

- KCONFIG_CONFIG=input config file.  coreboot sets this to $(DOTCONFIG).

- KCONFIG_AUTOHEADER=path and filename of autoconf.h file. coreboot sets this
  to $(obj)/config.h.

- KCONFIG_DEPENDENCIES=”kbuild dependency file".  This defaults to
  auto.conf.cmd and outputs the name of all of the Kconfig files used.

- KCONFIG_SPLITCONFIG=”directory name for individual SYMBOL.h files”.
  coreboot sets this to $(obj)/config.

#### Used only for ‘make menuconfig’
- MENUCONFIG_MODE=single_menu.  Set to "single_menu" to enable.  All other
  values disable the option.  This makes submenus appear below the menu option
  instead of opening a new screen.

- MENUCONFIG_COLOR=&lt;theme&gt;.  This sets the color theme for the menu from
  these options:

   -  mono       =&gt; selects colors suitable for monochrome displays.
   -  blackbg    =&gt; selects a color scheme with black background.
   -  classic    =&gt; theme with blue background. The classic look.
   -  bluetitle  =&gt; an LCD friendly version of classic. (default).


#### Used only for ‘make nconfig’

- NCONFIG_MODE=single_menu

   Submenus appear below the menu option instead of opening a new screen.


#### Unused in coreboot

Although these variables are not used by coreboot, their values should be left
at the default values.  Other values may have unexpected effects on the
codebase.

- KCONFIG_SEED=randconfig seed value.
- KCONFIG_PROBABILITY=randconfig percent to set to y.
- KCONFIG_NOSILENTUPDATE=value.  Define to prevent silent updates to .config
  file.
- KCONFIG_OVERWRITECONFIG=value. Define to prevent breaking a .config symlink.
- KCONFIG_TRISTATE=filename of tristate.conf file.
- SRCTREE=path to src directory.
- KCONFIG_AUTOCONFIG=path and filename of the auto.conf file.

    coreboot sets this to $(obj)/auto.conf.  Although this value is actually
    set by coreboot, the resulting file is not used.

- CONFIG_=prefix for Kconfig output symbols.

   coreboot expects this to *NOT* be set.



## Kconfig Language

The Kconfig language has about 30 keywords, some overloaded, and some with the
same meaning.  Whitespace may have specific meaning, for example in the 'help'
keyword.  There are 8 logical operators for use in expressions, which allow
values to be set based on other values.


### Terminology

- Symbols - There are two types of symbols, "constant" and “non-constant”.
    - Constant symbols are alphanumeric values used in expressions for
      comparison. The Kconfig language specification says that these must be
      surrounded by single or double quotes.
    - Non-constant symbols are the 'config' values that are output into the
      saved .config, auto.conf and config.h files. Non-constant symbols are
      typically defined with the 'config' keyword, although they can also be
      defined with the 'choice' keyword. These symbols may be used in a file's
      expressions before they are defined. Valid characters for non-constant
      symbols are any combination of alphanumeric characters, underscore.
      Although “1234” is accepted as a symbol name, as is “o_o”, convention is
      to use all uppercase words that are descriptive of the symbol's use so
      they make sense when turned into CONFIG_NAME #defines.

- Expressions - An expression is a logical evaluation. It can be as simple as
  a static 'y' or 'n', or can be a symbol. Alternatively, expressions can be
  complex evaluations of multiple symbols using the various logical operators.
  The Kconfig language allows these logical evaluations in several places. The
  most common use for complex expressions is following 'if' or ‘depends on’
  keywords, but they can also be used to set the value for a prompt or default
  values.

- Types - Each Kconfig symbol is one of the following types: bool, hex, int,
  string, or tristate. The tristate type is not used for coreboot, leaving just
  four types. As noted in the keyword summaries, a symbol must have a consistent
  type anywhere it is defined. Also, Kconfig will simply not display a symbol
  that has no type defined. A warning will be displayed in the terminal where
  menuconfig was run if this happens:
  _src/Kconfig:25:warning: config symbol defined without type_.

- Prompts - Input prompts are the text associated with the symbols which shown
  to the user. The Kconfig language definition does not require surrounding the
  prompt’s text with quotes, however it is recommended that quotes be used for
  maximum compatibility.

- Menu Entries - These keyword blocks create the symbols and questions that are
  visible in the front end.


## Keywords

### bool

The 'bool' keyword assigns a boolean type to a symbol. The allowable values for
a boolean type are 'n' or 'y'. The keyword can be followed by an optional prompt
string which makes the symbol editable in one of the configuration front ends.


##### Usage:
bool \[prompt\] \[if &lt;expr&gt;\]


##### Example:
    config ANY_TOOLCHAIN
        bool "Allow building with any toolchain"
        default n
        depends on COMPILER_GCC


##### Notes:
- Putting the prompt after the 'bool' keyword is the same as using a 'prompt'
  keyword later. See the 'prompt' keyword for more notes.
- Only the first type definition for each symbol is valid. Further matching
  definitions are fine, although unnecessary. Conflicting type definitions will
  be ignored, and a warning will be presented on the console where the
  configuration front end was run:
  _warning: ignoring type redefinition of 'SYMBOL' from 'hex' to 'integer'_.
- Boolean symbols do not need a default and will default to ‘n’.


##### Restrictions:

- This keyword must be within a symbol definition block, started by the
  'config' keyword.

--------------------------------------------------------------------------------

### choice

This creates a selection list of one or more boolean symbols. For bools, only
one of the symbols can be selected, and one will be be forced to be selected,
either by a ‘default’ statement, or by selecting the first config option if
there is no default value listed.

##### Usage:
choice \[symbol\]
- \[prompt\]
- \[default\]


##### Example:
    choice TESTCHOICE
        prompt "Test choice"
        default TESTCHOICE2 if TESTCHOICE_DEFAULT_2
        default TESTCHOICE3

    config TESTCHOICE1
        bool "Choice 1"
    config TESTCHOICE2
        bool "Choice 2"
    config TESTCHOICE3
        bool "Choice 3"
    config TESTCHOICE4
        bool "Choice 4" if TESTCHOICE_SHOW_4
    endchoice

    config TESTCHOICE_DEFAULT_2
        def_bool y

    config TESTCHOICE_SHOW_4
        def_bool n

    config TESTSTRING
        string
        default “String #1” if TESTCHOICE1
        default “String #2” if TESTCHOICE2
        default “String #4” if TESTCHOICE3
        default “String #4” if TESTCHOICE4
        default “”


##### Notes:
- If no symbol is associated with a choice, then you can not have multiple
  definitions of that choice. If a symbol is associated to the choice, then
  you may define the same choice (ie. with the same entries) in another place.
  Any selection in either location will update both choice menu selections. In
  coreboot, the value of the symbol is always 1.
- As shown in the example above, the choice between bools can be used to set
  the default for a non-bool type.  This works best when the non-bool type
  does not have an input prompt.


##### Restrictions:
- Symbols used for 'choice' entries must have input prompts defined using the
  'prompt' keyword.
- Symbols used in 'choice' entries may not be enabled with a 'select'
  statement, they can be defaulted using a second Kconfig symbol however.

--------------------------------------------------------------------------------

### comment

This keyword defines a line of text that is displayed to the user in the
configuration frontend and is additionally written to the output files.


##### Usage:
comment &lt;prompt&gt;
- \[depends on\]


##### Example:

    if CONSOLE_SERIAL
        comment "I/O mapped, 8250-compatible"
        depends on DRIVERS_UART_8250IO
    endif


##### Notes:
- Comments are only valid outside of config blocks, but can be within menu and
  if blocks.

--------------------------------------------------------------------------------

### config

This is the keyword that starts a block defining a Kconfig symbol. The symbol
modifiers follow the 'config' statement.

##### Usage:
config &lt;symbol&gt;

-  \[bool | def_bool | int | hex | string\]
- \[depends on\]
- \[prompt\]
- \[help\]
- \[range\]
- \[select\]


##### Example:
    config SEABIOS_PS2_TIMEOUT
        prompt "PS/2 keyboard timeout" if PAYLOAD_SEABIOS
        default 0
        depends on PAYLOAD_SEABIOS
        int
        help
          Some PS/2 keyboard controllers don't respond to commands
          immediately after powering on. This specifies how long
          SeaBIOS will wait for the keyboard controller to become
          ready before giving up.


##### Notes:
- Non-coreboot projects also use the 'tristate' and 'def_tristate' types.
- Ends at the next Kconfig keyword that is not valid inside the config block:

    menu | endmenu | if | endif | choice | config | source | comment

--------------------------------------------------------------------------------

### default

The ‘default’ keyword assigns a value to a symbol in the case where no preset
value exists, i.e. the symbol is not present and assigned in .config.  If there
is no preset value, and no ‘default’ keyword, the user will be asked to enter a
valid value when building coreboot.


##### Usage:
default &lt;expr&gt; \[if &lt;expr&gt;\]


##### Example:
    config GENERATE_MP_TABLE
        prompt "Generate an MP table" if HAVE_MP_TABLE || DRIVERS_GENERIC_IOAPIC
        bool
        default HAVE_MP_TABLE || DRIVERS_GENERIC_IOAPIC
        help
          Generate an MP table (conforming to the Intel
          MultiProcessor specification 1.4) for this board.


##### Notes:
- Kconfig defaults for symbols without a prompt *NEVER* affect existing legal
  symbol definitions in a .config file. The default only affects the symbol if
  there is no valid definition in a config file.  This is a frequent source of
  confusion.  It’s covered again in the Tips section below.
- The first valid 'default' entry for a symbol is always used. Any further
  'default' statements for a symbol are ignored.  This means that the order of
  Kconfig files is very important as the earlier files get to set the defaults
  first.  They should be sourced in the order from most specific (mainboard
  Kconfig files) to the most generic (architecture-specific Kconfig files).
- If there is no 'default' entry for a symbol, it gets set to 'n', 0, 0x0, or
  “” depending on the type, however the 'bool' type is the only type that
  should be left without a default value.
- If possible, the declaration should happen before all default entries to make
  it visible in Kconfig tools like menuconfig.

--------------------------------------------------------------------------------

### def_bool

‘def_bool’ is similar to the 'bool' keyword in that it sets a symbol’s type to
boolean. It lets you set the type and default value at the same time, instead
of setting the type and the prompt at the same time. It's typically used for
symbols that don't have prompts.


##### Usage:
def_bool &lt;expr&gt; \[if &lt;expr&gt;\]


##### Example:
    config EC_GOOGLE_CHROMEEC_LPC
        depends on EC_GOOGLE_CHROMEEC && ARCH_X86
        def_bool y
        select SERIRQ_CONTINUOUS_MODE
        help
          Google Chrome EC via LPC bus.


##### Notes:
- Only the first type definition for each symbol is valid. Further matching
  definitions are fine, although unnecessary. Conflicting type definitions will
  be ignored, and a warning will be presented on the console where the
  configuration front end was run:
  _warning: ignoring type redefinition of 'SYMBOL' from 'hex' to 'integer'_.

##### Restrictions:
- This keyword must be within a symbol definition block, started by the
  'config' keyword.

--------------------------------------------------------------------------------

### depends on

This defines a dependency for a menu entry, including symbols and comments.  It
behaves the same as surrounding the menu entry with an if/endif block.  If the
‘depends on’ expression evaluates to false, the 'prompt' value will not be
printed, and defaults will not be set based on this block.


##### Usage:
depends on &lt;expr&gt;


##### Example:
    config COMMON_CBFS_SPI_WRAPPER
        bool
        default n
        depends on SPI_FLASH
        depends on !ARCH_X86
        help
          Use common wrapper to interface CBFS to SPI bootrom.


##### Notes:
- Symbols that have multiple ‘depends on’ sections as above are equivalent to a
  single ‘depends on’ statement with sections joined by &&.  So the above is
  the same as “depends on SPI_FLASH && ! ARCH_X86”.

--------------------------------------------------------------------------------

### endchoice

This ends a choice block. See the 'choice' keyword for more information and an
example.

--------------------------------------------------------------------------------

### endif

This ends a block started by the 'if' keyword. See the 'if' keyword for more
information and an example.

--------------------------------------------------------------------------------

### endmenu

This ends a menu block. See the 'menu' keyword for more information and an
example.

--------------------------------------------------------------------------------

### help

The 'help' keyword defines the subsequent block of text as help for a config or
choice block. The help block is started by the 'help' keyword on a line by
itself, and the indentation level of the next line controls the end of the help
block. The help ends on the next non-blank line that has an indentation level
less than the indentation level of the first line following the 'help' keyword.

##### Usage:
help &lt;help text&gt;


##### Example:
    config COMPILER_GCC
        bool "GCC"
        help
          Use the GNU Compiler Collection (GCC) to build coreboot.  For details
          see http://gcc.gnu.org.


##### Notes:
- Identical to the '---help---' keyword which isn’t used in coreboot.
- Other keywords are allowed inside the help block, and are not recognized as
  keywords so long as the indentation rules are followed, even if they start a
  line.


##### Restrictions:
- Only used for 'config' and 'choice' keywords.

--------------------------------------------------------------------------------

### hex

This is another symbol type specifier, specifying an unsigned integer value
formatted as hexadecimal.

##### Usage:
hex &lt;expr&gt; \[if &lt;expr&gt;\]


##### Example:
    config INTEL_PCH_UART_CONSOLE_NUMBER
        hex "Serial IO UART number to use for console"
        default 0x0
        depends on INTEL_PCH_UART_CONSOLE


##### Notes:
- Kconfig doesn’t complain if you don’t start the default value for a hex
  symbol with ‘0x’, but not doing so will lead to issues.  It will update 10
  to 0x10 without warning the user.
- Putting the prompt text after the 'hex' keyword is the same as using a
  'prompt' keyword later. See the 'prompt' keyword for more notes.
- Only the first type definition for each symbol is valid. Further matching
  definitions are fine, although unnecessary. Conflicting type definitions will
  be ignored, and a warning will be presented on the console where the
  configuration front end was run:
  _warning: ignoring type redefinition of 'SYMBOL' from 'hex' to 'integer'_.


##### Restrictions:
- This keyword must be within a symbol definition block, started by the
  'config' keyword.
- 'hex' type symbols must have a 'default' entry set.

--------------------------------------------------------------------------------

### if

The 'if' keyword is overloaded, used in two different ways. The first definition
enables and disables various other keywords, and follows the other keyword
definition. This usage is shown in each of the other keywords' usage listings.

The second usage of the 'if' keyword is part of an if/endif block. Most items
within an if/endif block are not evaluated, while others, such as the 'source'
keyword, ignore the existence of the if/endif block completely. Symbols defined
within an if/endif block are still created, although their default values are
ignored - all values are set to 'n'.


##### Usage:
if &lt;expr&gt;

- \[config\]
- \[choice\]
- \[comment\]
- \[menu\]

endif


##### Example:
    if ARCH_X86

    config SMP
        bool
        default y if MAX_CPUS != 1
        default n
        help
          This option is used to enable certain functions to make
          coreboot work correctly on symmetric multi processor (SMP) systems.
    endif

##### Restrictions:
- Corresponding ‘if’ and ‘endif’ statements must exist in the same file.

--------------------------------------------------------------------------------

### int

A type setting keyword, defines a symbol as an integer, accepting only signed
numeric values.  The values can be further restricted with the ‘range’ keyword.


##### Usage:
int &lt;expr&gt; \[if &lt;expr&gt;\]


##### Example:
    config PRE_GRAPHICS_DELAY_MS
        int "Graphics initialization delay in ms"
        default 0
        help
          On some systems, coreboot boots so fast that connected
          monitors (mostly TVs) won't be able to wake up fast enough
          to talk to the VBIOS. On those systems we need to wait for a
          bit before executing the VBIOS.


##### Notes:
- Only the first type definition for each symbol is valid. Further matching
  definitions are fine, although unnecessary. Conflicting type definitions will
  be ignored, and a warning will be presented on the console where the
  configuration front end was run:
  _warning: ignoring type redefinition of 'SYMBOL' from 'hex' to 'integer'_.


##### Restrictions:
- This keyword must be within a symbol definition block, started by the 'config'
  keyword.
- 'int' type symbols must have a default value set.

--------------------------------------------------------------------------------

### mainmenu

The 'mainmenu' keyword sets the title or title bar of the configuration front
  end, depending on how the configuration program decides to use it. It can only
  be specified once and at the very beginning of the top level Kconfig file,
  before any other statements.


##### Usage:
mainmenu &lt;prompt&gt;

##### Example:
mainmenu "coreboot configuration"

##### Restrictions:
- Must be the first statement in the top level Kconfig.
- Must only be used once in the entire Kconfig tree.

--------------------------------------------------------------------------------

### menu

The 'menu' and 'endmenu' keywords tell the configuration front end that the
enclosed statements are part of a group of related pieces.


##### Usage:
menu &lt;prompt&gt;

- \[choice\]
- \[config\]
- \[menu\]
- \[if/endif\]

endmenu


##### Example:
    menu "On-Chip Device Power Down Control"
    config TEMP_POWERDOWN
        bool "Temperature sensor power-down"

    config SATA_POWERDOWN
        bool "SATA power-down"

    config ADC_POWERDOWN
        bool "ADC power-down"

    config PCIE0_POWERDOWN
        bool "PCIE0 power-down"

    config MAC_POWERDOWN
        bool "MAC power-down"

    config USB1_POWERDOWN
        bool "USB2.0 Host Controller 1 power-down"

    config IDE_POWERDOWN
        bool "IDE power-down"

    endmenu

##### Restrictions:
- Must be closed by a corresponding ‘endmenu’ keyword in the same file.

--------------------------------------------------------------------------------

### prompt

The 'prompt' keyword sets the text displayed for a config symbol or choice in
configuration front end.


##### Usage:
prompt &lt;prompt&gt; \[if &lt;expr&gt;\]


##### Example:
    config REALMODE_DEBUG
        prompt "Enable debug messages for option ROM execution"
        bool
        default n
        depends on PCI_OPTION_ROM_RUN_REALMODE
        depends on DEFAULT_CONSOLE_LOGLEVEL_7 || DEFAULT_CONSOLE_LOGLEVEL_8
        help
          This option enables additional x86emu related debug
          messages.  Note: This option will increase the time to emulate a ROM.

          If unsure, say N.


##### Notes:
- The same rules apply for menu entries defined by the 'prompt' keyword and
  other prompt types such as those defined by the 'int' or 'string' keywords.
- Redefining the prompt text in multiple instances of config symbols is allowed.
  Only the current prompt statement for a particular entry will be displayed by
  the front end in any given location.  This means that multiple mainboards can
  set different prompt values for a symbol, and it would appear differently in
  each mainboard’s menu.  The symbol can even have multiple entries in the same
  menu with different prompts if desired. For example, both of these would get
  printed, and changing either entry would change the other.

        config PROMPT_TEST
            string "Prompt value 1"

        config PROMPT_TEST
            prompt "Prompt value 2"

- Although not required, it's recommended that you use quotes around prompt
  statements.
* If the prompt is redefined inside the SAME config entry, you will get a
  warning:
  _warning: prompt redefined_.
  For example, this is not allowed:

        config PROMPT_TEST
            string "Prompt value 1"
            prompt "Prompt value 2"
--------------------------------------------------------------------------------

### range

This sets the allowable minimum and maximum entries for hex or int type config
symbols.


##### Usage:
range &lt;symbol&gt; &lt;symbol&gt; \[if &lt;expr&gt;\]


##### Example:
    config TEST1
        hex "test 1"
        range 0x0 0x10


##### Notes:
- Only the first definition of a range is used for any symbol. Further
  definitions will be ignored without warning.

--------------------------------------------------------------------------------

### select

The ‘select’ keyword is used within a bool type config block.  In coreboot (and
other projects that don't use modules), the 'select' keyword can force an
unassociated bool type symbol to 'y'.  When the symbol for the config block is
‘y’, the ‘select’ action is taken.  Otherwise the bool is unaffected.


##### Usage:
select &lt;symbol&gt; \[if &lt;expr&gt;\]


##### Example:
    config TPM
        bool
        default n
        select MEMORY_MAPPED_TPM if ARCH_X86
        select I2C_TPM if ARCH_ARM
        select I2C_TPM if ARCH_ARM64
        help
          Enable this option to enable TPM support in coreboot.
          If unsure, say N.

##### Notes:
- Using the 'select' keyword can create logical contradictions in Kconfig, which
  will create warnings and fail to save the .config.  Following is an example of
  an obviously invalid configuration, where selecting BOOLTEST violates the
  ‘depends on’ of BOOLTEST2:

        config BOOLTEST
            bool "bool Test"
            select BOOLTEST2

        config BOOLTEST2
            bool "Bool Test 2"
            depends on !BOOLTEST

##### Restrictions:
- The ‘select’ keyword only works on bool type symbols.
- Symbols created inside of choice blocks cannot be selected, and should be
  enabled by using default values instead.

--------------------------------------------------------------------------------

### source

The 'source' keyword functions much the same as an 'include' statement in c.
This pulls one or more files into Kconfig at the location of the 'source'
command. This statement is always parsed - there is no way to conditionally
source a file. coreboot has modified the source statement slightly to handle
directory globbing. The '*' character will match with any directory.


##### Usage:
source &lt;prompt&gt;


##### Example:

    choice
        prompt "Mainboard vendor"
        default VENDOR_EMULATION

        source "src/mainboard/*/Kconfig.name"

    endchoice

    source "src/mainboard/*/Kconfig"


##### Notes:
- As with all prompt values, the 'source' prompt may be enclosed in single or
  double quotes, or left without any quotes.  Using quotes is highly recommended
  however.
- The 'source' keyword loads files relative to the working directory where the
  Kconfig command was run. For coreboot, this is the root coreboot directory, so
  all source commands in the src directory need to start with ‘src/’.
- In coreboot's Kconfig, if a sourced file does not exist, the statement is
  simply ignored. This is different than other versions of Kconfig.
- 'source' pulls a file into the Kconfig tree at the location of the keyword.
  This allows for files containing small bits of the Kconfig tree to be pulled
  into a larger construct.  A restriction on this is that the starting/ending
  keyword pairs must be within the same file - ‘endif’ cannot appear in a
  different file than the ‘if’ statement that it ends. The same is true of
  menu/endmenu and choice/endchoice pairs.

The coreboot Kconfig structure uses this along with globbing to build up the
mainboard directory.  Each mainboard’s Kconfig.name file contains just two
statements that generate a list of all the platform names:

    config BOARD_AMD_NORWICH
         bool "Norwich"


##### Restrictions:
- 'source' keywords always load in the specified file or files. There is no way
  to optionally pull in a file. Putting an if/endif block around a source
  command does not affect the source command, although it does affect the
  content.  To avoid confusion, use if/endif blocks inside sourced files to
  remove their content if necessary.

--------------------------------------------------------------------------------

### string

The last of the symbol type assignment keywords. 'string' allows a text value to
be entered.


##### Usage:
string &lt;expr&gt; \[if &lt;expr&gt;\]


##### Example:
    config BOOTBLOCK_SOUTHBRIDGE_INIT
        string
        default "southbridge/amd/pi/hudson/bootblock.c"

    config HUDSON_GEC_FWM_FILE
        string "GEC firmware path and filename"
        depends on HUDSON_GEC_FWM


##### Notes:
- Putting the prompt after the 'string' keyword is the same as using a 'prompt'
keyword later. See the prompt keyword for more notes.
- Only the first type definition for each symbol is valid. Further matching
  definitions are fine, although unnecessary. Conflicting type definitions will
  be ignored, and a warning will be presented on the console where the
  configuration front end was run:
  _warning: ignoring type redefinition of 'SYMBOL' from 'hex' to 'string'_.
- Some characters may not get interpreted correctly when inside a string entry
  depending on how they are used - inside a C file, inside a Makefile, passed
  through a Makefile to a C file, or something else.  It may be necessary to
  escape the characters at times.  Because this is very dependent upon how the
  symbol is actually used, a definitive guide cannot be given here.
- 'string' type variables do NOT need a default, and will default to the
  value “”.


##### Restrictions:
- This keyword must be within a symbol definition block, started by the 'config'
  keyword.

--------------------------------------------------------------------------------




## Keywords not used in coreboot at the time of writing:

- allnoconfig_y:
- defconfig_list
- def_tristate
- env
- ---help---
- menuconfig
- modules
- optional
- option
- tristate
- visible if


## Build files generated by Kconfig

### build/config.h

The config.h file is a very basic header file made up of a list of #define
statements:

    #define SYMBOL NAME XXX


##### Symbol types:
- bool, int, and hex types -  Every symbol of one of these types created in the
  Kconfig tree is defined.  It doesn’t matter whether they’re in an if/endif
  block, or have a ‘depends on’ statement - they ALL end up being defined in
  this file.
- String - Only string types that actually have a value associated with them
  are defined.

The config.h file uses 0 and 1 to represent Kconfig's 'n' and 'y' values
respectively. String values are placed inside double quotes.

The name of the file is controlled by the $KCONFIG_AUTOHEADER environment
variable, which is set to $(obj)/config.h by the coreboot makefiles.

The prefix used for the symbols is controlled by the $CONFIG_ environment
variable.  This is not set in coreboot, which uses the default CONFIG_ prefix
for all of its symbols.

The coreboot makefile forces the config.h file to be included into all coreboot
C files. This is done in Makefile.inc on the compiler command line using the
“-include $(obj)/config.h” command line option.

Example of various symbol types in the config.h file:

    #define CONFIG_BOOTBLOCK_SOURCE "bootblock_simple.c" # String
    #define CONFIG_CBFS_SIZE 0x00300000                  # Hex
    #define CONFIG_TTYS0_BAUD 115200                     # Int
    #define CONFIG_HAVE_ACPI_TABLES 1                    # Bool enabled
    #define CONFIG_EXPERT 0                              # Bool disabled
    #define CONFIG_NORTHBRIDGE_INTEL_I440LX 0            # Bool excluded


### .config

The .config file in the root directory is used as the input file, but also by
the makefiles to set variable values. The main difference is that it does not
contain all of the symbols. It excludes symbols defined in an if/endif block
whose expression evaluated as false. Note that the symbol
CONFIG_NORTHBRIDGE_INTEL_I440LX shown in the config.h example above is not
present in the .config file.

In addition, the .config file below contains the 'comment' prompt text from the
Kconfig, separating the blocks.

    ## General setup ##
    CONFIG_BOOTBLOCK_SOURCE="bootblock_simple.c" # String
    CONFIG_CBFS_SIZE=0x00300000                  # Hex
    CONFIG_TTYS0_BAUD=115200                     # Int
    CONFIG_HAVE_ACPI_TABLES=y                    # Bool enabled
    # CONFIG_EXPERT is not set                   # Bool disabled

This file is included directly by the makefile, and sets the CONFIG symbols so
that they are available during the build process.


### build/auto.conf

Although the controlling variable for the auto.conf filename,
KCONFIG_AUTOCONFIG, is set in the coreboot makefiles, the auto.conf file itself
is not used by coreboot.  This file has the same syntax and structure as the
.config file, but contains all symbols in the Kconfig tree, including those that
are not enabled, or are excluded by if/endif blocks, or the 'depends on'
keyword.  The kconfig tool could be updated to not generate this file, but since
it's not hurting anything, it's still being generated.



## Defconfig or Miniconfig files

Miniconfig files are the standard .config files with all of the symbols which
are set to their default values stripped out.  These files are very useful for
debugging your config, as well as being the best way to store your .config file.
If you store your config as a full config file, it will be much harder to
maintain.  Any Kconfig symbols with updated default values will retain their old
values,  and any symbols which have been removed will still remain in the file.
Storing full config files just generally leads to a lot more maintenance than
storing miniconfig files.

The easiest way to generate the miniconfig file is by running

    make savedefconfig DOTCONFIG=.config DEFCONFIG=[output file]

DEFCONFIG defaults to ‘defconfig’, DOTCONFIG defaults to ‘.config’.


To turn the miniconfig back into a full config file, use one of the two targets:

    make olddefconfig DOTCONFIG=[input/output file]

or

    make defconfig KBUILD_DEFCONFIG=[input file] DOTCONFIG=[output file]

In both of these cases, DOTCONFIG defaults to .config.



## Editing and updating saved .config files


### Don’t save full config files

Save miniconfig files, as mentioned in the previous section.


### Disable values with ‘# CONFIG_SYMBOL is not set’

A common mistake when trying to disable a value is to edit the .config file and
change it from ‘CONFIG_SYMBOL=y’ to ‘CONFIG_SYMBOL=n’, but this doesn’t
correctly disable the symbol.  If the default value for the symbol is ‘n’ to
begin with, this isn’t an issue - the symbol just gets ignored, and the default
value is used.  The problem is where the default for the symbol is ‘y’.  When
the bad entry in the .config file gets ignored, the value is set back to ‘y’,
leading to much frustration.

Always disable the Kconfig symbols in the .config file with the syntax:

    # CONFIG_SYMBOL is not set

### Only the LAST instance of a symbol is used

When reading a saved .config file, Kconfig uses the LAST instance of a symbol
that it comes across, and ignores any previous instances. This can be used to
override symbols in a saved .config file by appending the new value to a config
file.

For example:

A .config file that contains these two lines:

    # CONFIG_BOOLTEST is not set
    CONFIG_BOOLTEST=y

After running ‘make olddefconfig’, ends up with the value:

    CONFIG_BOOLTEST=y

A case where this can be used is by a making a script to create two versions of
a coreboot rom for a single platform. The first ROM could be built with serial
console disabled, and the second ROM, built as a debug version, could have
serial console enabled by overriding the "CONFIG_CONSOLE_SERIAL" symbol, and
setting it to enabled.

## General Kconfig Tips and Notes

### Default values for config options

The FIRST valid default that the Kconfig parser comes across will be used for
each symbol. This means that the organization of the tree is very important.
The structure should go from most specific at the top of the Kconfig tree to the
most general later in the tree.  In coreboot, the mainboard directories get
loaded first, as they are sourced very early in the src/Kconfig file.  Chipset
Kconfig files get sourced later, and the architecture specific Kconfig files get
sourced even later.  This allows the mainboards to set their defaults early,
overriding the default values set in chipset or architecture.

Due to this mechanism, a default defined early cannot be changed by a default
set in a later Kconfig file. There are ways around this, involving 'depends on'
statements, but they add additional variables which are generally just used
internal to Kconfig.


### Select statement usage

The 'select' keyword forces the value of a symbol with a bool type to 'y'. It
overrides any dependencies of the symbol, so using it carelessly can lead to
unpredictable results.



### All bool, int, and hex Kconfig symbols are ALWAYS defined in the C code

All bool, int, and hex Kconfig symbols are ALWAYS defined in the C code if they
are in a sourced Kconfig - do NOT use #ifdef CONFIG_SYMBOL

String symbols are the exception.  All others (int, hex, etc.) are always
defined in config.h.  Never use an #ifdef statement for a Kconfig symbol other
than strings in C to determine whether the symbol is enabled or disabled. So
long as the symbol is in ANY sourced Kconfig file, it will be defined. Even if
the symbol is only inside of an if/endif block where the if expression evaluates
as false, the symbol STILL gets defined in the config.h file (though not in the
.config file).

Use \#if CONFIG(SYMBOL) to be sure (it returns false for undefined symbols
and defined-to-0 symbols alike).



### Symbols with prompts use defaults *ONLY* when initially created or enabled.

Symbols with a prompt which may be user-modified are NOT updated to default
values when changing between platforms or modifying other symbols. There are
only two times the default values are used:
1. When a config is initially created.
2. When a dependency which had previously kept the symbol from being active
   changes to allowing it to be active.

Because of this, starting with a saved .config for one platform and updating it
for another platform can lead to very different results than creating a platform
from scratch.



### Symbols with no prompt will be the default value (unless 'select' is used).

Symbols that do not have a prompt will always use the first valid default value
specified in Kconfig. They cannot be updated, even if they are modified in a
saved .config file. As always, a 'select' statement overrides any specified
'default' or 'depends on' statement.


## Differences between coreboot's Kconfig and other Kconfig implementations.

- coreboot has added the glob operator '*' for the 'source' keyword.
- coreboot’s Kconfig always defines variables except for strings. In other
  Kconfig implementations, bools set to false/0/no are not defined.
- coreboot’s version of Kconfig adds the KCONFIG_STRICT environment variable to
  error out if there are any issues in the Kconfig files.  In the Linux kernel,
  Kconfig will generate a warning, but will still output an updated .config or
  config.h file.


## Kconfig Editor Highlighting

#### vim:

vim has syntax highlighting for Kconfig built in (or at least as a part of
vim-common), but most editors do not.


#### ultraedit:

https://github.com/martinlroth/wordfiles/blob/master/kconfig.uew



#### atom:

https://github.com/martinlroth/language-kconfig


## Syntax Checking:

The Kconfig utility does some basic syntax checking on the Kconfig tree.
Running "make oldconfig" will show any errors that the Kconfig utility
sees.

### util/kconfig_lint

Because the Kconfig utility is relatively forgiving, and ignores issues that a
developer might be interested in, kconfig_lint, another Kconfig checker has been
written.

The file kconfig_lint and its associated readme can be found in the coreboot
utils/lint directory.  It is useful for parsing the Kconfig tree, and for
showing warnings, errors, and notes about coreboot’s Kconfig files.


    kconfig_lint <options>
       -o|--output=file         Set output filename
       -p|--print               Print full output
       -e|--errors_off          Don't print warnings or errors
       -w|--warnings_off        Don't print warnings
       -n|--notes               Show minor notes
       --path=dir               Path to top level kconfig
       -c|--config=file         Filename of config file to load
       -G|--no_git_grep         Use standard grep tools instead of git grep


The -p option is very useful for debugging Kconfig issues, because it reads all
of the Kconfig files in the order that the Kconfig tools would read them, and
prints it out, along with where each line came from and which menu it appears
in.

## License:
This work is licensed under the Creative Commons Attribution 4.0 International
License. To view a copy of this license, visit
https://creativecommons.org/licenses/by/4.0/ or send a letter to Creative
Commons, PO Box 1866, Mountain View, CA 94042, USA.

Code examples snippets are licensed under GPLv2, and are used here under fair
use laws.
