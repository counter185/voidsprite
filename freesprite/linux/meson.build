data_conf = configuration_data()

# Desktop file

desktop_file = install_data(
  'com.github.counter185.voidsprite.desktop',
  install_dir: join_paths(get_option('datadir'), 'applications')
)

# Metainfo/Appstream

metainfo_file = install_data(
  'com.github.counter185.voidsprite.metainfo.xml',
  install_dir: join_paths(get_option('datadir'), 'metainfo')
)

# Mime type for .voidsn files
if get_option('flatpak')
  install_data(
    'voidsn.xml',
    rename: 'com.github.counter185.voidsprite.xml',
    install_dir: join_paths(get_option('datadir'), 'mime', 'packages')
  )
else
  install_data(
    'voidsn.xml',
    install_dir: join_paths(get_option('datadir'), 'mime', 'packages')
  )
endif

# Icon
install_data(
  'com.github.counter185.voidsprite.png',
  install_dir: join_paths(get_option('datadir'), 'icons', 'hicolor', '32x32', 'apps')
)

install_data(
  'com.github.counter185.voidsprite.png',
  install_dir: join_paths(get_option('datadir'), 'icons', 'hicolor', '256x256', 'apps')
)
