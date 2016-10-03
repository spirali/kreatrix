" Filenames: *.kx

if version < 600
	syntax clear
elseif exists("b:current_syntax")
	finish
endif


syn match kxpipe "\|"
syn match kxupdate "<-"
syn match kxset "="
syn match kxtype "[A-Z][A-Za-z0-9_]*"
syn match kxmsg "[a-z0-9_][A-Za-z0-9_:]*"
syn match kxparam ":[A-Za-z0-9_:]*"
syn match kxnum "[0-9][0-9\.]*"

syn match kxreturn "\^"

syn region kxcomment start="//" end="$"
syn region kxlongcomment start="/\*" end="\*/"
syn region kxstring start="\"" skip="\\\"" end="\""

syn match kxsymbol "#[A-Za-z_][A-Za-z0-9_]*"
syn match kxdot "[.]"
"syn keyword kxoperators
syn match kxmethod "[\{\}\[\]\|]"

"highlight link kxmsg Identifier
highlight link kxtype Type
highlight link kxmethod Delimiter
highlight link kxcomment Comment
highlight link kxlongcomment Comment

"highlight link kxdot Delimiter
highlight link kxsymbol Constant
highlight link kxstring String
highlight link kxupdate Operator
highlight link kxset Operator
highlight link kxparam Identifier
highlight link kxreturn Operator
highlight link kxop Operator

highlight link kxpipe Special
highlight link kxnum Number


let b:current_syntax = "kreatrix"

