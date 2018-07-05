# Docker container to create coreboot status reports

This container expects input in `/data-in/{coreboot,board-status}.git` and
emits two files `/data-out/{board-status.html,kconfig-options.html}`.
