{
  'target_defaults': {
    'include_dirs': [
      './src/stdlib/cppheaders',
      './src/stdlib/cheaders',
      './src/'
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
      'target_name': 'os',
      'type': 'static_library',
      'sources': [
        './src/platform/general/vm.cc'
      ],
      
      'conditions': [
        ['PLATFORM=="x64"',
          {
            'sources': [
              './src/platform/x64/vm-x64.cc',
              './src/platform/x64/failure-x64.cc'
            ]
          }
        ] ### PLATFORM=x64
      ]
      
    }
  ]
}
