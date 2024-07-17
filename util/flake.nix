{
    description = ( "Devshell for python serial GUI" );

    inputs = {
        nixpkgs = { url = "github:NixOS/nixpkgs/nixpkgs-unstable"; };
    };

    outputs = { self, nixpkgs }:
    let
        system = "x86_64-linux"; # should also work for "aarch64-linux" "aarch64-darwin" "x86_64-darwin"
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
        devShells.${system}.default = pkgs.mkShell {
            buildInputs = dependencies;
        };
    };
}
