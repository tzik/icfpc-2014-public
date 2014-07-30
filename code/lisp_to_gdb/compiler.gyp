{
  'targets': [
    {
      'target_name': 'compiler_all',
      'type': 'none',
      'dependencies': [
        'lisp2gdb',
        'gdb2gcc',
        'util',
      ]
    },
    {
      'target_name': 'gdb2gcc',
      'type': 'executable',
      'dependencies': [
        '<(DEPTH)/emulator/emulator.gyp:gcc',
      ],
      'sources': [
        'gdb2gcc.cc',
      ]
    },
    {
      'target_name': 'lisp2gdb',
      'type': 'executable',
      'dependencies': [
        'parser',
        'converter',
        'preprocessor',
        'tokenizer',
      ],
      'sources': [
        'lisp2gdb_main.cc',
        'lisp2gdb.h',
      ]
    },
    {
      'target_name': 'converter',
      'type': 'static_library',
      'dependencies': ['util'],
      'sources': [
        'converter.cc',
        'lisp2gdb.h',
      ]
    },
    {
      'target_name': 'parser',
      'type': 'static_library',
      'sources': [
        'parser.cc',
        'parser.h',
      ]
    },
    {
      'target_name': 'preprocessor',
      'type': 'static_library',
      'sources': [
        'preprocessor.cc',
        'preprocessor.h',
      ]
    },
    {
      'target_name': 'tokenizer',
      'type': 'static_library',
      'sources': [
        'tokenizer.cc',
        'tokenizer.h',
      ]
    },
    {
      'target_name': 'util',
      'type': 'static_library',
      'sources': [
        'util.cc',
        'util.h',
      ]
    },
  ]
}
