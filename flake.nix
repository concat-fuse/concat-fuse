{
  description = "Virtually concatenate files";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-23.05";
    flake-utils.url = "github:numtide/flake-utils";

    tinycmmc.url = "github:grumbel/tinycmmc";
    tinycmmc.inputs.nixpkgs.follows = "nixpkgs";
    tinycmmc.inputs.flake-utils.follows = "flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils, tinycmmc }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in {
        packages = rec {
          default = concat-fuse;

          concat-fuse = pkgs.stdenv.mkDerivation {
            pname = "concat-fuse";
            version = "0.3.2";

            src = ./.;

            nativeBuildInputs = with pkgs; [
              cmake
              pkg-config
            ] ++ [
              tinycmmc.packages.${system}.default
            ];

            buildInputs = with pkgs; [
              fuse
              gtest
              minizip
              openssl
              python3
              zlib
            ];
           };
        };
      }
    );
}
