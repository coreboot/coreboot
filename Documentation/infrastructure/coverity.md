# Coverity Scan for open source firmware

## What’s Coverity and Coverity Scan?

Coverity is a static analysis tool. It hooks into the build process
and in addition to the compiler creating object files, Coverity collects
information about the code. That data is then processed in a separate pass
to identify common programming errors, like out of bounds accesses in C.

Coverity Scan is an online service for Open Source projects providing this
analysis for free. The analysis pass is done on their servers and issues
can be handled in their [web UI](https://scan.coverity.com/).

The Scan service has some quotas based on code size to avoid overloading
the system, but even at one build per week, that’s usually good enough
because the identified issues still need to be triaged and fixed or they
will simply be re-identified next week.

### Triage?

The Web UI looks a bit like an issue tracker, even if it’s not a very
good one. It’s possible to mark identified issues as valid or invalid,
and annotate them with metadata which CLs fix them. The latter isn’t
strictly necessary because Coverity Scan simply marks issues it can’t
find anymore as fixed, but at times it helped identify issues that made
a comeback.

### Alternatives

There’s also clang’s scan-build, which is fully open-source, and
finds different issues. As such, it’s less of an alternative and more
of a complement.

There’s a regular run of that for coreboot but not for the other projects
hosted at coreboot.org.

One downside is that it emits a bunch of HTML to report on issues,
but there’s no interactivity (e.g. marking issues solved), no way
to merge multiple builds (e.g. multiple board builds of a single tree)
or a simple way to extract burndown charts and the like from that.

#### Looking for a project?

On the upside, it can emit the data in a machine readable format, so if
anybody needs a project, a scan-build web-frontend like Coverity Scan would
be feasible without having to go through scan-build’s guts, just by parsing
text files - plus all the stateful and web parts to build on top.

## Logging into Coverity Scan

Coverity Scan needs an account. It supports its own accounts and GitHub
OAuth.

Access to the dashboards needs approval: Request and you shall receive.

## coreboot & friends and Coverity Scan

coreboot, flashrom, Chromium EC and other projects of that family have
been made Coverity aware, that is, their build systems support building
with a custom compiler configuration passed in “just right” to enable
Coverity to add its hooks.

The public coreboot CI system at
[https://qa.coreboot.org/](https://qa.coreboot.org/) regularly does
builds with Coverity and sends them off to Coverity Scan.

Specifically, it covers:

* Chromium EC: [Coverity Scan site][crECCoverity] ([build job][crECBuildJob])
* coreboot: [Coverity Scan site][corebootCoverity] ([build job][corebootBuildJob]), [scan-build output][corebootScanBuild] ([build job][corebootScanBuildJob])
* em100: [Coverity Scan site][em100Coverity] ([build job][em100BuildJob])
* fcode-utils: [Coverity Scan site][fcodeUtilsCoverity] ([build job][fcodeUtilsBuildJob])
* flashrom: [Coverity Scan site][flashromCoverity] ([build job][flashromBuildJob])
* memtest86+: [Coverity Scan site][memtestCoverity] ([build job][memtestBuildJob])
* vboot: [Coverity Scan site][vbootCoverity] ([build job][vbootBuildJob])

[crECCoverity]: https://scan.coverity.com/projects/chromium-ec
[corebootCoverity]: https://scan.coverity.com/projects/coreboot
[em100Coverity]: https://scan.coverity.com/projects/em100
[fcodeUtilsCoverity]: https://scan.coverity.com/projects/fcode-utils
[flashromCoverity]: https://scan.coverity.com/projects/flashrom
[memtestCoverity]: https://scan.coverity.com/projects/memtest86
[vbootCoverity]: https://scan.coverity.com/projects/vboot

[corebootScanBuild]: https://www.coreboot.org/scan-build/

[crECBuildJob]: https://qa.coreboot.org/view/coverity/job/ChromeEC-Coverity/
[corebootBuildJob]: https://qa.coreboot.org/view/coverity/job/coreboot-coverity/
[corebootScanBuildJob]: https://qa.coreboot.org/view/coverity/job/coreboot_scanbuild/
[em100BuildJob]: https://qa.coreboot.org/view/coverity/job/em100-coverity/
[fcodeUtilsBuildJob]: https://qa.coreboot.org/view/coverity/job/fcode-utils-coverity/
[flashromBuildJob]: https://qa.coreboot.org/view/coverity/job/flashrom-coverity/
[memtestBuildJob]: https://qa.coreboot.org/view/coverity/job/memtest86plus-coverity/
[vbootBuildJob]: https://qa.coreboot.org/view/coverity/job/vboot-coverity/

Some projects (e.g. Chromium EC) build a different subset of boards on
each run, ensuring that everything is analyzed eventually. The downside
is that coverity issues pop up and disappear somewhat randomly as they
are discovered and go unnoticed in a later build.

More projects that are hosted on review.coreboot.org (potentially as a
mirror, like vboot and EC) could be served through that pipeline. Reach
out to {stepan,patrick,martin}@coreboot.org.
