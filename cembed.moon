map = (f, a, ...) ->
   t = {}
   for _ in *a
     _i = _index_0
     t[_i] = f(_,...)
   return t

totable = (str) ->
  tab = {}
  str\gsub '.',(c) -> table.insert tab,c
  tab

string.rtrim = (del='') =>
  n = #@
  while n > 0 and @\find('^%s', n) do n = n - 1
  @\sub(1, n) .. del

string.join = (arr) =>
  ret = ''
  for str in *arr do
    ret ..= str
  @ .. ret

fmt = (fmt,dic) ->
  for k,v in pairs dic
    fmt = fmt\gsub '{'.. k ..'}', tostring v
  return fmt

make_array = (data) ->
  i = {0}
  fn = (x) ->
    x = tostring(string.byte x) .. ','
    if i[0] + #x > 78
        i[0] = #x
        x = '\n' .. x
    else
        i[0] += #x
    return x
  '{'.. ""\join map(fn,totable data)\rtrim ',' ..'}'
