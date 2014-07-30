{
  'target_defaults': {
    'default_configuration': 'Debug',
    'cflags': ['-std=gnu++11', '-Wall', '-Wextra'],
    'cxxflags': ['-std=gnu++11', '-Wall', '-Wextra'],
    'include_dirs': ['<(DEPTH)'],

    'configurations': {
      'Debug': {
        'cflags+': ['-g', '-O0'],
        'cxxflags+': ['-g', '-O0'],
        'msvs_settings': {
          'VCCLCompilerTool': {
            'Optimization': '0', # /Od
            'RuntimeLibrary': '1', # /MTd
          },
          'VCLinkerTool': {
            'LinkIncremental': '2',
          },
        },
        'xcode_settings+': {
          'GCC_OPTIMIZATION_LEVEL': '0', # -O0
          'OTHER_CFLAGS': '-std=gnu++11',
        },
      }, # Debug
      'Release': {
        'cflags+': ['-O3', '-DNDEBUG'],
        'cxxflags+': ['-O3', '-DNDEBUG'],
        'msvs_settings':{
          'VCCLCompilerTool': {
            'Optimization': '2', # /O2
            'InlineFunctionExpansion': '2',
            'RuntimeLibrary': '0', # /MT
          },
          'VCLinkerTool': {
            'LinkIncremental': '1',
            'OptimizeReferences': '2',
          },
        },
        'xcode_settings+': {
          'GCC_OPTIMIZATION_LEVEL': '3', # -O3
          'OTHER_CFLAGS': '-std=gnu++11',
        },
      }, # Release
    },
    'variables': {
      'component%': 'static_library',
    },
  }, # target_defaults

  'targets': [
    {
      'target_name': 'emulator',
      'type': 'none',
      'dependencies': [
        'emulator/emulator.gyp:emulator_all'
      ]
    },
    {
      'target_name': 'emulator_tests',
      'type': 'none',
      'dependencies': [
        'emulator/emulator_test.gyp:emulator_test',
        'lisp_to_gdb/compiler.gyp:compiler_all'
      ]
    }
  ]
}
