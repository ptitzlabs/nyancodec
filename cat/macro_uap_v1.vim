edit!
silent! g/^FX/d
silent! g/^\d\@!/d
%s/^\d\+.\{-}\ze\a/
%s/.\{-}\zs\s\+\d\++\=$/
g/\c^spare/s/.*/"spare", ""
g/^"\@!/s/\(\S\+\)\s\+\(.*\)/"\1","\2"
g/^"/s/\(.*\)/[\1],
norm ggi
put! = '{ \"uap\" : ['
norm G$xo
put! = ']}'
%!python -m json.tool
g/\s\{-}\[/,/\(\s\{-}\],\)\|\(]\n\s\{-}]\)/j



