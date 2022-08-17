with import <nixpkgs> {};

stdenvNoCC.mkDerivation {
	name = "coreboot-toolchain";

	buildInputs = [
		bison
		curl
		flex
		git
		gnat12
		gnumake
		patch
		zlib
	];

	shellHook = ''
		export SSL_CERT_FILE="/etc/ssl/certs/ca-certificates.crt"
	'';
}
