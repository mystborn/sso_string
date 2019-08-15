project('sso_string', 'c')

c_comp = meson.get_compiler('c')
check_location = get_option('check_location')

include_files = include_directories(['include'])

# This allows for other projects to use this as a subproject.
sso_string_dep = declare_dependency(
    include_directories: include_files
)

if check_location != ''
    test_inc = include_directories(['include', check_location + '/include'])
    subdir('tests')
elif c_comp.compiles('#include <check.h>')
    test_inc = include_files
    subdir('tests')
endif