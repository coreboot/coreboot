{ self, inputs, ... }:

# Upstream-based packaging of Lenovo X230

{
	perSystem = { system, pkgs, inputs', self', ... }: {
		# FIXME(Krey): Build this on Clang/LLVM instead of GCC
		packages.corebootLenovoX230 = pkgs.stdenv.mkDerivation {
			pname = "corebootLenovoX230";
			version = "24.05";
			src = pkgs.fetchFromGitHub {
				owner = "coreboot";
				repo = "coreboot";
				rev = "24.05";
				hash = "sha256-Fq3tZje6QoMskxqWd61OstgI9Sj25yijf8S3LiTJuYc=";
				fetchSubmodules = true;
			};

			postPatch = builtins.concatStringsSep "\n" [
				"patchShebangs util/xcompile/xcompile"
				"patchShebangs util/genbuild_h/genbuild_h.sh"
			];

			nativeBuildInputs = [
				# NOTE(Krey): Nixpkgs-stable currently uses toolchain for 24.02 where unstable is at ours 24.05
				inputs.nixpkgs-unstable.legacyPackages.${system}.coreboot-toolchain.x64
				inputs.nixpkgs-unstable.legacyPackages.${system}.coreboot-toolchain.i386
				inputs.nixpkgs-unstable.legacyPackages.${system}.seabios
			];

			configurePhase = ''
				printf "%s/n" \
					"CONFIG_VENDOR_LENOVO=y" \
					"CONFIG_BOARD_LENOVO_X230=y" \
					"CONFIG_PYLOAD_EDK2=y" \
					"CONFIG_COMPILER_LLVM_CLANG=y" \
					"CONFIG_USE_X86_64_SUPPORT=y" \
					"CONFIG_LOCALVERSION=kreyren" \
					"CONFIG_USE_OPTION_TABLE=y" \
				> .config

				make olddefconfig # Generate default options to those supplemented

				echo "Using the following configuration:"
				cat .config

				# cp -r ${inputs.nixpkgs-unstable.legacyPackages.${system}.seabios}
			'';

			buildPhase = ''
				make
			'';

			installPhase = ''
				prefix="$out/share/coreboot"

				mkdir -p "$prefix"
				install -m 0444 build/coreboot.rom "$prefix"
			'';
		};

		# FIXME(Krey): Figure out how to declare for `nix run` to only produce the built binary and output an absolute path to it
		# apps.corebootLenovoX230.program = self'.packages.corebootLenovoX230;
	};
}
