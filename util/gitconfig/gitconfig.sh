#!/bin/bash
if [ ! -d .git ]; then
	echo "Error: Not in a git repository"
	exit 1
fi
mkdir -p .git/hooks
for hook in commit-msg pre-commit ; do
	if [ util/gitconfig/$hook -nt .git/hooks/$hook ] || \
		[ ! -x .git/hooks/$hook ]; then
		sed -e "s,%MAKE%,remake,g" util/gitconfig/$hook > .git/hooks/$hook
		chmod +x .git/hooks/$hook
	fi
done
# Now set up the hooks for 3rdparty/
for hooks in .git/modules/{3rdparty/blobs,libhwbase,libgfxinit}/hooks; do
	if [ -d $hooks ]; then
		if [ util/gitconfig/commit-msg -nt $hooks/commit-msg ] || \
			[ ! -x $hooks/commit-msg ]; then
			sed -e "s,%MAKE%,remake,g"
				util/gitconfig/commit-msg > $hooks/commit-msg
			chmod +x $hooks/commit-msg
		fi
	fi
done
for d in 3rdparty/{blobs,libhwbase,libgfxinit}; do
	if [ -d $d ]; then
		(cd $d || exit 1
		 git config remote.origin.push HEAD:refs/for/master)
	fi
done
git config remote.origin.push HEAD:refs/for/master
git config alias.sup '!git submodule update --remote --rebase && git submodule update --init --checkout'
git config alias.sup-destroy '!git submodule deinit --force "$(git rev-parse --show-toplevel)"; git submodule init && git submodule update --checkout'
(git config --includes user.name >/dev/null && git config --includes user.email >/dev/null) || (printf 'Please configure your name and email in git:\n\n git config --global user.name "Your Name Comes Here"\n git config --global user.email your.email@example.com\n'; exit 1)
