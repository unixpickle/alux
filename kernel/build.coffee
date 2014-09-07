path = require 'path'
anarch = require './dependencies/anarch/build.coffee'

mainSources = [
  'src/arch/all'
  'src/compiler'
  'src/memory'
  'src/tasks'
  'src/threads'
  'src/syscall'
  'src/scheduler'
  'src/ipc'
  'src/util'
  'dependencies/anidmap/src'
]

includes = [
  'dependencies/anidmap/include'
  'src/include'
]

module.exports = (Finder, Makefile, environment) ->
  arch = process.env['TARGET_ARCH']
  if not (arch in ['x64'])
    throw new Error 'unknown architecture: ' + arch;
  
  finder = new Finder()
  for source in mainSources
    finder.search source
  finder.search 'src/arch/' + arch
  
  # find anarch sources and use anarch's includes
  includes = includes.concat anarch finder
  
  objdir = path.join environment.root, 'build/objects'
  makefile = new Makefile finder, includes, objdir
  targetDefine = "-D__ANARCH_TARGET_#{arch.toUpperCase()}__"
  makefile.addFlags 'c', targetDefine
  makefile.addFlags 'cpp', targetDefine
  return makefile
