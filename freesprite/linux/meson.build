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
install_data(
  'voidsn.xml',
  install_dir: join_paths(get_option('datadir'), 'mime', 'packages')
)

# Icon
install_data(
  'com.github.counter185.voidsprite.png',
  install_dir: join_paths(get_option('datadir'), 'icons', 'hicolor', '32x32', 'apps')
)
