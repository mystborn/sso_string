if check_location != 'true'
    check_lib = check_location + '/lib'
    
    compat = c_comp.find_library('compat',
        required: true,
        dirs: check_lib)

    check = c_comp.find_library('check',
        required: true,
        dirs: check_lib)

    deps = [compat, check]
else
    check = c_comp.find_library('check',
        required: true)

    deps = [check]
endif

link_args = []

if c_comp.get_id() == 'msvc' and get_option('buildtype') == 'release'
    link_args += '/NODEFAULTLIB:MSVCRTD'
endif

compiler_args += '/DSSO_STRING_MACROS="../tests/test_macros.h"'

string_tests = executable('string_tests',
    ['../src/sso_string.c', './tests.c'],
    include_directories: test_inc,
    c_args: compiler_args,
    dependencies: deps,
    link_args: link_args
)

test('String Tests', string_tests)