# shellcheck shell=sh # POSIX
set +u # Do not fail on nounset as we use command-line arguments for logic

direnv reload

case "$?" in
	0) : ;; # Success
	*) echo "reload exitted with an unknown exit code: $?"
esac
