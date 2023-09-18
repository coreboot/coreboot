Upcoming release - coreboot 4.22
========================================================================

The 4.22 release is planned for mid-November, 2023

Update this document with changes that should be in the release notes.

* Please use Markdown.
* See the past few release notes for the general format.
* The chip and board additions and removals will be updated right
  before the release, so those do not need to be added.
* Note that all changes before the release are done are marked upcoming.
  A final version of the notes are done after the release.

### Significant or interesting changes

* Add changes that need a full description here

* This section should have full descriptions and can or should have
  a link to the referenced commits.

### x86: support .data section for pre-memory stages

x86 pre-memory stages did not support the `.data` section and as a
result developers were required to include runtime initialization code
instead of relying on C global variable definition.

Other platforms do not have that limitation. Hence, resolving it helps
to align code and reduce compilation based restriction (cf. the use of
`ENV_HAS_DATA_SECTION` compilation flag in various places of coreboot
code).

There were three types of binary to consider:
1. eXecute-In-Place pre-memory stages
2. `bootblock` stage is a bit different as it uses Cache-As-Ram but
   the memory mapping and its entry code different
3. pre-memory stages loaded in and executed from Cache-As-RAM
   (cf. `CONFIG_NO_XIP_EARLY_STAGES`).

eXecute-In-Place pre-memory stages (#1) relies on a new ELF segment as
the code segment Virtual Memory Address and Load Memory Address are
identical but the data needs to be linked in cache-As-RAM (VMA) but to
be stored right after the code (LMA).

`bootblock` (#2) also uses this new segment to store the data right
after the code and it loads it to Cache-As-RAM at runtime. However,
the code involved is different.

Not eXecute-In-Place pre-memory stages (#3) did not really need any
special work other than enabling a .data section as the code and data
VMA / LMA translation vector is the same.

Related important commits:

- c9cae530e5ac54c5b3639d0d555966ca5cad65ff ("cbfstool: Make add-stage
  support multiple ignore sections")
- 79f2e1fc8b6192e96f99c05f71baeb77d4633d40 ("cbfstool: Make add-stage
  support multiple loadable segments")
- b7832de0260b042c25bf8f53abcb32e20a29ae9c ("x86: Add .data section
  support for pre-memory stages")

### Toolchain updates

* Upgrade GMP from 6.2.1 to 6.3.0
* Upgrade binutils from 2.40 to 2.41
* Upgrade MPFR from 4.2.0 to 4.2.1

### Additional coreboot changes

The following are changes across a number of patches, or changes worth
noting, but not needing a full description.

* Changes that only need a line or two of description go here.

### Platform Updates

* To be filled in immediately before the release by the release team

### Plans to move platform support to a branch

* Section to be filled in or removed after discussion

### Statistics from the 4.21 to the 4.22 release

* To be filled in immediately before the release by the release team


### Significant Known and Open Issues

* To be filled in immediately before the release by the release team
