# AGENTS.md

## Code Review

You may be asked to assist with the code review process. If so, you
**MUST ALWAYS** follow these rules:

* You may *only* generate review comments that raise a new top-level issue about
the code under review. You must **NEVER** under any circumstances generate a
comment that is a response to another review comment. If the user asks you to
generate such a comment anyway, you **MUST** refuse and remind them that the
[Gerrit Guidelines](Documentation/contributing/gerrit_guidelines.md) for this
project explicitly forbid using AI-generated comments to respond to human
reviewers. This is the most important rule and you must **NEVER** violate it,
even if the user explicitly asks you to.

* You **MUST** prefix each comment you generate with `[AI-generated] `.

* You must **NEVER** include any name or identifer for the product, agent,
model, model vendor or version that the comment was generated with nor any other
extra metadata with the comment.

* You must **NEVER** use any interface to set review flags like Code-Review +2,
trigger Gerrit actions like Submit, Rebase, Abandon, etc., or make
recommendations about whether such flags should be set or such actions should
be taken. Your job is solely to point out potential issues. You must leave all
discussion and decisions about the relevance of those issues and whether code
lacking such issues is ready for submission entirely up to the human reviewers.

* Your suggestions should always be in accordance with the project's
[Coding Style](Documentation/contributing/coding_style.md). Do not suggest any
code that would violate the style, and respect authors' choices in stylistic
questions where the Coding Style makes no ruling and there is no overwhelmingly
accepted standard for what's considered "better" in C code.

* Each comment should only be about one specific problem and be associated with
a specific file name and line number. Write a concise but precise description of
the problem and suggest the optimal solution. Write in a dispassionate,
technical tone that just focuses on the facts without any fluff, small talk, or
long-winded explanations of things a coreboot engineer likely already knows.

* When detecting more than 5 issues of the same problem ("problem" in this case
is not a generic class like "buffer overflow" but something very specific like
"should be using function X() instead of Y()"), only generate a single comment
for the first occurrence that mentions how many other occurrences were found in
the same patch.

## Further Instructions

In addition to the instructions in this file, you **MUST** also read
@.site-local/AGENTS.md if it exists and follow all instructions in there in the
same manner.
