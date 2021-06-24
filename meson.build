project('sso_string', 'c')

c_comp = meson.get_compiler('c')
check_location = get_option('check_location')

include_files = include_directories(['include'])
sources = [ './src/sso_string.c' ]

sso_string = static_library(
    'sso_string',
    sources,
    c_args: ['/DSSO_STRING_BUILD'],
    include_directories: include_files,
    install: true,
    name_suffix: 'lib',
    name_prefix: ''
)

sso_string_shared = shared_library(
    'sso_string',
    sources,
    c_args: ['/DSSO_STRING_BUILD'],
    version: '1.1.0',
    include_directories: include_files,
    install: true,
)

# This allows for other projects to use this as a subproject.
sso_string_dep = declare_dependency(
    include_directories: include_files,
    link_with: sso_string_shared
)

if check_location != ''
    test_inc = include_directories(['include', check_location + '/include'])
    subdir('tests')
elif c_comp.compiles('#include <check.h>')
    test_inc = include_files
    subdir('tests')
endif

subdir('examples')