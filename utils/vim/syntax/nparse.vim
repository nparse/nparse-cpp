" Vim syntax file
" Language:		nParse script
" Maintainer:	Alex Kudinov <alex.s.kudinov@gmail.com>
" Filenames:	*.ng

" common syntax
syn match		nparseCommon			+[-_[:alnum:]]\++
syn match		nparseOperator			+[:=;$()[\]{}\+*?_#|%^/\\@&!]+
syn match		nparseLabel				"\*\s*\d\+"
syn match		nparseSubstitution		+$[^[:space:]:=;$()[\]{}<>\+*?_#|%^/\\@&!]\++
syn match		nparseAssignment		+:[^[:space:]:=;$()[\]{}<>\+*?_#|%^/\\@&!]\++
syn region		nparseFunctor			start=+<+ end=+>+ contains=nparseAction

" semantic actions
syn region		nparseAction			start=+{+ end=+}+ contains=nparseAction,nparseKeyword,nparseGlobal,nparseLocal,nparseNumber,nparseString,nparseIndex,nparseComment
syn keyword		nparseKeyword			push pop if else elseif for foreach in each while do switch case default break continue contained
syn keyword		nparseKeyword			null boolean true false integer real string array contained
syn match		nparseGlobal			+[_[:alnum:]]\++ contained
syn match		nparseLocal				+\$[bifdrs]\?\d*\|_[_[:alnum:]]\++ contained
syn match		nparseNumber			"[-+]\?\d\+\(\.\d\+\)\?\([eE][-+]\?\d\+\)\?" contained
syn match		nparseIndex				+[_[:alnum:]]\+::\@!+ contained
syn region		nparseString			start=+"+ end=+"+ skip=+\\"+ contains=nparseSubString 
syn region		nparseString			start=+'+ end=+'+ skip=+\\'+
syn match		nparseSubString			+$\@<!$\({[_[:alnum:]]*}\|[_[:alnum:]]*\)+ contained

" regular expressions
syn region		nparseRegEx				start=+"^+ end=+"+ skip=+\\"+
syn region		nparseRegEx				start=+'^+ end=+'+ skip=+\\'+
syn match		nparseRegEx				"\^[^;[:space:]]\+"

" declarations
syn region		nparseDeclaration		start=+\(namespace\|import\)\s\++ end=+;+ contains=nparsePrefix
syn match		nparsePrefix			+\s\+[^:;]\++ contained

" comments
syn match		nparseComment			"//.*$"
syn region		nparseComment			start=+/\*+ end=+\*/+

" default color scheme
highlight		nparseCommon			ctermfg=white guifg=white
highlight		nparseDeclaration		ctermfg=darkred guifg=darkred
highlight		nparsePrefix			ctermfg=lightred guifg=lightred
highlight		nparseOperator			ctermfg=cyan guifg=cyan
highlight		nparseLabel				ctermfg=magenta guifg=magenta
highlight		nparseSubstitution		ctermfg=gray guifg=gray
highlight		nparseAssignment		ctermfg=cyan guifg=cyan
highlight		nparseFunctor			ctermfg=lightblue guifg=lightblue
highlight		nparseAction			ctermfg=darkgreen guifg=darkgreen
highlight		nparseKeyword			ctermfg=yellow guifg=yellow
highlight		nparseGlobal			ctermfg=lightgreen guifg=lightgreen
highlight		nparseLocal				ctermfg=lightgray guifg=lightgray
highlight		nparseNumber			ctermfg=yellow guifg=yellow
highlight		nparseIndex				ctermfg=white guifg=white
highlight		nparseString			ctermfg=yellow guifg=yellow
highlight		nparseSubString			ctermfg=brown guifg=brown
highlight		nparseRegEx				ctermfg=brown guifg=brown
highlight		nparseComment			ctermfg=darkgray guifg=darkgray
