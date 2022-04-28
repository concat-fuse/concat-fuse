{
  description = "Virtually concatenate files";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-21.11";
    flake-utils.url = "github:numtide/flake-utils";

    tinycmmc.url = "gitlab:grumbel/cmake-modules";
    tinycmmc.inputs.nixpkgs.follows = "nixpkgs";
    tinycmmc.inputs.flake-utils.follows = "flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils, tinycmmc }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in rec {
        packages = flake-utils.lib.flattenTree {
          concat-fuse = pkgs.stdenv.mkDerivation {
            pname = "concat-fuse";
            version = "0.3.2";
            src = nixpkgs.lib.cleanSource ./.;
            nativeBuildInputs = [
              pkgs.cmake
              pkgs.pkg-config
              tinycmmc.defaultPackage.${system}
            ];
            buildInputs = [
              pkgs.fuse
              pkgs.gtest
              pkgs.minizip
              pkgs.openssl
              pkgs.python3
              pkgs.zlib
            ];
           };
        };
        defaultPackage = packages.concat-fuse;
      });
}
