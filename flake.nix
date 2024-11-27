{
	description = "NiXium's Coreboot";

	inputs = {
		# Release inputs
			nixpkgs-master.url = "github:nixos/nixpkgs/master";
			nixpkgs-staging-next.url = "github:nixos/nixpkgs/staging-next";
			nixpkgs-staging.url = "github:nixos/nixpkgs/staging";
			nixpkgs-unstable.url = "github:nixos/nixpkgs/nixos-unstable";

			nixpkgs.url = "https://flakehub.com/f/NixOS/nixpkgs/*.tar.gz";

			nixpkgs-23_05.url = "github:nixos/nixpkgs/nixos-23.05";
			nixpkgs-23_11.url = "github:nixos/nixpkgs/nixos-23.11";
			nixpkgs-24_05.url = "github:nixos/nixpkgs/nixos-24.05";

		# Principle inputs
			nixos-flake.url = "github:srid/nixos-flake";
			flake-parts.url = "github:hercules-ci/flake-parts";
			mission-control.url = "github:Platonic-Systems/mission-control";

			flake-root.url = "github:srid/flake-root";

		# DISKO
			disko = {
				url = "github:nix-community/disko";
				inputs.nixpkgs.follows = "nixpkgs";
			};
			disko-unstable = {
				url = "github:nix-community/disko";
				inputs.nixpkgs.follows = "nixpkgs-unstable";
			};
			disko-master = {
				url = "github:nix-community/disko";
				inputs.nixpkgs.follows = "nixpkgs-master";
			};

		nixos-generators = {
			url = "github:nix-community/nixos-generators";
			inputs.nixpkgs.follows = "nixpkgs";
		};
		nixos-generators-unstable = {
			url = "github:nix-community/nixos-generators";
			inputs.nixpkgs.follows = "nixpkgs-unstable";
		};
		nixos-generators-master = {
			url = "github:nix-community/nixos-generators";
			inputs.nixpkgs.follows = "nixpkgs-master";
		};
	};

	outputs = inputs @ { self, ... }:
		inputs.flake-parts.lib.mkFlake { inherit inputs; } {
			imports = [
				# ./lib # Implement libs
				# ./tasks # Include Tasks

				inputs.flake-root.flakeModule
				inputs.mission-control.flakeModule
			];

			# Set Supported Systems
			systems = [
				"x86_64-linux"
				"aarch64-linux"
				"riscv64-linux"
				"armv7l-linux"
			];

			perSystem = { system, config, inputs', ... }: {
				devShells.default = inputs.nixpkgs.legacyPackages.${system}.mkShell {
					name = "Coreboot-devshell";
					nativeBuildInputs = [
						# Shell
						inputs.nixpkgs.legacyPackages.${system}.bashInteractive # For terminal
						inputs.nixpkgs.legacyPackages.${system}.shellcheck # Linting of shell files

						# Nix
						inputs.nixpkgs.legacyPackages.${system}.nil # Needed for linting
						inputs.nixpkgs.legacyPackages.${system}.nixpkgs-fmt # Nixpkgs formatter

						# Utilities
						inputs.nixpkgs.legacyPackages.${system}.git # Working with the codebase
						inputs.nixpkgs.legacyPackages.${system}.nano # Editor to work with the codebase in cli

						inputs.nixos-generators.packages.${system}.nixos-generate

						inputs.nixpkgs.legacyPackages.${system}.fira-code # For liquratures in code editors

						inputs.disko.packages.${system}.disko-install
						inputs.disko.packages.${system}.disko

						# Build Dependencies
						inputs.nixpkgs.legacyPackages.${system}.ncurses
						inputs.nixpkgs.legacyPackages.${system}.pkg-config
						inputs.nixpkgs.legacyPackages.${system}.openssl
						inputs.nixpkgs.legacyPackages.${system}.coreboot-toolchain.i386
						inputs.nixpkgs.legacyPackages.${system}.libuuid
						inputs.nixpkgs.legacyPackages.${system}.imagemagick
						inputs.nixpkgs.legacyPackages.${system}.python312
						inputs.nixpkgs.legacyPackages.${system}.python312Packages.pyopenssl
						inputs.nixpkgs.legacyPackages.${system}.m4
						inputs.nixpkgs.legacyPackages.${system}.bison
						inputs.nixpkgs.legacyPackages.${system}.flex
					];
					inputsFrom = [
						config.mission-control.devShell
						config.flake-root.devShell
					];
					# Environmental Variables
					#VARIABLE = "value"; # Comment
				};

				formatter = inputs.nixpkgs.legacyPackages.${system}.nixpkgs-fmt;
			};
		};
}
