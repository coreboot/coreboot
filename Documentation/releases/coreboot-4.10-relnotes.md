Upcoming release - coreboot 4.10
===========================

The 4.10 release is planned for April/May 2019

Update this document with changes that should be in the release
notes.
* Please use Markdown.
* See the [4.7](coreboot-4.7-relnotes.md) and [4.9](coreboot-4.9-relnotes.md)
 release notes for the general format.
* The chip and board additions and removals will be updated right
before the release, so those do not need to be added.

Significant changes
-------------------

### `device_t` is no more
coreboot used to have a data type, `device_t` that changed shape depending on
whether it is compiled for romstage (with limited memory) or ramstage (with
unlimited memory as far as coreboot is concerned). It's an old relic from the
time when romstage wasn't operated in Cache-As-RAM mode, but compiled with
our romcc compiler.

That data type is now gone.
