if process.argv.length isnt 4
  console.log 'Usage: coffee set-length.coffee <alux.bin> <program.bin>'
  process.exit 1

die = (err) ->
  console.log err
  process.exit 1

fs = require 'fs'
mainBin = process.argv[2]
programBin = process.argv[3]

main = ->
  fs.stat mainBin, (err, mainInfo) ->
    die err if err?
    die 'not a file: ' + mainBin if not mainInfo.isFile()
    fs.stat programBin, (err, progInfo) ->
      die err if err?
      die 'not a file: ' + programBin if not progInfo.isFile()
      handleStats mainInfo.size, progInfo.size

handleStats = (mainSize, progSize) ->
  die 'main source too small' if mainSize < 8
  fs.open mainBin, 'a+', (err, fd) ->
    die err if err?
    sizeBuffer = new Buffer ((progSize >> (8 * i)) & 0xff for i in [0...4])
    fs.write fd, sizeBuffer, 0, 4, mainSize - 8, (err) ->
      die err if err?
      fs.close fd, ->
        process.exit 0

main()