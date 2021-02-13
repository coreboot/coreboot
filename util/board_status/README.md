General information on boot logs
================================

To gather good boot logs, you must set the debug level to Spew.

If your board uses CMOS configuration[1], you can do it like this:

```shell
nvramtool -w debug_level=Spew
```

Else you will need to have to set the default log level at compile
time, to do that go in `Console  --->` in make menuconfig, then set
`Default console log level` to SPEW.

Reference:
----------

[1] in make menuconfig you should have: `General setup  --->`, `[*] Use CMOS for configuration values`.
    This option is also known as `CONFIG_USE_OPTION_TABLE` in `Kconfig`.

Gather information through SSH
==============================

`board_status.sh` can gather information through ssh with the `-r`
option.

When using `-r <host>`, The script will attempt to log into
root@host.

In order for `-r <host>` to work, the script has to be able
to log into the remote host's root account, without having
to provide a password.

That can be achieved with the use of SSH keys and ssh-agent.

`board_status.sh` expects the remote host to have the following
programs in its path: `cbmem`, `dmesg`.

Boot log gathering through a serial port
========================================

When using `board_status.sh -s </dev/xxx>`, script starts by retrieving the
boot log through the serial port.

To produce such logs, power off the board, run `board_status.sh`
with the right arguments, power on the board.

At that point the logs will be displayed by `board_status.sh` as they
are produced by the board.

<kbd>Enter</kbd> will have to be pressed once the board has booted and is in a
state where the script is able to log into that board.

Publishing
==========

It expects the user to already have an account in
[coreboot's Gerrit instance](https://review.coreboot.org).

The `-u` switch will publish the results: It will make a `git patch` out
of the status information, that will be directly pushed in the
board-status repository.
