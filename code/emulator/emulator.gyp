{
  'targets': [
    {
      'target_name': 'emulator_all',
      'type': 'none',
      'dependencies': [
        # Put all targets here.
        'gcc',
        'ghc',
        'game',
        'character',
        'lambdaman',
        'ghost',
        'fake_lambdaman',
        'fake_ghost',
        'vm_lambdaman',
        'emulator_main',
        'util',
        'pos',
      ]
    },
    {
      'target_name': 'game',
      'type': 'static_library',
      'dependencies': [
        'map',
        'ghost',
        'lambdaman',
        'gcc',
        'ghc',
      ],
      'sources': [
        'game.cc',
        'game.h',
      ],
    },
    {
      'target_name': 'gcc',
      'type': 'static_library',
      'sources': [
        'gcc_vm.cc',
        'gcc_vm.h',
      ]
    },
    {
      'target_name': 'ghc',
      'type': 'static_library',
      'sources': [
        'ghc.cc',
        'ghc.h',
      ]
    },
    {
      'target_name': 'map',
      'type': 'static_library',
      'dependencies': ['pos'],
      'sources': [
        'map.cc',
        'map.h',
      ],
    },
    {
      'target_name': 'character',
      'type': 'static_library',
      'dependencies': ['pos'],
      'sources': [
        'character.cc',
        'character.h',
      ],
    },
    {
      'target_name': 'lambdaman',
      'type': 'static_library',
      'dependencies': ['character'],
      'sources': [
        'lambdaman.cc',
        'lambdaman.h',
      ],
    },
    {
      'target_name': 'fake_lambdaman',
      'type': 'static_library',
      'dependencies': ['lambdaman'],
      'sources': [
        'fake_lambdaman.cc',
        'fake_lambdaman.h',
      ],
    },
    {
      'target_name': 'vm_lambdaman',
      'type': 'static_library',
      'dependencies': ['lambdaman', 'gcc'],
      'sources': [
        'vm_lambdaman.cc',
        'vm_lambdaman.h',
      ],
    },
    {
      'target_name': 'ghost',
      'type': 'static_library',
      'dependencies': ['character'],
      'sources': [
        'ghost.cc',
        'ghost.h',
      ],
    },
    {
      'target_name': 'fake_ghost',
      'type': 'static_library',
      'dependencies': [
        'ghost',
        'util',
      ],
      'sources': [
        'fake_ghost.cc',
        'fake_ghost.h',
      ],
    },
    {
      'target_name': 'emulator_main',
      'type': 'executable',
      'dependencies': [
        'game',
        'fake_ghost',
        'fake_lambdaman',
        'vm_lambdaman',
      ],
      'sources': [
        'emulator_main.cc',
      ],
    },
    {
      'target_name': 'pos',
      'type': 'static_library',
      'sources': [
        'pos.cc',
        'pos.h',
      ],
    },
    {
      'target_name': 'util',
      'type': 'static_library',
      'sources': [
        'util.cc',
        'util.h',
      ],
    },
  ]
}



