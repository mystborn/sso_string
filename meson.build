project('sso_string', 'c')

c_comp = meson.get_compiler('c')
check_location = get_option('check_location')
alloc_header = get_option('alloc_header')
alloc_headers = get_option('alloc_headers')
alloc_sources = get_option('alloc_sources')
dirs_to_include = ['include'] + alloc_headers

include_files = include_directories(dirs_to_include)
sources = [ './src/sso_string.c' ] + alloc_sources

compiler_args = ['/DSSO_STRING_BUILD']

if alloc_header != ''
    compiler_args += ['/DSSO_STRING_MACROS="' + alloc_header + '"']
endif


sso_string = static_library(
    'sso_string',
    sources,
    c_args: compiler_args,
    include_directories: include_files,
    install: true,
    name_suffix: 'lib',
    name_prefix: ''
)

sso_string_shared = shared_library(
    'sso_string',
    sources,
    c_args: compiler_args,
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
    dirs_to_include += 'tests'
    if check_location != 'true'
        dirs_to_include += check_location + '/include'
    endif
    test_inc = include_directories(dirs_to_include)
    subdir('tests')
endif

subdir('examples')