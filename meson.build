project('hello-triangle', 'c', version : '0.1.0', default_options : ['c_std=c11'])

cc = meson.get_compiler('c')
dependencies = [
    dependency('gbm'),
    dependency('egl'),
    dependency('glesv2'),
    dependency('libdrm'),
    cc.find_library('m', required : false)
]

sources = files(
  'common.c',
  'cube-smooth.c',
  'drm-common.c',
  'drm-legacy.c',
  'esTransform.c',
  'cube-renderer.c',
)

executable('cube-renderer',
    sources,
    dependencies : dependencies,
    install : true
)
