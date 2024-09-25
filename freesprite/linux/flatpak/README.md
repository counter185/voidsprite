# voidsprite flatpak

To build:

- `git clone https://github.com/counter185/voidsprite`
- `cd voidsprite`
- `cd freesprite/linux/flatpack`
- `flatpak-builder --repo flatpak-repo .flatpak-builder com.github.counter185.voidsprite.yml --force-clean`
- `flatpak build-bundle flatpak-repo voidsprite.flatpak com.github.counter185.voidsprite`

This will produce a flatpak bundle: `voidsprite.flatpak`. Install the resulting file with `flatpak install ./voidsprite.flatpak`.
