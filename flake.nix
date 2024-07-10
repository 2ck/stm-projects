{
    description = ( "Development environment for STM32" );

    inputs = {
        nixpkgs = { url = "github:NixOS/nixpkgs/nixpkgs-unstable"; };
    };

    outputs = { self, nixpkgs }:
    let
        system = "x86_64-linux"; # should also work for "aarch64-linux" "aarch64-darwin" "x86_64-darwin"
        pkgs = nixpkgs.legacyPackages.${system};
        dependencies = [
            pkgs.gcc-arm-embedded
            pkgs.openocd
            pkgs.minicom
        ];
    in
    {
        devShells.${system}.default = pkgs.mkShell {
            buildInputs = dependencies;
        };
    };
}
