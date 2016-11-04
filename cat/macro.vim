silent! echo "file reset"
silent! edit!
silent!  %s/"/\\"/g
silent! %s/,/#
silent! echo "deleting header/footer lines"
silent! g/^The up-to-date/d
silent! g/^SUR.*/d
silent! g/^Edition.*/d
silent! g/^SDPS.*/d
silent! g/^Release.*/d
silent! g/^Page.*/d
silent! g/.*SUR.ET1.ST05.200.*/d
silent! g/.*Surveillan.*Part.*/d
silent! "silent! g/^Data Item.*/d
silent! %s/^Data Item.*\ze\n\(Definition\)\@!//g
silent! g/^\(\p\|\n\)\@!/d
silent! %s/\(\p\|\n\)\@!/ /g
silent! %s/\]/)
silent! %s/\[/(
silent! echo "deleting document beginning and the end"
silent! g/./,/\cDescription of Standard Data Items/d
silent! g/./,/\cDefinition of all data items/d
silent! g/.*\cUser Application Profile/,$/d
silent! norm gg
silent! let header='{"cat" : '
silent! let footer="\n\n\n\n}"
silent! echo "merging lines"
silent! g/\(^NOTE\|^Note\).*/,/\(^Encoding\|^Octet\|^Structure\|\(^Data Item\|\d.\d.\d.*Data Item\)\|\%$\)/-1j
silent! g/\(^Format\).*/,/\(^Encoding\|^Octet\|^Structure\|\(^Data Item\|^\d.\d.\d.*Data Item\)\)/-1j
silent! g/\(^Encoding\).*/,/\(^Encoding\|^Octet\|^Structure\|\(^Data Item\|^\d.\d.\d.*Data Item\)\|\d.*User Application\)/-1j
silent! g/\(^Definition\).*/,/\(^Structure\|^Format\|^Octet\|\(^Data Item\|^\d.\d.\d.*Data Item\)\)/-1j
silent! g/\(^bit\).*/,-1/\(^bit\|^Encoding\|^NOTE\|^Structure\|^Format\|^Octet\|\(^Data Item\|^\d.\d.\d.*Data Item\)\)/s/\(.*\d\)\n\(\d.*\)/\1\2
silent! g/\(^bit\).*/,/\(^bit\|^Encoding\|^NOTE\|^Structure\|^Format\|^Octet\|\(^Data Item\|^\d.\d.\d.*Data Item\)\)/-1j
silent! g/\(^Octet\).*/,/\(^bit\|^NOTE\|^Structure\|^Format\|^Octet\|\(^Data Item\|^\d.\d.\d.*Data Item\)\)/-2d
silent! %s/Structure\(.*\)\n\(.*\)\csubfield/Structure\1 \2subfield
silent! g/\c^structure of primary subfield/,/\c^structure of subfield/-1d
silent! %s/\c.note.:/\rNOTE :
silent! g/\(^Structure:\).*\n/,/\(^bit\)/-1j
silent! g/\(^NOTE\|^Note\).*/,/\(^Encoding\|^Octet\|^Structure\|\(^Data Item\|^\d.\d.\d.*Data Item\)\|\%$\)/-1j
silent! %s//</g
silent! %s/\(°\|\)\(\s\{,2}\/\s\{,2}\d\+\)/\2\1/g
silent! %s/°\|/deg/g
silent! g/^bit.*/s/degrees\s/deg 
silent! g/^bit.*/s/degree\s/deg 
silent! %s/\cStructure\nof/Structure of
silent! %s/= =/=
silent! echo "fixing bit appearance"
silent!  %s/^bit-\s\{-}\ze\d/bit-
silent!  %s/^bit\s\{-}\ze\d/bit-
silent!  %s/^bits-\s\{-}\ze\d/bits-
silent!  %s/^bits\s\{-}\ze\d/bits-
silent! %s/^bit-\ze\d\+\/\d\+/bits-
silent! %s/LSB/qLSBq
silent! %s/(qLSBq)/qLSBq
silent! %s/qLSBq/(LSB)
silent! %s/(LSB)\S/(LSB) 
silent! %s/m\/s 2/m\/s2
silent! %s/.Structure:/\rStructure:
silent! %s/Structure\zs.\{,5}\zeSubfield/ of /g
silent! echo "fixing structure ids"
silent! %s/.Structure of/\rStructure of
silent!  %s/^\cStructure of First.\{-}Part/\rStructure of Subfield # 1:
silent!  %s/^\cStructure of First.\{-}Ext/\rStructure of Subfield # 2:
silent!  %s/^\cStructure of Second.*/\rStructure of Subfield # 3:
silent!  %s/^\cStructure of Third.*/\rStructure of Subfield # 4:
silent!  %s/^\cStructure of Fourth.*/\rStructure of Subfield # 5:
silent!  %s/^\cStructure of Fifth.*/\rStructure of Subfield # 6:
silent!  %s/^\cStructure of Six.*/\rStructure of Subfield # 7:
silent!  %s/^\cStructure of Seventh.*/\rStructure of Subfield # 8:
silent!  %s/^\cStructure of Eighth.*/\rStructure of Subfield # 9:
silent!  %s/^\cStructure of Nine.*/\rStructure of Subfield # 10:
silent!  %s/^\cStructure of Eleven*/\rStructure of Subfield # 11:
silent!  %s/^\cStructure of Tvelv.*/\rStructure of Subfield # 12:
silent! echo "reading items"
silent! %s/.*Data.*Item\s\+\([a-zA-Z0-9\/]\+\)\A\+\(\a.*\)\nDefinition\A*\(\a.*\)\n\(Format\|Structure\)\A*\(\a.*\)/"item" : "\1" {\r"item name" : "\2",\r"item definition" : "\3",\r"format" : "\5",\r
silent!  %s/\c^note\A*\(\a.*\)/"note" : "\1",
silent! %s/\c^encoding rule.*optional.*/"mandatory" : "false",
silent! %s/\c^encoding rule.*shall be present.*/"mandatory" : "true",
silent! g/^"format"/s/"format" : \zs\c.\{-}fixed.\{-}\(\h\{-}\)\A\{,3}octet.*/"fixed",\r"length" : "\1",
silent! g/^"format"/s/"format" : \zs\c.\{-}\(\h\{-}\)\A\{,3}octet.\{-}fixed.*/"fixed",\r"length" : "\1",
silent! g/^"format"/s/"format" : \zs\c.\{-}compound.\{-}\(\h\{-}\)\A\{,3}octet.*/"compound",\r"length" : "\1",
silent! g/^"format"/s/"format" : \zs\c.\{-}\(\h\{-}\)\A\{,3}octet.\{-}compound.*/"compound",\r"length" : "\1",
silent! g/^"format"/s/"format" : \zs\c.\{-}\(variable\).\{-}\(\h\{-}\)\A\{,3}octet.*/"variable",\r"length" : "\2",
silent! g/^"format"/s/"format" : \zs\c.\{-}\(\h\{-}\)\A\{,3}octet.\{-}\(variable\).*/"variable",\r"length" : "\1",
silent! g/^"format"/s/"format" : \zs\c.\{-}\(variable\).*/"variable",\r"length" : 1,
silent! g/^"format"/s/"format" : \zs\c.\{-}\(exten\).\{-}\(\h\{-}\)\A\{,3}octet.*/"extendible",\r"length" : "\2",
silent! g/^"format"/s/"format" : \zs\c.\{-}\(\h\{-}\)\A\{,3}octet.\{-}\(exten\).*/"extendible",\r"length" : "\1",
silent! g/^"format"/s/"format" : \zs\c.\{-}\(exten\).*/"extendible",\r"length" : 1,
silent! g/^"format"/s/"format" : \zs\c.\{-}\(repetiti\).\{-}\(\h\{-}\)\A\{,3}octet.*/"repetitive",\r"length" : "\2",
silent! g/^"format"/s/"format" : \zs\c.\{-}\(\h\{-}\)\A\{,3}octet.\{-}\(repetiti\).*/"repetitive",\r"length" : "\1",
silent! %s/"length" : "one.\{-}"/"length" : 1/g
silent! %s/"length" : "two.\{-}"/"length" : 2/g
silent! %s/"length" : "three.\{-}"/"length" : 3/g
silent! %s/"length" : "four.\{-}"/"length" : 4/g
silent! %s/"length" : "five.\{-}"/"length" : 5/g
silent! %s/"length" : "six.\{-}"/"length" : 6/g
silent! %s/"length" : "seven.\{-}"/"length" : 7/g
silent! %s/"length" : "eight.\{-}"/"length" : 8/g
silent! %s/\(.*\)\n\(.*\)\nbits\D*\(\d*\).\{-}\(\d*\)\s\(.*\)\nbit\D*\(\d*\)\D*and\D*\(\d*\)\D*(LSB)\(.*\)/bits-\3\/\6 \1 \5 (LSB) \8\rbits-Q\6Q\/\7 \2 \5 (LSB) \8
silent! echo "fixing lsb to decimal format from fractions, powers, etcetera"
silent! %s/Q\zs\d\+\zeQ/\=submatch(0)-1/g
silent! %s/\Q\(\d\+\)Q/\1
silent! %s/(LSB)/ QLSBQ /g
silent! %s/\d\zs\n\ze)//g
silent! %s/(\d\+\zs\s\ze\d\+)//g
silent! %s/\d\.\d\+\zs\s\ze\d\+//g
silent! %s/([+-]\=\(\d\+\))/\1/g
silent! %s/(\(\d\+\.\d\+\))/\1
silent! %s/\(\d\+\)\s\{,2}\(\D\{,4}\)\s\{,2}\/\s\{,2}\(\d\+\)/\1\/\3 \2
silent! %s/^bits\(.*LSB.*\)\@!.*\zs\nbit\(.\{,3}\d\+\D\d\+\)\@!.\{-}\d\+\ze\(.*LSB.*\)/ 
silent! %s:2\s\{,3}(\([+-]\=\d\+\)+f):\= pow(2,str2float(submatch(1))):g
silent! g/^bit.*LSB.*/s/\(\d\.\d\+\)\*10\([-+]\=\d\+\)/\=str2float(submatch(0))*pow(10,str2float(submatch(2)))/g
silent! g:^bit.*LSB.*:s:QLSBQ\D\{,6}\zs\(\d\+\)\s\{,2}/\s\{,2}2\(\d\+\):\= str2float(submatch(1))*pow(2,-str2float(submatch(2))):g
silent! g:^bit.*LSB.*:s:QLSBQ\D\{,20}\zs2\s\{,2}-\s\{,2}\(\d\+\):\= pow(2,-str2float(submatch(0))):g
silent! g:^bit.*LSB.*:s:QLSBQ\D\{,20}\zs\(\d\+\)\s\{,2}/\s\{,2}\(\d\+\):\= str2float(submatch(1))/str2float(submatch(2)):g
silent! g:^bit.*LSB.*:s:=\D\{,4}\zs\(\d\+\)\s\{,2}/\s\{,2}2\(\d\+\):\= str2float(submatch(1))*pow(2,-str2float(submatch(2))):g
silent! g:^bit.*LSB.*:s:=\D\{,4}\zs2\s\{,2}-\s\{,2}\(\d\+\):\= pow(2,-str2float(submatch(0))):g
silent! g:^bit.*LSB.*:s:=\D\{,4}\zs\(\d\+\)\s\{,2}/\s\{,2}\(\d\+\):\= str2float(submatch(1))/str2float(submatch(2)):g
silent! g:^bit.*LSB.*:s:=\D\{,4}\zs10\([+-]\=\d\+\):\=pow(10.0,str2float(submatch(1)):g
silent! g/^"begin bit.*/s/\s\{2,}/ /g
silent!  %s/\(^bits-\d\+\/\d\+\)(\S\)/\1 \2
silent!  %s/\(^bit-\d\+\/\d\+\)(\S\)/\1 \2
silent!  %s/bits\zs.{,3}\ze\d\+./{,3}\d\+/-
silent!  %s/bits.{,3}\d\+\zs./{,3}\ze\d\+/\/
silent!  %s/^bits.*\zs\(\crange\)\ze.\{,10}</
silent!  %s/^bits.*\zs\(\crange\)\ze.\{,20}min/
silent! %s/bits-\d\+\zs.\ze\d\+/\//g
silent! %s/)\zs\ze\S/ /g
silent! g/^bit\A\d\+.\{-}.*\cresolution.*=.*\d\+.*=.*\d\+.*/s/bit\A\(\d\+\)\s\{-}(\(\S.\{-}\))/"bit begin" : \1, "bit end" : \1, "type" : "lsb-selector", "name1" : "\2", "options" : [ /g
silent! g/^bit\A\d\+.\{-}=\s\{,2}.*\clsb.*=\s\{,2}\d\s\{,2}/s/bit\A\(\d\+\)\s\{-}(\(\S.\{-}\))/"bit begin" : \1, "bit end" : \1, "type" : "lsb-selector", "name1" : "\2", "options" : [ 
silent! g/.*"lsb-selector".*=.*=.*=.*=/s/=\s\{,2}\d\+\s\{1,}.\{-}\(\S.\{-}\)\c.lsb.\{-}=\s\{-}\(\d\+\(\.\d\+\)\=\)\(\s\{,3}\)\=\(\S\{,20}\)\|$/["\1", \2, "\5"],/g
g/.*"lsb-selector".*=[^=]*=[^=]*/s/=\s\{,2}\d\+\(.\{-}\)\(\d\+\)\s\+\(\S*\).\{-}=\s\{,2}\d\+\(.\{-}\)\(\d\+\)\s\+\(\S*\)/\["\1", \2,"\3", "\4",\5, "\6"\] 
silent! g/.*"lsb-selector".*/s/.*\zs/],
silent! %s/Structure.*\nbit\zs-\(\d\+\)\ze\D\{2,}.*LSB.*\n\"note"/s-\1\/1
silent!  %s/"length" : \(\d\+\)\_.\{,500}\nbit\zs-\ze\d.*QLSBQ/s-\1XBYTES\//g
silent!  %s/"length" : \(\d\+\)\_.\{,500}\nbit\zs-\ze\d.*QLSBQ/s-\1XBYTES\//g
silent!  %s/"length" : \(\d\+\)\_.\{,500}\nbit\zs-\ze\d.*QLSBQ/s-\1XBYTES\//g
silent!  %s/"length" : \(\d\+\)\_.\{,500}\nbit\zs-\ze\d.*QLSBQ/s-\1XBYTES\//g
silent!  %s/"length" : \(\d\+\)\_.\{,500}\nbit\zs-\ze\d.*QLSBQ/s-\1XBYTES\//g
silent!  %s/"length" : \(\d\+\)\_.\{,500}\nbit\zs-\ze\d.*QLSBQ/s-\1XBYTES\//g
silent!  %s/"length" : \(\d\+\)\_.\{,500}\nbit\zs-\ze\d.*QLSBQ/s-\1XBYTES\//g
silent!  %s/"length" : \(\d\+\)\_.\{,500}\nbit\zs-\ze\d.*QLSBQ/s-\1XBYTES\//g
silent!  %s/"length" : \(\d\+\)\_.\{,500}\nbit\zs-\ze\d.*QLSBQ/s-\1XBYTES\//g
silent!  %s/"length" : \(\d\+\)\_.\{,500}\nbit\zs-\ze\d.*QLSBQ/s-\1XBYTES\//g
silent!  %s/"length" : \(\d\+\)\_.\{,500}\nbit\zs-\ze\d.*QLSBQ/s-\1XBYTES\//g
silent!  %s/"length" : \(\d\+\)\_.\{,500}\nbit\zs-\ze\d.*QLSBQ/s-\1XBYTES\//g
silent!  %s/"length" : \(\d\+\)\_.\{,500}\nbit\zs-\ze\d.*QLSBQ/s-\1XBYTES\//g
silent!  %s/"length" : \(\d\+\)\_.\{,500}\nbit\zs-\ze\d.*QLSBQ/s-\1XBYTES\//g
silent!  %s/"length" : \(\d\+\)\_.\{,500}\nbit\zs-\ze\d.*QLSBQ/s-\1XBYTES\//g
silent!  %s/"length" : \(\d\+\)\_.\{,500}\nbit\zs-\ze\d.*QLSBQ/s-\1XBYTES\//g
silent!  %s/"length" : \(\d\+\)\_.\{,500}\nbit\zs-\ze\d.*QLSBQ/s-\1XBYTES\//g
silent!  %s/"length" : \(\d\+\)\_.\{,500}\nbit\zs-\ze\d.*QLSBQ/s-\1XBYTES\//g
silent! %s/\(\d\+\)XBYTES/\=submatch(0)*8/
silent! g/\c^bit.\d\+\D\{3,}.*LSB
silent! %s/^bits.*\zs\(\crange\)\=.*"\@!\cmin"\@!\D\{,20}\(-.\{,2}\d\+\(\.\d\+\)\=\)\D\{,20}\"\@!\cmax"\@!\D\{,20}\(\d\+\(\.\d\+\)\=\)/ "min" : \2, "max" : \4, "signed" : "true",/g
silent!  %s/^bits.*\zs\(\crange\)\=.*"\@!\cmin"\@!\D\{,20}\(\d\+\(\.\d\+\)\=\)\D\{,20}\"\@!\cmax"\@!\D\{,20}\(\d\+\(\.\d\+\)\=\)/ "min" : \2, "max" : \4, "signed" : "false",/g
silent! %s/^bits.*\zs\(\crange\)\=.*"\@!\cmax"\@!\D\{,20}\(\d\+\(\.\d\+\)\=\)/ "min" : 0, "max" : \2, "signed" : "false",
silent! g/^bits.*\clsb.*/s/\(\(-.\{,2}\d\+\(\.\d\+\)\=\)\D\{,8}<.\{-}<\D\{-}\(\d\+\(\.\d\+\)\=\)\)/ "min" : \2, "max" : \4, "signed" : "true",/g
silent! g/^bits.*\clsb.*/s/\(\(\d\+\(\.\d\+\)\=\)\D\{,8}<.\{-}<\D\{-}\(\d\+\(\.\d\+\)\=\)\)/ "min" : \2, "max" : \4, "signed" : "false",/g
silent! g/^bits\(.*signed.*\)\@!.*$/s/^bits.*\ccomplement.*\zsQLSBQ\D\{,10}\(\d\+\(\.\d\+\)\=\(e[+-]\d\+\)\=\)\(\s\{,3}\)\=\([^"=]\{,20}\)/ "type" : "float", "lsb" : \1, "unit" : "\5", "signed" : "true",
silent! g/^bits\(.*signed.*\)\@!.*$/s/^bits.*\zsQLSBQ\D\{,10}\(\d\+\(\.\d\+\)\=\(e[+-]\d\+\)\=\)\(\s\{,3}\)\=\([^"=]\{,20}\).*\ccomplement/ "type" : "float", "lsb" : \1, "unit" : "\5", "signed" : "true",
silent! g/^bits\(.*signed.*\)\@!.*$/s/^bits.*\zsQLSBQ\D\{,10}\(\d\+\(\.\d\+\)\=\(e[+-]\d\+\)\=\)\(\s\{,3}\)\=\([^"=]\{,20}\)/ "type" : "float", "lsb" : \1, "unit" : "\5", "signed" : "false",
silent! g/^bits/%s/^bits.*\zsQLSBQ\D\{,10}\(\d\+\(\.\d\+\)\=\(e[+-]\d\+\)\=\)\(\s\{,3}\)\=\([^"=]\{,20}\)\ze\_.\{,1000}\ccomplement/ "type" : "float", "lsb" : \1, "unit" : "\5",
silent! g/^bits/%s/^bits.*\zsQLSBQ\D\{,10}\(\d\+\(\.\d\+\)\=\(e[+-]\d\+\)\=\)\(\s\{,3}\)\=\([^"=]\{,20}\)/"type" : "float", "lsb" : \1, "unit" : "\5",
silent! g/^bits.*float.*/s/^bits.\(\d\+\)\D\(\d\+\)\s\{-}\(\((\=[^"]\{,20})\=\)[^"]\{-}\)\(\s\+\)/"bit begin" : \1, "bit end" : \2, "name1" : "\4", "name2" : "\3", 
silent! g/^bits.*"lsb".*/s/.*".\{,20}" :.\{-},\zs.*/ /g
silent! g/^bits.*"lsb".*/s/bits-\(\d\+\)\/\(\d\+\)\(\s\{-}\)\=\(\((\@!\S.\{,10}\)\=(\(.\{-}\))\)\(\s\{-}\)\=\(\S.\{-}\)\ze\(\s\{-}\)\=".\{-}" : /"bit begin" : \1, "bit end" : \2, "name11" : "\4", "name2" : "\8", /g
silent! g/^bits.\d\+.\d\+/s/bits.\(\d\+\).(\d\+\)\s\+\ze\("min"\|"max"\|"type"\)\(\s\{-}\)\=".\{-}" : /"bit begin" : \1, "bit end" : \2, "name12" : "\4", "name2" : "" /g
silent! g/^bits.\d\+.\d\+/s/bits.\(\d\+\).(\d\+\)\(\s\{-}\)\ze\("min"\|"max"\|"type"\)/"bit begin" : \1, "bit end" : \2, "name13" : "", "name2" : "" /g
silent! %s/bits.\(\d\+\).\(\d\+\)\s\+\ze\("min"\|"max"\|"type"\)/"bit begin" : \1, "bit end" : \2, "name14" : "", "name2" : "", /g
silent! %s/"(\(.\{-}\))"/"\1"/g
silent! %s/\s\{1,}/ /g
silent! %s/^bits.\(\d\+\)\D\(\d\+\).*\cspare bit.*/"bit begin" : \1, "bit end" : \2, "type" : "spare",
silent! %s/^bit.\(\d\+\).*\cspare bit.*/"bit begin" : \1, "bit end" : \1, "type" : "spare",
silent! %s/: "[^"]*",\zs[^"]*\ze"\|$/ 
silent! g/^bits.*\c\(repet\|(REP)\)/s/^bits.\(\d\+\)\D\(\d\+\)\s\{-}\(\(.\{,20}\)[^"]*\)/"bit begin" : \1, "bit end" : \2, "type" : "rep", "name111" : "\4", "name2" : "\3", 
silent! g/^bits.*\(\d\+\s\{,2}=.\{-}\d\s\{,2}=\)\{1,}/s/bits-\(\d\+\)\/\(\d\+\)/"bit begin" : \1, "bit end" : \2, "type" : "enum", 
silent! g/\(\d\+\s\{,2}=.\{-}\d\s\{,2}=\)\{1,}/s/\(\d\+\)\s\{,2}=\s\{-}\(\S.\{-}\)\ze\(\(\(\d\+\)\s\{,2}=\)\|$\)/["\1", "\2"], /g
silent! g/^"begin.*enum"/s/.*\zs$/],
silent! %s/.*enum\",\s\{-}\zs(\(\S.\{-}\))\s\{-}\(\S.\{-}\)\=\ze\s\{-}\[\"/ "name15" : "\1", "name2" : "\2", "options" : 
silent! g/^bits.*\(\(=\s\{,2}\d\+\s\{1,}\).*\(\(=\s\{,2}\d\+\s\{1,}\)\|$\)\)/s/bits.\(\d\+\).\(\d\+\)\(\s\{-}\)\=\((\(\S.\{-}\))\)\=\s\{-}\(\S.\{-}\)\=\s\{-}\ze=/"bit begin" : \1, "bit end" : \2, "name1" : "\6", "name2" : "\7", "type" : "enum", "options" : 
silent! g/^bit.*(FX)/d
silent! g/.*lsb-selector.*\n/,/^bits.*/ s/^bits.\(\d\+\).\(\d\+\)\s\{-}\(\S.*\)/bit begin" : \1, "bit end" : \2, "name16" : "\3", "type" : "variable float", "signed" : "false", /g
silent! g/^bit.*\(\(=\s\{,2}\d\+\s\{1,}\).*\(\(=\s\{,2}\d\+\s\{1,}\)\|$\)\)/s/bit.\(\d\+\)\s\{-}\((\(\S.\{-}\))\)\s\{-}\(\S.\{-}\)\=\s\{-}\ze=/"bit begin" : \1, "bit end" : \1, "name17" : "\3", "name2" : "\4", "type" : "enum", "options" : 
silent! g/.*"type" : "enum".*\(=\s\{,2}\d\+\s.*=\s\{,2}\d\+\s\)\{1,}/s/=\s\{,2}\(\d\+\)\s\{,5}\(\S.\{-}\)\ze\(\(\s\{-}=\)\|$\)/["\1", "\2"], /g
silent! g/.*"type" : "enum".*"options" : [^\[]*$/s/.*"options" : \zs.*/ ["0", "option 1"], ["1", "option 2"],
silent! g/^bits.*\character/s/^bits.\(\d\+\)\D\(\d\+\)\(.\{-}\(\d\).\{,4}bits .*\)/"bit begin" : \1, "bit end" : \2, "type" : "string", "char size" : \4, "name18" : "\3",
silent! %s/\n\@!\(Character \d\)/\r\1/g
silent! %s/\n\n\(Character \d\)/\r\1\g
silent! %s/\(\nCharacter \d.*\)\_.\{,20}\nCharacter \(\d\).*\(\n"note".*ASCII.*\)/\r"bit begin" : \2, "bit end" : 1, "type" : "string", "string length" : \2, "char size" : 8,\3
silent! %s/bit begin" : \zs\(\d\)\ze.*"string length"/\=submatch(0)*8/ "silent! g/^bits.*\coctal/s/^bits.\(\d\+\)\D\(\d\+\)\s\{-}\(.*\)/"bit begin" : \1, "bit end" : \2, "type" : "octal", "namea19" : "\3", 
silent! g/^bits.*\cASCII.\{-}character/s/^bits.\(\d\+\)\D\(\d\+\)\s\{-}\(\(.\{,20}\).*\)/"bit begin" : \1, "bit end" : \2, "type" : "char", "name110" : "\4", "name2": "\3"
silent! g/^bits.*float.*/s/^bits.\(\d\+\)\D\(\d\+\)\(\s\{-}\)\=\(.\{-,15}).\{-}\)\ze"type/"bit begin" : \1, "bit end" : \2, "name114" : "\5", "name2" : "\6", 
silent! g/^bits/\(.*"type".*\)s/^bits.\(\d\+\)\D\(\d\+\)\s\{-}\(\(.\{,15}.\{-}\s\).\{-}\)/"bit begin" : \1, "bit end" : \2, "type" : "int", "name112" : "\4", "name2" : "\3", 
silent! g/^bits\(.*"type".*\)\@!/s/^bits.\(\d\+\)\D\(\d\+\)\s\{-}\(\([^"]\{,15}[^"]\{-}\s\)[^"]\{-}\)/"bit begin" : \1, "bit end" : \2, "type" : "int", "name112" : "\4", "name2" : "\3", 
silent! g/^bit/s/^bit.\(\d\+\)\s\{-}\(\(.\{,15}.\{-}\s\).*\)\ze\("min"\|"max"\|"type"\)/"bit begin" : \1, "bit end" : \1, "type" : "enum", "name113" : "\3", "name2:" "\2" , "options" : [["0", "option 1"], ["1", "option 2"]],
silent! g/^"format"/s/"format" : \zs\c.\{-}fixed.\{-}\(\h\{-}\)\A\{,3}octet.*/"fixed",\r"length" : "\1",
silent! g/^"format"/s/"format" : \zs\c.\{-}\(\h\{-}\)\A\{,3}octet.\{-}fixed.*/"fixed",\r"length" : "\1",
silent! g/^"format"/s/"format" : \zs\c.\{-}compound.\{-}\(\h\{-}\)\A\{,3}octet.*/"compound",\r"length" : "\1",
silent! g/^"format"/s/"format" : \zs\c.\{-}\(\h\{-}\)\A\{,3}octet.\{-}compound.*/"compound",\r"length" : "\1",
silent! g/^"format"/s/"format" : \zs\c.\{-}\(extendible\|variable\).\{-}\(\h\{-}\)\A\{,3}octet.*/"variable",\r"length" : "\2",
silent! g/^"format"/s/"format" : \zs\c.\{-}\(\h\{-}\)\A\{,3}octet.\{-}\(extendible\|variable\).*/"variable",\r"length" : "\1",
silent! %s/Structure.*\zs\n\(\u\{3,}\s\)\{1,}.*/
silent! %s/Structure.*\zs\n\(\u\{3,}\s\)\{1,}.*/
silent! %s/^"bit.*"enum".\{-}\zs\[\(.*\)\],\ze/[[\1]]
silent!  %s/^"bit".*\zs,$/
silent! %s/^"length".*\zs/\r"subfields" : \r [
silent! %s/^Structure.*:\zs\(.*\)/\r\1
silent! g/\(^\u\{2,} \|$\)\{2}/d
silent! g/^$/d
silent! %s/^\s\{1,}/
silent! g/^"bit/s/\."/"/g
silent! %s/"unit" : "\zssecond/s
silent! %s/"[^":,]*\zs\s\+\ze"//g
silent! %s/"\zs\s\+\ze[^":,]*"//g
silent! %s/","/", "
silent! %s/"unit" : "\(\d\+\)\=[a-zA-Z\/]\+\d\=\zs[^"]*\ze"//g
silent! g/.*"type" : "float".*"unit" : "\(s\|ms\)"/s/"type" : \zs"float"\ze/"time"
silent! %s/complement\_.\{,1000}"signed" : \zs"false"\ze/"true"
silent! %s/"signed" : \zs"false"\ze\_.\{,1000}complement/"true"
silent! g/^Structure.*\n\("bit\)\@!/s/Structure.\{-}\(\d\+\).\{-}:.*\n\(.*\)\(\_.\{,100}\n\(Format.*\)\)\=/{\r"id" : \1,\r"subfield info" : "\2 \4",\r"data" :\r[
silent! g/^Structure.*\n\("bit\)\@!/s/Structure.\{-}#\s\{,4}\(\d\+\)\=.*\n\(.*\)\(\_.\{,100}\n\(Format.*\)\)\=/{\r"id" : \1,\r"subfield info" : "\2 \4",\r"data" :\r[
silent! g/^Structure.*\n\("bit\)\@!/s/Structure.\{-}\(\d\+\)\=.*\n\(.*\)\(\_.\{,70}\n\(Format.*\)\)\=/{\r"id" : 1,\r"subfield info" : "\2 \4",\r"data" :\r[
silent! g/^Structure.*\n\("bit\)/s/Structure.\{-}#\s\{,4}\(\d\+\)\=.*/{\r"id" : \1,\r"subfield info" : "",\r"data" :\r[
silent! "%s/^Structure.*/{\r"subfield info" : "",\r"data" :\r[
silent! %s/^\("bit begin".*\)",.{-}$/{\1},
silent! %s/^\("bit begin".*\)/{\1},
silent! g/\c\d.\d.user application profile/,/,$/s/FX.*/
silent! g/^$/d
silent! g!/^["{\[]/d
silent! g/^"subfields/j
silent! g/^"data/j
silent! %s/"bit end" : \zs\(\d\+\)\ze/\=submatch(0)-1/
silent! %s/"bit end" : \(\d\+\).*\n.*"bit begin" : \zs\(\d\+\)\ze/\1
silent! %s/^.*"bit begin" : \zs\(0\)\ze.*char size" : \(\d\+\).*string length" : \(\d\+\)/\=submatch(1)*submatch(2)
silent! %s/^"item"/\r\r\r"item"
silent! %s/{\n"id"/\r\r{"id"
silent! %s/"data" :.*\(\n{"bit begin.*\)\{-}\zs\ze\n{\@!/\r],
silent! g/^"data".*/,/^]/j
silent! %s/"subfield info".*\(\n".*\)\{-}\zs\ze\n"\@!/\r},
silent! g/^$/d
silent! %s/^"item"/\r\r\r"item"
silent! g/^{"id".*/,/^}/j
silent! %s/"subfields.*\(\n{.*\)\{-}\zs\ze\n{\@!/\r],
silent! g/"subfields".*/,/^\]/j
silent! %s/"item".*\(\n".*\)\{-}\zs\ze\n"\@!/\r},
silent! %s/"item" :\s\+\(.*\){/\r\r\1 : {
silent! %s/"cat"/{"cat"
silent! norm Go}
silent! norm Gklr 
silent! %s/\]\s*,\s*\]/\]\]/g
silent! %s/},\s*{/},\r{/g
silent! %s/\],\s*\[/\],\r\[/g
silent! %s/},\s\{-}\]/}\]/g
silent! %s/\],\s\{-}\]/\]\]/g
silent! %s/,\zs.\{-}\ze.*\({.*\)\@!".\{,10}" :/ /g
silent! g/^$/d
silent! %s/\[\s\+{/[{/g
silent! %s/},\s+\]/},]/g
silent! %s/"\s*,\s*}/"}/g
silent! %s/\],\n}/\]\r}
silent! %s/\]\s\{-},\s\{-}}/\]}/g
silent! %s/: \[{/:\r[\r{/g
silent! %s/^{\(\S\)/{\r\1/g
silent! %s/,[^"}\]]*}/}
silent! %s/: "[^"]*",\zs[^"]*"/ "/g
silent! %s/\][^\]},]*}/\]}
silent! %s/\[[^\[{",]*\[/[[
silent! %s/\][^\]}",]*\]/]]
silent! %s/\^./%
silent! %s/}\s\{-},\s\{-}}/}}/g
silent! %s/}\s{-},\s\{-}\n}/}\r}/g
silent! norm gg
silent! put! =header
silent! norm G
silent! %s/^{"cat" : \zs\ze\_.\{-}"\(\S\d\+\)\//"\1",
silent! %s/QLSBQ/LSB
silent! %s/name1\d\+/name1
silent! %s/name2\d\+/name2
silent! %s/"[^":,]*\zs\s\+\ze"//g
silent! %s/"\zs\s\+\ze[^":,]*"//g
silent! %s/"name1" : "(\([^)]*\))\s\{-}\([^"]*\)", "name2" : ""/"name1" : "\1", "name2" : "\2"
silent! %s/"name1" : "(\([^)]*\))\s\{-}\([^"]*\)"/"name1" : "\1"
silent! %s/#/,/g
silent! g/"bit begin"\(.*"name1".*\)\@!/s/"bit end" \: \d\+, \zs\ze/"name1" : "",/g
silent! g/"bit begin"\(.*"name2".*\)\@!.*/s/.*"name1" : "[^"]*"\zs/, "name2" : ""/g
g^"string"\(.*"string length".*\)\@!^s^"bit begin" : \(\d\+\),.\{-}"bit end" : \(\d\+\),.\{-}char size" : \(\d\+\)\zs\ze^\=', "string length" : ' . (submatch(1) - submatch(2)) / submatch(3) 
%!python -m json.tool
%s/"true"/true
%s/"false"/false


