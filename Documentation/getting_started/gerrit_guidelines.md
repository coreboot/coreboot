coreboot Gerrit Etiquette and Guidelines
========================================

The following rules are the requirements for behavior in the coreboot
codebase in gerrit. These have mainly been unwritten rules up to this
point, and should be familiar to most users who have been active in
coreboot for a period of time. Following these rules will help reduce
friction in the community.

Note that as with many rules, there are exceptions. Some have been noted
in the 'More Detail' section. If you feel there is an exception not listed
here, please discuss it in the mailing list to get this document  updated.
Don't just assume that it's okay, even if someone on IRC says it is.


Summary
-------
These are the expectations for committing, reviewing, and submitting code
into coreboot git and gerrit. While breaking individual rules may not have
immediate consequences, the coreboot leadership may act on repeated or
flagrant violations with or without notice.

* Don't violate the licenses.
* Let non-trivial patches sit in a review state for at least 24 hours
before submission.
* Try to coordinate with platform maintainers when making changes to
platforms.
* If you give a patch a -2, you are responsible for giving concrete
recommendations for what could be changed to resolve the issue the patch
addresses.
* Don't modify other people's patches without their consent.
* Be respectful to others when commenting.
* Don’t submit patches that you know will break other platforms.


More detail
-----------
* Don't violate the licenses. If you're submitting code that you didn't
write yourself, make sure the license is compatible with the license of the
project you're submitting the changes to. If you’re submitting code that
you wrote that might be owned by your employer, make sure that your
employer is aware and you are authorized to submit the code. For
clarification, see the Developer's Certificate of Origin in the coreboot
[Signed-off-by policy](https://www.coreboot.org/Development_Guidelines#Sign-off_Procedure).

* Let non-trivial patches sit in a review state for at least 24 hours
before submission. Remember that there are coreboot developers in timezones
all over the world, and everyone should have a chance to contribute.
Trivial patches would be things like whitespace changes or spelling fixes.
In general, small changes that don’t impact the final binary output. The
24-hour period would start at submission, and would be restarted at any
update which significantly changes any part of the patch. Patches can be
'Fast-tracked' and submitted in under this 24 hour with the agreement of at
least 3 +2 votes.

* Do not +2 patches that you authored or own, even for something as trivial
as whitespace fixes. When working on your own patches, it’s easy to
overlook something like accidentally updating file permissions or git
submodule commit IDs. Let someone else review the patch. An exception to
this would be if two people worked in the patch together. If both +2 the
patch, that is acceptable, as each is giving a +2 to the other's work.

* Try to coordinate with platform maintainers and other significant
contributors to the code when making changes to platforms. The platform
maintainers are the users who initially pushed the code for that platform,
as well as users who have made significant changes to a platform. To find
out who maintains a piece of code, please use util/scripts/maintainers.go
or refer to the original author of the code in git log.

* If you give a patch a -2, you are responsible for giving concrete
recommendations for what could be changed to resolve the issue the patch
addresses. If you feel strongly that a patch should NEVER be merged, you
are responsible for defending your position and listening to other points
of view. Giving a -2 and walking away is not acceptable, and may cause your
 -2 to be removed by the coreboot leadership after no less than a week. A
 notification that the -2 will be removed unless there is a response will
 be sent out at least 2 days before it is removed.

* Don't modify other people's patches unless you have coordinated this with
the owner of that patch. Not only is this considered rude, but your changes
could be unintentionally lost. An exception to this would be for patches
that have not been updated for more than 90 days. In that case, the patch
can be taken over if the original author does not respond to requests for
updates. Alternatively, a new patch can be pushed with the original
content, and both patches should be updated to reference the other.

* Be respectful to others when commenting on patches. Comments should
be kept to the code, and should be kept in a polite tone. We are a
worldwide community and English is a difficult language. Assume your
colleagues are intelligent and do not intend disrespect. Resist the urge to
retaliate against perceived verbal misconduct, such behavior is not
conducive to getting patches merged.

* Don’t submit code that you know will break other platforms. If your patch
affects code that is used by other platforms, it should be compatible with
those platforms. While it would be nice to update any other platforms, you
must at least provide a path that will allow other platforms to continue
working.


Recommendations for gerrit activity
-----------------------------------
These guidelines are less strict than the ones listed above. These are more
of the “good idea” variety. You are requested to follow the below
guidelines, but there will probably be no actual consequences if they’re
not followed. That said, following the recommendations below will speed up
review of your patches, and make the members of the community do less work.

* Each patch should be kept to one logical change, which should be
described in the title of the patch. Unrelated changes should be split out
into separate patches. Fixing whitespace on a line you’re editing is
reasonable. Fixing whitespace around the code you’re working on should be a
separate ‘cleanup’ patch. Larger patches that touch several areas are fine,
so long as they are one logical change. Adding new chips and doing code
cleanup over wide areas are two examples of this.

* Test your patches before submitting them to gerrit. It's also appreciated
if you add a line to the commit message describing how the patch was
tested. This prevents people from having to ask whether and how the patch
was tested. Examples of this sort of comment would be ‘TEST=Built
platform’ or ‘Tested by building and booting platform’.  Stating that the
patch was not tested is also fine, although you might be asked to do some
testing in cases where that would be reasonable.

* Take advantage of the lint tools to make sure your patches don’t contain
trivial mistakes. By running ‘make gitconfig’, the lint-stable tools are
automatically put in place and will test your patches before they are
committed. As a violation of these tools will cause the jenkins build test
to fail, it’s to your advantage to test this before pushing to gerrit.

* Don't submit patch trains longer than around 20 patches unless you
understand how to manage long patch trains. Long patch trains can become
difficult to handle and tie up the build servers for long periods of time
if not managed well. Rebasing a patch train over and over as you fix
earlier patches in the train can hide comments, and make people review the
code multiple times to see if anything has changed between revisions. When
pushing long patch trains, it is recommended to only push the full patch
train once - the initial time, and only to rebase three or four patches at
a time.

* Run 'make what-jenkins-does' locally on patch trains before submitting.
This helps verify that the patch train won’t tie up the jenkins builders
for no reason if there are failing patches in the train. For running
parallel builds, you can specify the number of cores to use by setting the
the CPUS environment variable. Example:
        make what-jenkins-does CPUS=8

* Use a topic when pushing a train of patches. This groups the commits
together so people can easily see the connection at the top level of
gerrit. Topics can be set for individual patches in gerrit by going into
the patch and clicking on the icon next to the topic line. Topics can also
be set when you push the patches into gerrit. For example, to push a set of
commits with the the i915-kernel-x60 set, use the command:
        git push origin HEAD:refs/for/master/i915-kernel-x60

* If one of your patches isn't ready to be merged, make sure it's obvious
that you don't feel it's ready for merge yet. The preferred way to show
this is by marking in the commit message that it’s not ready until X. The
commit message can be updated easily when it’s ready to be pushed.
Examples of this are "WIP: title" or "[NEEDS_TEST]: title".  Another way to
mark the patch as not ready would be to give it a -1 or -2 review, but
isn't as obvious as the commit message. These patches can also be pushed as
drafts as shown in the next guideline.

* When pushing patches that are not for submission, these should be marked
as such. This can be done in the title ‘[DONOTSUBMIT]’, or can be pushed as
draft commits, so that only explicitly added reviewers will see them. These
sorts of patches are frequently posted as ideas or RFCs for the community
to look at. To push a draft, use the command:
        git push origin HEAD:refs/for/master%private,wip

* Respond to anyone who has taken the time to review your patches, even if
it's just to say that you disagree. While it may seem annoying to address a
request to fix spelling or 'trivial' issues, it’s generally easy to handle
in gerrit’s built-in editor. If you do use the built-in editor, remember to
get that change to your local copy before re-pushing. It's also acceptable
to add fixes for these sorts of comments to another patch, but it's
recommended that that patch be pushed to gerrit before the initial patch
gets submitted.

* Consider breaking up large individual patches into smaller patches
grouped by areas. This makes the patches easier to review, but increases
the number of patches. The way you want to handle this is a personal
decision, as long as each patch is still one logical change.

* If you have an interest in a particular area or mainboard, set yourself
up as a ‘maintainer’ of that area by adding yourself to the MAINTAINERS
file in the coreboot root directory. Eventually, this should automatically
add you as a reviewer when an area that you’re listed as a maintainer is
changed.

* Submit mainboards that you’re working on to the board-status repo. This
helps others and shows that these mainboards are currently being
maintained. At some point, boards that are not up to date in the
board-status repo will probably end up getting removed from the coreboot
master branch.

* Abandon patches that are no longer useful, or that you don’t intend to
keep working on to get submitted.

* Bring attention to patches that you would like reviewed. Add reviewers,
ask for reviewers on IRC or even just rebase it against the current
codebase to bring it to the top of the gerrit list. If you’re not sure who
would be a good reviewer, look in the MAINTAINERS file or git history of
the files that you’ve changed, and add those people.

* Familiarize yourself with the coreboot [commit message
guidelines](https://www.coreboot.org/Git#Commit_messages), before pushing
patches. This will help to keep annoying requests to fix your commit
message to a minimum.

* If there have been comments or discussion on a patch, verify that the
comments have been addressed before giving a +2. If you feel that a comment
is invalid, please respond to that comment instead of just ignoring it.

* Be conscientious when reviewing patches. As a reviewer who approves (+2)
a patch, you are responsible for the patch and the effect it has on the
codebase. In the event that the patch breaks things, you are expected to
be actively involved in the cleanup effort. This means you shouldn’t +2 a
patch just because you trust the author of a patch - Make sure you
understand what the implications of a patch might be, or leave the review
to others. Partial reviews, reviewing code style, for example, can be given
a +1 instead of a +2. This also applies if you think the patch looks good,
but may not have the experience to know if there may be unintended
consequences.

* If there is still ongoing discussion to a patch, try to wait for a
conclusion to the discussion before submitting it to the tree. If you feel
that someone is just bikeshedding, maybe just state that and give a time
that the patch will be submitted if no new objections are raised.

* When working with patch trains, for minor requests it’s acceptable to
create a fix addressing a comment in another patch at the end of the patch
train. This minimizes rebases of the patch train while still addressing the
request. For major problems where the change doesn’t work as intended or
breaks other platforms, the change really needs to go into the original
patch.

* When bringing in a patch from another git repo, update the original
git/gerrit tags by prepending the lines with 'Original-'.  Marking
the original text this way makes it much easier to tell what changes
happened in which repository. This applies to these lines, not the actual
commit message itself:
        Commit-Id:
        Change-Id:
        Signed-off-by:
        Reviewed-on:
        Tested-by:
        Reviewed-by:
The script 'util/gitconfig/rebase.sh' can be used to help automate this.
Other tags such as 'Commit-Queue' can simply be removed.


Expectations contributors should have
-------------------------------------
* Don't expect that people will review your patch unless you ask them to.
Adding other people as reviewers is the easiest way. Asking for reviews for
individual patches in the IRC channel, or by sending a direct request to an
individual through your favorite messenger is usually the best way to get a
patch reviewed quickly.

* Don't expect that your patch will be submitted immediately after getting
a +2. As stated previously, non-trivial patches should wait at least 24
hours before being submitted. That said, if you feel that your patch or
series of patches has been sitting longer than needed, you can ask for it
to be submitted on IRC, or comment that it's ready for submission in the
patch. This will move it to the top of the list where it's more likely to
be noticed and acted upon.

* Reviews are about the code. It's easy to take it personally when someone
is criticising your code, but the whole idea is to get better code into our
codebase. Again, this also applies in the other direction: review code,
criticize code, but don’t make it personal.


Requests for clarification and suggestions for updates to these guidelines
should be sent to the coreboot mailing list at <coreboot@coreboot.org>.
