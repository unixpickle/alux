if process.argv.length isnt 3
  console.log 'Usage: coffee writesize.coffee <file>'

die = (err) ->
  console.log err
  process.exit 1

fs = require 'fs'
file = process.argv[2];

fs.stat file, (err, info) ->
  die err if err?
  die 'not a file' if not info.isFile()
  size = info.size
  sizeBuffer = new Buffer ((size >> (8 * i)) & 0xff for i in [0...4])
  fs.open file, 'a+', (err, fd) ->
    fs.write fd, sizeBuffer, 0, 4, 0x20, (err) ->
      die err if err?
      fs.close fd, ->
        console.log 'done'
        process.exit 0
