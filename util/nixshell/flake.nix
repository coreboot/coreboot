{
  nixpkgs,
  ...
}:
let
  inherit (nixpkgs) lib;
  systems = [
    "x86_64-linux"
    "aarch64-linux"
  ];
  forAllSystems = f: lib.genAttrs systems (system: f system);
  perSystem =
    system:
    let
      pkgs = import nixpkgs { inherit system; };
      corebootToolchains = builtins.filter (x: lib.isDerivation x) (
        builtins.attrValues pkgs.coreboot-toolchain
      );
      sdkPackages = with pkgs; [
        autoconf
        autoconf-archive
        automake
        bison
        bzip2
        cacert
        ccache
        clang-tools
        cmake
        cscope
        curlMinimal
        diffutils
        dtc
        e2fsprogs
        flex
        freetype
        gawk
        gettext
        gitMinimal
        gnat
        gnumake
        gnutls
        go
        imagemagick
        lcov
        libtool
        libuuid
        meson
        ncurses
        ninja
        openssh
        openssl
        p7zip
        parted
        patch
        pciutils
        perl
        pkg-config
        python3
        qemu
        rustup
        sharutils
        shellcheck
        universal-ctags
        unifont
        unzip
        util-linux
        wget
        xz
        zlib
      ];
    in
    {
      devShells.default = pkgs.mkShellNoCC {
        packages = sdkPackages ++ corebootToolchains;
        inputsFrom = [ pkgs.linux_latest ];

        # Firmware build systems own their flags. In particular, EDK2 uses
        # -Werror together with -Wno-format, which conflicts with nixpkgs'
        # injected -Wformat-security hardening flag.
        hardeningDisable = [ "all" ];
      };
    };
in
{
  devShells = forAllSystems (system: (perSystem system).devShells);
}
