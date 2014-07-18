path = require 'path'

mainSources = [
  'src/entry/all'
]

includes = [
  '../dependencies/anarch/include'
  '../dependencies/anarch/src/stdlib/hpp'
  '../dependencies/anarch/src/stdlib/h'
  '../dependencies/anarch/dependencies/ansa/include'
]

module.exports = (Finder, Makefile, environment) ->
  arch = process.env['TARGET_ARCH']
  if not (arch in ['x64'])
    throw new Error 'unknown architecture: ' + arch;
  
  finder = new Finder()
  for source in mainSources
    finder.search source
  finder.search 'src/entry/' + arch
  
  objdir = path.join environment.root, 'build/objects'
  return new Makefile finder, includes, objdir
