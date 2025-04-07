# Git Commit messages

There are a number of different recommendations for git commit
messages, so this is another one.

It's expected that coreboot contributors already generally understand
the idea of writing good git commit messages, so this is not trying
to go over everything again. There are other tutorials that cover that.

- [Linux Kernel tip tree Handbook](https://www.kernel.org/doc/html/latest/process/maintainer-tip.html#patch-subject)
- [How to write a Git Commit Message](https://cbea.ms/git-commit/)

## Line length

- The subject line should be <= 65 characters, with an absolute maximum
  of 72 characters
- Prose in the commit message should be <= 72 characters
- If reflowing prose to 72 characters can reduce the length of the
  commit message by 2 or more lines, please reflow it. Using the entire
  72 characters on a line when reasonable is recommended, but not
  required.
- Non-prose text in the body in the commit message does not need to be
  wrapped at 72 characters. Examples: URLs, compiler output, or poetry.

## Both Subject & body

- Use the present tense. (The for loop exits too early, so ...", not
 "The for loop exited too early, so ...").
- When using acronyms, make sure they're explained in the commit
  message itself or in the [acronyms list](https://doc.coreboot.org/acronyms.html).

## Commit message Subject Line

- Start the subject line with a prefix denoting the area of the change.
  Often part of the path can be used by that. Looking through existing
  commit messages summaries with `git log --oneline ...` gives a good
  idea. Because this prefix takes space used by the rest of the subject,
  it should be kept short while still uniquely describing the area.
  - Don't include `src/`
  - Use abbreviations where possible:
    - mb: mainboard
    - vc: vendorcode
- Don't end the subject line with a period.
- Use the imperative mood. ("Fix whitespace", not "whitespace fixes").

## Commit Message Body

- Make sure the problem being solved by the commit is described. While
  it may be obvious to the committer, it may not be obvious to others.
- Reference other commits with either CB:XXXXX or a 10 character hash
  and the subject.
- When using a URL in a commit message, use archive.org when possible.
  URLs often get changed or go stale, so this keeps them stable.
- Make sure that all changes in a patch are addressed in the commit
  message.
- A TEST= tag is highly recommended, but not required. This lets people
  know whether you tested it by booting the board or just by compiling.
- All but the most trivial of patches should generally have a body.
- A BUG= tag can be added when the author wants to indicate that the
  patch is or is not related to a bug. This can be either in coreboot's
  issue tracker, or a private issue tracker.
    - `BUG=b:####` is used by the Google internal issue tracker.
    - `BUG=chromium:####` indicates the Chromium public tracker at
      https://issues.chromium.org/
    - `BUG=CID ####` can be used to indicate coverity error fixes.
    - `BUG=https://...` can be used to link directly to a public
      tracker.
- The BRANCH= tag is used in cases where a patch needs to be added to a
  specific downstream branch. This is never required by the coreboot
  project.

## Footers

- The Signed-off-by line is required (Jenkins forces this).
- The Change ID is required (Gerrit forces this.)
- When adding a patch that has already gone through another git or
  gerrit, the footers from those previous commits may be added, but
  keep the list reasonable.
