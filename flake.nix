{
  description = "coreboot development SDK";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = inputs: import ./util/nixshell/flake.nix inputs;
}
