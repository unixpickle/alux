module.exports = (Finder, Makefile) ->
  arch = process.env['TARGET_ARCH']
  if not (arch in ['x64'])
    throw new Error 'unknown architecture: ' + arch

  # add things to this array as needed
  mainSources = [
    'main'
    'stdlib'
    'memory'
    'tests'
  ]

  finder = new Finder()
  for source in mainSources
    finder.search 'src/' + source
  finder.search 'src/arch/' + arch

  includes = [
    'src'
    'src/arch/' + arch + '/include'
    'src/stdlib/hpp'
    'src/stdlib/h'
  ]

  objdir = process.env['OBJDIR']
  if not objdir?
    throw new Error 'missing OBJDIR variable'
  return new Makefile finder, includes, objdir
