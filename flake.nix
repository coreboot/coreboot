{
	description = "Coreboot";

	inputs = {
		# Release inputs
			nixpkgs.url = "https://flakehub.com/f/NixOS/nixpkgs/*.tar.gz";


		# Principle inputs
			nixos-flake.url = "github:srid/nixos-flake";
			flake-parts.url = "github:hercules-ci/flake-parts";
			mission-control.url = "github:Platonic-Systems/mission-control";

			flake-root.url = "github:srid/flake-root";

		nixos-generators = {
			url = "github:nix-community/nixos-generators";
			inputs.nixpkgs.follows = "nixpkgs";
		};
	};

	outputs = inputs @ { self, ... }:
		inputs.flake-parts.lib.mkFlake { inherit inputs; } {
			imports = [
				./tasks # Include Tasks
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
					name = "NiXium-devshell";
					nativeBuildInputs = [
						inputs.nixpkgs.legacyPackages.${system}.bashInteractive # For terminal
						inputs.nixpkgs.legacyPackages.${system}.nil # Needed for linting
						inputs.nixpkgs.legacyPackages.${system}.nixpkgs-fmt # Nixpkgs formatter
						inputs.nixpkgs.legacyPackages.${system}.git # Working with the codebase
						inputs.nixpkgs.legacyPackages.${system}.nano # Editor to work with the codebase in cli

						inputs.nixpkgs.legacyPackages.${system}.fira-code # For liquratures in code editors

						inputs.nixpkgs.legacyPackages.${system}.flashrom

						inputs.nixos-generators.packages.${system}.nixos-generate

						inputs.nixpkgs.legacyPackages.${system}.coreboot-toolchain.x64
						inputs.nixpkgs.legacyPackages.${system}.clang
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
