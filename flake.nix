{
    description = ( "Development environment for STM32" );

    inputs = {
        nixpkgs = { url = "github:NixOS/nixpkgs/nixpkgs-unstable"; };
    };

    outputs = { self, nixpkgs }:
    let
        systems = [ "x86_64-linux" "aarch64-darwin" "x86_64-darwin" ];
        forAll = nixpkgs.lib.attrsets.genAttrs;
    in
    {
        devShells = forAll systems (system:
        let
            pkgs = nixpkgs.legacyPackages.${system};
            dependencies = [
                pkgs.gcc-arm-embedded
                pkgs.openocd
                pkgs.minicom
            ];
        in
        {
            default = pkgs.mkShell {
                buildInputs = dependencies;
            };
        });
    };
}
