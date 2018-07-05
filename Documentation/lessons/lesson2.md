# coreboot Lesson 2: Submitting a patch to coreboot.org

## Part 1: Setting up an account at coreboot.org

If you already have an account, skip to Part 2.

Otherwise, go to <https://review.coreboot.org> in your preferred web browser.
Select **Register** in the upper right corner.

Select the appropriate sign-in. For example, if you have a Google account,
select **Google OAuth2** (gerrit-oauth-provider plugin)".**Note:** Your
username for the account will be the username of the account you used to
sign-in with. (ex. your Google username).

## Part 2a: Set up RSA Private/Public Key

If you prefer to use an HTTP password instead, skip to Part 2b.

For the most up-to-date instructions on how to set up SSH keys with Gerrit go to
<https://gerrit-documentation.storage.googleapis.com/Documentation/2.14.2/user-upload.html#configure_ssh)>
and follow the instructions there. Then, skip to Part 3.

Additionally, that section of the Web site provides explanation on starting
an ssh-agent, which may be particularly helpful for those who anticipate
frequently uploading changes.

If you instead prefer to have review.coreboot.org specific instructions,
follow the steps below. Note that this particular section may have the
most up-to-date instructions.

If you do not have an RSA key set up on your account already (as is the case
with a newly created account), follow the instructions below; otherwise,
doing so could overwrite an existing key.

In the upper right corner, select your name and click on **Settings**.
Select **SSH Public Keys** on the left-hand side.

In a terminal, run "ssh-keygen" and confirm the default path ".ssh/id_rsa".

Make a passphrase -- remember this phrase. It will be needed whenever you use
this RSA Public Key. **Note:** You might want to use a short password, or
forego the password altogether as you will be using it very often.

Open "id_rsa.pub", copy all contents and paste into the textbox under
"Add SSH Public Key" in the https://review.coreboot.org webpage.

## Part 2b: Setting up an HTTP Password

Alternatively, instead of using SSH keys, you can use an HTTP password. To do so,
after you select your name and click on **Settings** on the left-hand side, rather
than selecting **SSH Public Keys**, select **HTTP Password**.

Click **Generate Password**. This should fill the "Password" box with a password. Copy
the password, and add the following to your $HOME/.netrc file:

	machine review.coreboot.org login YourUserNameHere password YourPasswordHere

where YourUserNameHere is your username, and YourPasswordHere is the password you
just generated.

## Part 3: Clone coreboot and configure it for submitting patches

Go to the **Projects** tab in the upper left corner and select **List**.
From the dropdown menu that appears, select "coreboot".

If you are using SSH keys, select **ssh** from the tabs under "Project coreboot"
and run the command that appears. This should prompt you for your id_rsa passphrase,
if you previously set one.

If you are using HTTP, instead, select **http** from the tabs under "Project coreboot"
and run the command that appears

After it finishes cloning, "cd coreboot" will take you into the local
git repository. Run "make gitconfig" to set up the hooks and configurations.
For example, you will be asked to run the following commands to set your
username and email.

	git config --global user.name "Your Name"
	git config --global user.email "Your Email"

## Part 4: Submit a commit

An easy first commit to make is fixing existing checkpatch errors and warnings
in the source files. To see errors that are already present, build the files in
the repository by running 'make lint' in the coreboot directory. Alternatively,
if you want to run 'make lint' on a specific directory, run:

	for file in $(git ls-files | grep src/amd/quadcore); do \
	util/lint/checkpatch.pl --file $file --terse; done

where <filepath> is the filepath of the directory (ex. src/cpu/amd/car).

Any changes made to files under the src directory are made locally,
and can be submitted for review.

Once you finish making your desired changes, use the command line to stage
and submit your changes. An alternative and potentially easier way to stage
and submit commits is to use git cola, a graphical user interface for git. For
instructions on how to do so, skip to Part 4b.

## Part 4a: Using the command line to stage and submit a commit

To use the command line to stage a commit, run

	git add <filename>

where `filename` is the name of your file.

To commit the change, run

	git commit -s

**Note:** The -s adds a signed-off-by line by the committer. Your commit should be
signed off with your name and email (i.e. **Your Name** **<Your Email>**, based on
what you set with git config earlier).

Running git commit first checks for any errors and warnings using lint. If
there are any, you must go back and fix them before submitting your commit.
You can do so by making the necessary changes, and then staging your commit again.

When there are no errors or warnings, your default text editor will open.
This is where you will write your commit message.

The first line of your commit message is your commit summary. This is a brief
one-line description of what you changed in the files using the template
below:

<filepath>: Short description
*ex. cpu/amd/pi/00630F01: Fix checkpatch warnings and errors*
**Note:** It is good practice to use present tense in your descriptions
and do not punctuate your summary.

Then hit Enter. The next paragraph should be a more in-depth explanation of the
changes you've made to the files. Again, it is good practice to use present
tense.
*ex.  Fix space prohibited between function name and open parenthesis,
line over 80 characters, unnecessary braces for single statement blocks,
space required before open brace errors and warnings.*

When you have finished writing your commit message, save and exit the text
editor. You have finished committing your change. If, after submitting your
commit, you wish to make changes to it, running "git commit --amend" allows
you to take back your commit and amend it.

When you are done with your commit, run 'git push' to push your commit to
coreboot.org. **Note:** To submit as a draft, use
'git push origin HEAD:refs/drafts/master' Submitting as a draft means that
your commit will be on coreboot.org, but is only visible to those you add
as reviewers.

## Part 4b: Using git cola to stage and submit a commit

If git cola is not installed on your machine, see
https://git-cola.github.io/downloads.html for download instructions.

After making some edits to src files, rather than run "git add," run
'git cola' from the command line. You should see all of the files
edited under "Modified".

In the textbox labeled "Commit summary" provide a brief one-line
description of what you changed in the files according to the template
below:

<filepath>: Short description
*ex. cpu/amd/pi/00630F01: Fix checkpatch warnings and errors*
**Note:** It is good practice to use present tense in your descriptions
and do not punctuate your short description.

In the larger text box labeled 'Extended description...' provide a more
in-depth explanation of the changes you've made to the files. Again, it
is good practice to use present tense.
*ex.  Fix space prohibited between function name and open parenthesis,
line over 80 characters, unnecessary braces for single statement blocks,
space required before open brace errors and warnings.*

Then press Enter two times to move the cursor to below your description.
To the left of the text boxes, there is an icon with an downward arrow.
Press the arrow and select "Sign Off." Make sure that you are signing off
with your name and email (i.e. **Your Name** **<Your Email>**, based on what
you set with git config earlier).

Now, review each of your changes and mark either individual changes or
an entire file as Ready to Commit by marking it as 'Staged'. To do
this, select one file from the 'Modified' list. If you only want to
submit particular changes from each file, then highlight the red and
green lines for your changes, right click and select 'Stage Selected
Lines'. Alternatively, if an entire file is ready to be committed, just
double click on the file under 'Modified' and it will be marked as
Staged.

Once the descriptions are done and all the edits you would like to
commit have been staged, press 'Commit' on the right of the text
boxes.

If the commit fails due to persisting errors, a text box will appear
showing the errors. You can correct these errors within 'git cola' by
right-clicking on the file in which the error occurred and selecting
'Launch Diff Tool'. Make necessary corrections, close the Diff Tool and
'Stage' the corrected file again. It might be necessary to refresh
'git cola' in order for the file to be shown under 'Modified' again.
Note: Be sure to add any other changes that haven't already been
explained in the extended description.

When ready, select 'Commit' again. Once all errors have been satisfied
and the commit succeeds, move to the command line and run 'git push'.
**Note:** To submit as a draft, use 'git push origin HEAD:refs/drafts/master'
Submitting as a draft means that your commit will be on coreboot.org, but is
only visible to those you add as reviewers.

## Part 5: Getting your commit reviewed

Your commits can now be seen on review.coreboot.org if you select “My”
and click on “Changes” and can be reviewed by others. Your code will
first be reviewed by build bot (Jenkins), which will either give you a warning
or verify a successful build; if so, your commit will receive a +1. Other
users may also give your commit +1.  For a commit to be merged, it needs
to receive a +2.**Note:** A +1 and a +1 does not make a +2. Only certain users
can give a +2.

## Part 6 (optional): bash-git-prompt

To help make it easier to understand the state of the git repository
without running 'git status' or 'git log', there is a way to make the
command line show the status of the repository at every point. This
is through bash-git-prompt.

Instructions for installing this are found at:
https://github.com/magicmonty/bash-git-prompt
**Note:** Feel free to search for different versions of git prompt,
as this one is specific to bash.

Alternatively, follow the instructions below:
Run the following two commands in the command line:

    cd
    git clone https://github.com/magicmonty/bash-git-prompt.git .bash-git-prompt --depth=1

**Note:** cd will change your directory to your home directory, so the
git clone command will be run there.

Finally, open the ~/.bashrc file and append the following two lines:

    GIT_PROMPT_ONLY_IN_REPO=1
    source ~/.bash-git-prompt/gitprompt.sh

Now, whenever you are in a git repository, it will continuously display
its state.

There also are additional configurations that you can change depending on your
preferences. If you wish to do so, look at the "All configs for .bashrc" section
on https://github.com/magicmonty/bash-git-prompt. Listed in that section are
various lines that you can copy, uncomment and add to your .bashrc file to
change the configurations. Example configurations include avoid fetching remote
status, and supporting versions of Git older than 1.7.10.

## Appendix: Miscellaneous Advice

### Updating a commit after running git push:

Suppose you would like to update a commit that has already been pushed to the
remote repository. If the commit you wish to update is the most recent
commit you have made, after making your desired changes, stage the files
(either using git add or in git cola), and amend the commit. To do so,
if you are using the command line, run "git commit --amend." If you are
using git cola, click on the gear icon located on the upper left side under
**Commit** and select **Amend Last Commit** in the drop down menu. Then, stage
the files you have changed, commit the changes, and run git push to push the
changes to the remote repository. Your change should be reflected in Gerrit as
a new patch set.

If, however, the commit you wish to update is not the most recent commit you
have made, you will first need to checkout that commit. To do so, find the
URL of the commit on <https://review.coreboot.org> and go to that page; if
the commit is one that you previously pushed, it can be found by selecting
**My** and then **Changes** in the upper left corner. To checkout this commit,
in the upper right corner, click on **Download**, and copy the command listed
next to checkout by clicking **Copy to clipboard**. Then, run the copied
command in your coreboot repository. Now, the last commit should be the most
recent commit to that patch; to update it, make your desired changes, stage
the files, then amend and push the commit using the instructions in the above
paragraph.