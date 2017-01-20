cembed = {}

map = (fn, a, ...) ->
   t = {}
   for _ in *a
     _i = _index_0
     t[_i] = fn(_,...)
   return t

totable = (str) ->
  tab = {}
  str\gsub '.',(c) -> table.insert tab,c
  tab

cembed.basename = (str) ->
  name = str\gsub '(.*/)(.*)', '%2'
  name

string.rstrip = (del='') =>
  n = #@
  while n > 0 and @\find('^%s', n) do n = n - 1
  @\sub(1, (@\sub(1, n))\find(del, -1)-1)

string.join = (arr) =>
  ret = ''
  for str in *arr do
    ret ..= str
  @ .. ret

cembed.fmt = (fmt,dic) ->
  for k,v in pairs dic
    fmt = fmt\gsub '{'.. k ..'}', tostring v
  return fmt

cembed.make_array = (data) ->
  i = {0}
  fn = (x) ->
    x = tostring(string.byte x) .. ','
    if i[1] + #x > 78
      i[1] = #x
      x = '\n' .. x
    else
      i[1] += #x
    return x
  '{'.. ""\join(map(fn,totable data))\rstrip(',') ..'}'

cembed.safename = (filename) ->
  string.gsub basename(filename)\lower!, '[^a-z0-9]', '_'

cembed.process = (...) ->
  filenames = {}
  strings = {}
  for name in *{...}
    if type(name) == "string" then filenames[#filenames + 1] = name
  for filename in *filenames
    data = io.open(filename, 'rb')\read '*a'
    table.insert strings,
      fmt '/* {filename} */\n' ..
          'static const char {name}[] = \n{array};',
          {
            'filename': basename(filename),
            'name': safename(filename),
            'array': make_array(data),
          }
  '/* Automatically generated; do not edit */\n\n'\join strings

cembed  
