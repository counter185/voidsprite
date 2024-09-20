project('libtga', 'c',
  default_options: ['warning_level=3', 'c_std=c99']
)

lib = static_library('tga', files('tga.c', 'tgaread.c', 'tgawrite.c'))
libtga_dep = declare_dependency(link_with: lib)
