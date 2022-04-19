project('memleak', 'c',
    default_options : ['c_std=c99',
        'c_args=-Wall',
        'b_ndebug=if-release',
        'b_sanitize=address',
        'b_lundef=false'],
    version : '0.0.0')

lsan_supp = meson.current_source_dir() + '/lsan.supp'
test_env = environment()
test_env.set('ASAN_OPTIONS', 'detect_leaks=1')
test_env.set('LSAN_OPTIONS', 'suppressions=' + lsan_supp)

memory_leak = executable('memory-leak',
    'memory-leak.c')

test('memory-leak', memory_leak, env: test_env, should_fail: true)

no_leak = executable('no-leak',
    'no-leak.c')

test('no-leak', no_leak, env: test_env, should_fail: false)

double_free = executable('double-free',
    'double-free.c')

test('testdoublefree', double_free, env: test_env, should_fail: true)

access_after_free = executable('access-after-free',
    'access-after-free.c')

test('access-after-free', access_after_free, env: test_env, should_fail: true)
