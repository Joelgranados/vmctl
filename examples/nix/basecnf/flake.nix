# SPDX-License-Identifier: GPL-3.0-or-later
# Copied from https://github.com/metaspace/run-kernel
# Original from Andreas Hindborg <a.hindborg@kernel.org>

{
  description = "A system expression for run-kernel";
  inputs = { nixpkgs.url = "nixpkgs/nixos-24.11"; };

  outputs = { nixpkgs, ... }: {
    nixosConfigurations.vm = nixpkgs.lib.nixosSystem {
      system = "x86_64-linux";
      modules = [ ./configuration.nix ];
    };
  };
}
