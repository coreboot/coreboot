#!/usr/bin/env sh

nix-build '<nixpkgs/nixos>' -A config.system.build.isoImage -I nixos-config=console.nix
