{
  'targets': [
    {
      'target_name': 'emulator_test',
      'type': 'none',
      'dependencies': [
        # Put all targets here.
        'gcc_test',
        'map_main',
      ]
    },
    {
      'target_name': 'gcc_test',
      'type': 'executable',
      'dependencies': [
        'emulator.gyp:gcc',
      ],
      'sources': [
        'gcc_vm_test.cc',
      ]
    },
    {
      'target_name': 'map_main',
      'type': 'executable',
      'dependencies': [
        'emulator.gyp:map',
      ],
      'sources': [
        'map_main.cc',
      ],
    },
  ]
}
