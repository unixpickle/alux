fs = require 'fs'
path = require 'path'

Makefile = require '../script-lib/makefile'
SourceFinder = require '../script-lib/source-finder'

arch = process.env['TARGET_ARCH']
if not arch in ['x64']
  console.error 'Unknown architecture: ' + arch
  process.exit 1

# add things to this array as needed
mainSources = [
  'main'
  'stdlib'
]

finder = new SourceFinder()
for source in mainSources
  finder.search 'src/' + source
finder.search 'src/arch/' + arch

includes = [
  'src'
  'src/arch/' + arch + '/include'
  'src/stdlib/hpp'
]

objdir = process.env['OBJDIR']
file = new Makefile finder, includes, objdir
file.generateRules()
result = file.encode()
fs.writeFileSync path.join(objdir, 'Makefile'), result
