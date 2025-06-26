{
    description = ( "Devshell for python serial GUI" );

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
                (pkgs.python3.withPackages (pip3: [
                    pip3.pyserial
                    pip3.pyqt5
                    pip3.pillow
                ]))
                pkgs.libsForQt5.full

            ];
        in
        {
            default = pkgs.mkShell {
                buildInputs = dependencies;
            };
        });
    };
}
