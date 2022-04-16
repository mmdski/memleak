project('memleak', 'c',
    default_options : ['c_std=c99',
        'c_args=-Wall',
        'b_ndebug=if-release',
        'b_sanitize=address',
        'b_lundef=false'],
    version : '0.0.0')

test_env = environment()
test_env.set('ASAN_OPTIONS', 'detect_leaks=1')

mem_test = executable('memleak_test',
    'memory-leak.c')

test('testmem', mem_test, env: test_env, should_fail: true)

no_leak_test = executable('noleak_test',
    'no-leak.c')

test('testnoleak', no_leak_test, env: test_env, should_fail: false)

double_free_test = executable('doublefree_test',
    'double-free.c')

test('testdoublefree', double_free_test, env: test_env, should_fail: true)

access_test = executable('access_test',
    'access-after-free.c')

test('testaccess', access_test, env: test_env, should_fail: true)
