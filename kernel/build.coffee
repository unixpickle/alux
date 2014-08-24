path = require 'path'

mainSources = [
  'src/arch/all'
  'src/compiler'
  'src/memory'
  'src/tasks'
  'src/syscall'
  'src/scheduler'
  'src/identifiers'
]

includes = [
  'dependencies/anarch/include'
  'dependencies/anarch/src/stdlib/hpp'
  'dependencies/anarch/src/stdlib/h'
  'dependencies/anarch/dependencies/ansa/include'
  'dependencies/anarch/dependencies/analloc2/include'
]

module.exports = (Finder, Makefile, environment) ->
  arch = process.env['TARGET_ARCH']
  if not (arch in ['x64'])
    throw new Error 'unknown architecture: ' + arch;
  
  finder = new Finder()
  for source in mainSources
    finder.search source
  finder.search 'src/arch/' + arch
  
  objdir = path.join environment.root, 'build/objects'
  makefile = new Makefile finder, includes, objdir
  targetDefine = "-D__ANARCH_TARGET_#{arch.toUpperCase()}__"
  makefile.addFlags 'c', targetDefine
  makefile.addFlags 'cpp', targetDefine
  return makefile
