{
  'target_defaults': {
    'include_dirs': [
      './src/stdlib/headers',
      './src/stdlib/hpp'
    ],
    'cflags': [
      '-nostdinc',
      '-Wextra',
      '-Wno-long-long',
      '-fno-stack-protector',
      '-mno-red-zone'
    ]
  }, 
  'targets': [
    {
      'target_name': 'learnos.a',
      'type': 'static_library',
      'sources': [
        './src/platform/vm.cc'
      ]
    }
  ]
}
