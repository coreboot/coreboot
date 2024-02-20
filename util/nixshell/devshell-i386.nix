with import <nixpkgs> {};

pkgs.mkShell {
	name = "coreboot-devshell-i386";

	packages = [
		cacert
		gdb
		git
		qemu
	];

	buildInputs = [
		ncurses
		openssl
	];

	nativeBuildInputs = [
		coreboot-toolchain.i386
		pkg-config
		openssh
	];

	shellHook = ''
		# In Nix, stdenv sets a STRIP environment variable, which has conflict
		# with libpayload/Makefile.payload. Unset the variable.
		unset STRIP
	'';
}
