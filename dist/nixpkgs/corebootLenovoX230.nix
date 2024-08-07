{ self, ... }:

# Upstream-based packaging of Lenovo X230

{
	perSystem = { system, pkgs, inputs', self', ... }: {
		packages.corebootLenovoX230 = pkgs.stdenvNoCC.mkDerivation {
			pname = "corebootLenovoX230";
			version = "24.05";
			src =
		};

		# Declare for `nix run`
		apps.nixos-sinnenfreude-stable-install.program = self'.packages.nixos-sinnenfreude-stable-install;
	};
}
