project('prc_string', 'c')

c_comp = meson.get_compiler('c')
check_location = get_option('check_location')

include_files = include_directories(['include'])

if check_location != ''
    test_inc = include_directories(['include', check_location + '/include'])
    subdir('tests')
elif c_comp.compiles('#include <check.h>')
    test_inc = include_files
    subdir('tests')
endif