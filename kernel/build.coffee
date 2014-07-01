mainSources = [
  'stdlib'
  'utilities'
  'memory'
  'module'
  'entry'
  'syscall'
  'arch/general'
  'scheduler/general'
  'scheduler/user'
  'scheduler/internal'
]

includes = [
  'src/stdlib/h'
  'src/stdlib/hpp'
  'src'
  '../dependencies/anslabs/include'
  '../dependencies/analloc2/include'
  '../dependencies/anlock/src'
]

deps = ['analloc2/src', 'anlock/src']

module.exports = (Finder, Makefile) ->
  arch = process.env['TARGET_ARCH']
  if not (arch in ['x64'])
    throw new Error 'unknown architecture: ' + arch
  
  scheduler = process.env['TARGET_SCHEDULER']
  if not (scheduler in ['fifo'])
    throw new Error 'unknown scheduler: ' + scheduler
  
  finder = new Finder()
  for source in mainSources
    finder.search 'src/' + source
  for dep in deps
    finder.search '../dependencies/' + dep
  finder.search 'src/arch/' + arch
  finder.search 'src/scheduler/' + scheduler
  
  includes = includes.concat [
    'src/arch/' + arch + '/include'
    'src/scheduler/' + scheduler + '/include'
  ]
  
  objdir = process.env['OBJDIR']
  if not objdir?
    throw new Error 'missing OBJDIR variable'
  return new Makefile finder, includes, objdir
