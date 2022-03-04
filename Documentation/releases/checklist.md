```eval_rst
:orphan:
```

# coreboot Release Process

This document describes our release process and all prerequisites to implement
it successfully.

## Purpose of coreboot releases
Our releases aren't primarily a vehicle for code that is stable across all
boards: The logistics of testing the more than 100 boards that are spread out
all continents (except Antarctica, probably) on a given tree state are
prohibitive for project of our size.

Instead, the releases are regular breakpoints that serve multiple purposes:
They support cooperation between multiple groups (corporations or otherwise)
in that it's easier to keep source trees synchronized based on a limited set
of commits. They allow a quick assessment of the age of any given build or
source tree based on its git version (4.8-1234 was merged into master a few
months after 4.8, which came out in April 2018. 4.0-21718's age is harder to
guess).

And finally we use releases to as points in time where we remove old code:
Once we decide that a certain part of coreboot gets in the way of future
development, we announce on the next release that we intend to remove that
part - and everything that depends on it - after the following release.
So removing feature FOO will be announced in release X for release
X+1. The first commit after X+1 is fair game for such removal.

Together with our 6 months release horizon, this provides time to plan
any migrations necessary to keep older boards in the tree by bringing
them up to current standards.

## Needed credentials & authorizations
* Website access is required to post the release files to the website.
* IRC admin access is required to update the topic.
* Git access rights are needed to post the tag.
* Blog post access is needed to do the blog post.
* A PGP key is required to sign the release tarballs and git tag.

This set of required credentials implies that releases can only be done
by a coreboot admin.

## When to release
Releases are done roughly on a 6-month schedule, ideally around end
of April and end of October (can be a bit earlier or delay into May
or November).

We initially followed a 3 month release schedule, but we found that to
be more frequent than was needed, so we scaled it back to twice a year.

## Checklist
### ~2 weeks prior to release
- [ ] Announce upcoming release to mailing list, ask people to test and
      to update release notes.

### ~1 week prior to release
- [ ] Send reminder email to mailing list, ask for people to test,
      and to update the release notes.
- [ ] Update the topic in the IRC channel with the date of the upcoming
      release.
- [ ] If there are any deprecations announced for the following release,
      make sure that a list of currently affected boards and chipsets is
      part of the release notes.
- [ ] Finalize release notes as much as possible
- [ ] Prepare release notes template for following release
- [ ] Update `Documentation/releases/index.md`
- [ ] Run `util/vboot_list/vboot_list.sh` script to update the list of
      boards supported by vboot.

### Day of release
- [ ] Select a commit ID to base the release upon, announce to IRC,
      ask for testing.
- [ ] Test the commit selected for release.
- [ ] Submit release notes
- [ ] Create new release notes doc template for the next version.
- [ ] Fill in the release date, remove "Upcoming release" and other filler
      from the current release notes.
- [ ] Run release script.
- [ ] Test the release from the actual release tarballs.
- [ ] Push signed Tag to repo.
- [ ] Announce that the release tag is done on IRC.
- [ ] Upload release files to web server.
- [ ] Also extract the release notes and place them on the web server.
- [ ] Upload crossgcc sources to web server.
- [ ] Update download page to point to files, push to repo.
- [ ] Write and publish blog post with release notes.
- [ ] Update the topic in the IRC channel that the release is done.
- [ ] Announce the release to the mailing list.

## Pre-Release tasks
Announce the upcoming release to the mailing list release 2 weeks ahead
of the planned release date.

The announcement should state the planned release date, point to the
release notes that are in the making and ask people to test the hardware
they have to make sure it's working with the current master branch,
from which the release will ultimately be derived from.

People should be encouraged to provide additions to the release notes.

The final release notes will reside in coreboot's Documentation/releases
directory, so asking for additions to that through the regular Gerrit
process works as well. Note that git requires lots of conflict resolution
on heavily edited text files though.

Frequently, we will want to wait until particular things are in the
release.  Once those are in, you can select the commit ID that you want
to use for your release.  For the 4.6 release, we waited until we had
time to do the release, then pulled in a few patches that we wanted
to have in the release.  The release was based on the final of those
patches to be pulled in.

When a release candidate has been selected, announce the commit ID to
the #coreboot IRC channel, and request that it get some testing, just
to make sure that everything is sane.

## Generate the release
After the commit for the release has been selected and verified, run the
release script - util/release/build-release.  This will download a new
tree, checkout the commit that you specified, download the submodules,
create a tag, then generate and sign the tarballs.

Be prepared to type in your PGP key’s passphrase.

````
usage: util/release/build-release <version> [commit id] [username] [gpg key id]
Tags a new coreboot version and creates a tar archive

version:    New version name to tag the tree with
commit id:  check out this commit-id after cloning the coreboot tree
username:   clone the tree using ssh://USERNAME - defaults to https://
gpg key id: used to tag the version, and generate a gpg signature
````

After running the script, you should have a new directory for the release,
along with 4 files - 2 tarballs, and 2 signature files.

````
drwxr-xr-x   9 martin martin      4096 Apr 30 19:57 coreboot-4.6
-rw-r--r--   1 martin martin  29156788 Apr 30 19:58 coreboot-4.6.tar.xz
-rw-r--r--   1 martin martin       836 Apr 30 19:58 coreboot-4.6.tar.xz.sig
-rw-r--r--   1 martin martin   5902076 Apr 30 19:58 coreboot-blobs-4.6.tar.xz
-rw-r--r--   1 martin martin       836 Apr 30 19:58 coreboot-blobs-4.6.tar.xz.sig
````

Here’s the command that was used to generate the 4.6 release:
````
% util/release/build-release 4.6 db508565 Gaumless 3E4F7DF7
````

## Test the release from the tarballs
* Run “make what-jenkins-does” and verify that everything is building.
* Build and test qemu
  ````
  cp configs/config.emulation_qemu_x86_i440fx .config; make olddefconfig; make
  qemu-system-x86_64 -bios build/coreboot.rom -serial stdio
  ````
* Build and test any other platforms you can.
* Compare the directory from the tarballs to the coreboot repo to make sure nothing went wrong.
* Push the tag to git

A good tag will look like this:
````
% git show 4.6
tag 4.6
Tagger: Martin Roth <martinroth@google.com>
Date:   Sun Apr 30 19:48:38 2017 -0600

coreboot version 4.6
-----BEGIN PGP SIGNATURE-----
Version: GnuPG v1

iQIcBAABCQAGBQJZBpP2AAoJEBl5bCs+T333xfgQAKhilfDTzqlr3MLJC4VChbmr
...
678e0NzyWsyqU1Vx2rdFdLANx6hghH1R7E5ybzHHUQrhb55BoEsnQMU1oS0npnT4
dwfLho1afk0ZLPUU1JFW
=25y8
-----END PGP SIGNATURE-----

commit db508565d2483394b709654c57533e55eebace51 (HEAD, tag: 4.6, origin/master, origin/HEAD)
...
````

When you used the script to generate the release, a signed tag was generated in the
tree that was downloaded. From the coreboot-X.Y tree, just run: `git push origin X.Y`.
In case you pushed the wrong tag already, you have to force push the new one.

You will need write access for tags to the coreboot git repo to do this.

## After the release is tagged in git
Announce that the release has been tagged - this lets people know that
they should update their trees to grab the new tag.  Until they do this,
the version number in build.h will still be based on the previous tag.

Copy the tarballs and .sig files generated by the script to
the coreboot server, and put them in the release directory at
`/srv/docker/www.coreboot.org-staticfiles/releases/`

````
% sha256sum -b coreboot-*.tar.xz > sha256suma.txt # Update the sha256sum file
% diff sha256sum.txt sha256suma.txt # make sure that the two new files are present (and that nothing else has changed)
% mv sha256suma.txt sha256sum.txt
````

People can now see the release tarballs on the website at
<https://www.coreboot.org/releases/>

The downloads page is the official place to download the releases from, and it needs to be updated with links to the new release tarballs and .sig files. It can be found at <https://review.coreboot.org/cgit/homepage.git/tree/downloads.html>

Here is an example commit to change it: <https://review.coreboot.org/c/homepage/+/19515>

## Upload crossgcc sources
Sometimes the source files for older revisions of
crossgcc disappear. To deal with that we maintain a mirror at
<https://www.coreboot.org/releases/crossgcc-sources/> where we host the
sources used by the crossgcc scripts that are part of coreboot releases.

Run

````
% util/crossgcc/buildgcc -u
````

This will output the set of URLs that the script uses to download the
sources. Download them yourself and copy them into the crossgcc-sources
directory on the server.

## After the release is complete
Post the release notes on <https://blogs.coreboot.org>

## Making a branch
At times we will need to create a branch, generally for patch fixes.
When making a branch, do NOT name it the same as the release tag: X.Y - this creates trouble when trying to check it out, as git can’t tell whether you want the tag or the branch.
Instead, name it X.Y\_branch: `git checkout 4.8; git checkout -b 4.8_branch; git push origin 4.8_branch`

You can then cherry-pick changes and push them up to the branch:
````
git cherry-pick c6d134988c856d0025153fb885045d995bc8c397
git push origin HEAD:refs/for/4.8_branch
````
