#!/usr/bin/env sh

config=$1

if [ -z "$config" ] || [ ! -f "$config" ]; then
	echo "Usage: $0 <config.nix>"
	echo "No config given. Exiting."
	exit 1
fi

nix-build '<nixpkgs/nixos>' \
	-A config.system.build.isoImage \
	-I nixos-config=$config \
	-I nixpkgs=https://github.com/NixOS/nixpkgs/archive/refs/heads/nixos-22.05.tar.gz
