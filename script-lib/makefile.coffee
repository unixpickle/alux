path = require 'path'

class Makefile
  constructor: (@finder, @includes, @outRoot) ->
    @rules = {}
    @projRoot = process.env['PROJECT_ROOT']
  
  generateRules: ->
    @_generateWithVars @finder.cSources, 'CC', 'CFLAGS'
    @_generateWithVars @finder.cppSources, 'CXX', 'CXXFLAGS'
    @_generateWithVars @finder.nasmSources, 'NASM', 'NASMFLAGS', false
  
  encode: ->
    result = 'all: ' + (x for own x of @rules).join(' ') + '\n\n'
    for own aPath, info of @rules
      result += aPath + ': ' + info.deps + '\n'
      result += '\t' + info.command + '\n\n'
    return result
  
  _addRule: (outPath, deps, command) ->
    return if @rules[outPath]?
    @rules[outPath] =
      deps: deps
      command: command
  
  _generateWithTemplate: (files, template) ->
    for file in files
      output = path.join @outRoot, @_simplifyName file
      output = output.replace /\.[a-z]*$/, '.o'
      input = path.join @projRoot, file
      command = template.replace('%OUT', output).replace '%IN', input
      @_addRule output, input, command
  
  _generateWithVars: (files, compVar, flagsVar, extraFlags = true) ->
    compiler = process.env[compVar]
    flags = process.env[flagsVar]
    if extraFlags
      includes = @_getIncludes()
      command = "#{compiler} -c #{flags} #{includes} %IN -o %OUT"
    else
      command = "#{compiler} #{flags ? ''} %IN -o %OUT"
    @_generateWithTemplate files, command
  
  _simplifyName: (name) ->
    return name.replace /\//g, '_'
  
  _getIncludes: ->
    val = process.env['INCLUDES'] ? ''
    for aPath in @includes
      val += ' ' if val.length
      val += '-I' + path.join @projRoot, aPath
    return val

module.exports = Makefile
