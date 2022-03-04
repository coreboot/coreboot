```eval_rst
:orphan:
```

# Communication templates related to release management

## Deprecation notices

Deprecation notices are part of release notes to act as a warning: at some
point in the future some part of coreboot gets removed. That point must be
at least 6 months after the release of the notice and it must be right after
some release: That is, the specified release must still contain the part in
question while one git commit later it might be removed.

The usual reason is progress: Infrastructure module X has been replaced by
infrastructure module X+1. Removing X helps keep the sources manageable
and likely opens opportunities to improve the codebase even more.
Sometimes everything using some module has been converted to its successor
already and it's natural for such modules to be removed. Even then it might
be useful to add an entry to the release notes to make everybody aware of
such a change, for maintainers of incomplete boards that they might keep in
their local trees and also to give credit to the developers of that change.

However this template isn't about such cases. Sometimes the tree contains
mainboards that rely on X and can't be easily migrated to X+1, often because
no active developer has access to these mainboards, and that is where this
type of deprecation notice comes in:

A deprecation notice shall outline what is being removed, when it is planned
for removal (always directly _after_ a future release so it remains clear when
something is part of coreboot and when it isn't anymore) and which devices
would be affected at the time of writing. Since past deprecation notices have
been read as "we plan to remove mainboards A, B, and C", sparking outrage
with the devoted users of A, B, or C, some care is necessary to make clear
which parts are slated for removal and which parts are merely consequences
if no action is taken. Or put differently: It should be obvious that besides
the deprecation plan, there is a call to action to save a couple of devices
from becoming officially unsupported.

As such, consider the following template when announcing a deprecation:

### The Thing to remove

A short description of the Thing slated for removal.

A short rationale why it's being removed (e.g. new and better Thing exists
in parallel; new Thing already demonstrated to work in this many releases;
removing Thing enables this or that improvement)

Timeline: Announced here, Thing will be removed right after the release X
months out (where X >= 6)

#### Call to action

Removing Thing requires work on a number of (boards, chipsets, …) that didn't
make the switch yet. The work approximately looks like this: (e.g. pointers to
commits where a board has been successfully migrated from Thing to new Thing).

Working on migrating away from Thing involves (hardware components, coreboot
systems, …) 1, 2, and 3. It's difficult to do on the remaining devices because
...

Parts of the tree that need work to become independent of Thing.
 - chipset A
   - board A1
   - board A2
 - chipset B
   - board B1

We prefer to move them along, but if we don't see any maintenance in our tree
we'll have to assume that there's no more interest in these platforms. As a
consequence these devices either have to work without Thing by the removal
date or they will be removed together with Thing. (side note: these removals
aren't the law, so if there's work in progress to move boards off X and a
roadmap that makes it probable to succeed, just not within the announced
deprecation timeline, we can still decide to postpone the actual removal by
one release. This needn't be put in the release notes themselves though or
it might encourage people to look for simple escape hatches.)

(If there are developers offering to write patches: )
There are developers interested in helping move these forward but they can't
test any changes for lack of equipment. If you have an affected device and
can run tests on it, please reach out to developers α, β, and γ.

(Otherwise maybe something more generic like this: )
If you want to take this on, the coreboot developer community will try to
help you: Reach out through one of our [forums](../community/forums.md).
