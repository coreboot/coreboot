with import <nixpkgs> {};

stdenvNoCC.mkDerivation {
	name = "coreboot-documentation";

	buildInputs = [
		git
		gnumake
		python3Packages.recommonmark
		python3Packages.sphinx_rtd_theme
		sphinx
	];
}
