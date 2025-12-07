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
foreach size : ['16x16', '32x32', '64x64', '128x128', '256x256', '512x512']
  install_data(
    'icons' / size + '.png',
    rename: 'com.github.counter185.voidsprite.png',
    install_dir: join_paths(get_option('datadir'), 'icons', 'hicolor', size, 'apps')
  )
endforeach

# Thumbnailer
install_data(
  'voidsprite.thumbnailer',
  install_dir: join_paths(get_option('datadir'), 'thumbnailer')
)
install_data(
  'voidsprite_thumbnailer',
  rename: 'thumbnailer',
  install_dir: join_paths(get_option('datadir'), 'voidsprite')
)
