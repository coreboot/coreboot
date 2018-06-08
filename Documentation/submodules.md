Use of git submodules in coreboot
=================================
coreboot uses git submodules to keep certain parts of the tree separate,
with two major use cases:

First, we use a vendor tool by NVIDIA for systems based on their SoC
and since they publish it through git, we can just import it into our
tree using submodules.

Second, lots of boards these days require binaries and we want to keep
them separate from coreboot proper to clearly delineate shiny Open Source
from ugly blobs.
Since we don't want to impose blobs on users who really don't need them,
that repository is only downloaded and checked out on explicit request.

Handling submodules
-------------------
For the most part, submodules should be automatically checked out on the
first execution of the coreboot Makefile.

To manually fetch all repositories (eg. when you want to prepare the tree
for archiving, or to use it without network access), run

    $ git submodule update --init --checkout

This also checks out the binaries below `3rdparty/`

Mirroring coreboot
------------------
When running a coreboot mirror to checkout from, for full operation, you
should also mirror the blobs and nvidia-cbootimage repository, and place
them in the same directory as the coreboot repository mirror.

That is, when residing in coreboot's repository, `cd ../blobs.git`
should move you to the blobs repository.

With that, no matter what the URL of your coreboot repository is, the
git client (of a sufficiently new version) is able to pick up the other
repositories transparently.

Minimum requirements
--------------------
git needs to be able to handle relative paths to submodule repositories,
and it needs to know about non-automatic submodules.

For these features, we require git version 1.7.6.1 or newer.
