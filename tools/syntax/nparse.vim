" Vim syntax file
" Language:     nParse script
" Maintainer:   Alex Kudinov <alex.s.kudinov@gmail.com>
" Filenames:    *.ng

" comments
syn region      ngComment           start="/\*" end="\*/"
syn region      ngComment           start="//" end="$"
hi              ngComment           ctermfg=grey

" delimiter
syn match       ngDelimiter         ";"
hi              ngDelimiter         ctermfg=white

" namespace
syn keyword     ngNamespace         namespace nextgroup=ngNamespaceArg skipwhite skipempty
syn match       ngNamespaceArg      "[-_.[:alnum:]]\+" nextgroup=ngDelimiter skipwhite skipempty
hi              ngNamespace         ctermfg=red cterm=bold
hi              ngNamespaceArg      ctermfg=red cterm=bold

" import
syn keyword     ngImport            import nextgroup=ngImportArg skipwhite skipempty
syn match       ngImportArg         "\([-_.[:alnum:]]\+/\+\)*[-_.[:alnum:]]\+" nextgroup=ngDelimiter skipwhite skipempty
hi              ngImport            ctermfg=red
hi              ngImportArg         ctermfg=red

" function/procedure
syn keyword     ngFunction          func function proc procedure nextgroup=ngFuncName skipwhite skipempty
syn match       ngFuncName          "[_[:alpha:]][_[:alnum:]]*" nextgroup=ngFuncArgs skipwhite skipempty
syn region      ngFuncArgs          start="(" end=")" contained nextgroup=ngFuncExpr,ngFuncBody skipwhite skipempty
syn region      ngFuncExpr          start="=" end="\(;\)\@=" contains=@ngAction nextgroup=ngDelimiter skipwhite skipempty
syn region      ngFuncBody          start="{" end="}" contains=ngFuncBody,@ngAction nextgroup=ngDelimiter skipwhite skipempty
hi              ngFunction          ctermfg=white
hi              ngFuncName          ctermfg=lightgreen
hi              ngFuncArgs          ctermfg=green
hi              ngFuncExpr          ctermfg=darkgreen
hi              ngFuncBody          ctermfg=darkgreen

" arguments
syn match       ngFuncArg1          "[_[:alpha:]][_[:alnum:]]*" contained containedin=ngFuncArgs,ngFuncExpr,ngFuncBody
syn match       ngFuncArg2          "_[_[:alnum:]]*" contained containedin=ngFuncArgs,ngFuncExpr,ngFuncBody
hi              ngFuncArg1          ctermfg=green
hi              ngFuncArg2          ctermfg=darkgray

" block
syn cluster     ngAction            contains=ngOperator,ngOperand,ngNumber,ngString,ngList,ngKeyword,ngComment
syn match       ngOperator          "/\(\*\|/\)\@!" contained
syn match       ngOperator          "[*%=#?:!|&^~,.<>]" contained
syn match       ngOperator          "[-+]\([[:digit:]]\)\@!" contained
syn keyword     ngOperand           null true false boolean integer real string array list object contained
syn match       ngOperand           "\$[bifdrs]\?[[:digit:]]*\>" contained
syn match       ngNumber            "\([-+]\|\<\)\d\+\(\.\d\+\)\?\([eE][-+]\?\d\+\)\?\>" contained
syn match       ngNumber            "\([-+]\|\<\)0[xX][0-9a-fA-F]\+\>" contained
syn region      ngString            start=`'` end=`'` skip=`\\'` contained
syn region      ngString            start=`"` end=`"` skip=`\\"` contained
syn match       ngStrPlaceholder    "$\@<!$\(\$\|{[_[:alnum:]]*}\|[_[:alnum:]]*\)" contained containedin=ngString
syn match       ngStrWhitespace     "[[:space:]]\+" contained containedin=ngString
syn region      ngList              start="\([_[:alnum:])\]][[:space:]]*\)\@<!\[" end="\]" contained contains=ngList
syn keyword     ngKeyword           if else ifelse for each foreach in do while switch case default break continue return push pop contained
hi              ngOperator          ctermfg=darkgreen
hi              ngOperand           ctermfg=yellow
hi              ngNumber            ctermfg=yellow
hi              ngString            ctermfg=brown
hi              ngStrPlaceholder    ctermfg=brown cterm=bold
hi              ngStrWhitespace     ctermfg=black cterm=underline
hi              ngList              ctermfg=darkcyan
hi              ngKeyword           ctermfg=darkgreen

" definition
syn keyword     ngEntity            rule node nextgroup=ngEntityName skipwhite skipempty
syn match       ngEntityName        "[-_.[:alnum:]]\+" nextgroup=ngEntityDef,ngFuncArgs skipwhite skipempty
hi              ngEntity            ctermfg=white
hi              ngEntityName        ctermfg=lightgreen cterm=bold

" entity (rule or node)
syn region      ngEntityDef         start="\(:=\|:\|{\)" end="\(;\)\@=" contains=ngFuncBody,ngString nextgroup=ngDelimiter skipwhite skipempty
syn match       ngGrRegExp          "\^[^;[:space:]]\+" contained containedin=ngEntityDef
syn match       ngGrAssign          ":[[:alpha:]][_[:alnum:]]*" contained containedin=ngEntityDef
syn match       ngGrReference       "\$:\?[-._[:alnum:]]\+" contained containedin=ngEntityDef
syn region      ngGrInvocation      start="<\(?\)\@!" end=">" contained containedin=ngEntityDef
syn match       ngGrLabel           "\*[[:space:]]*-\?[[:digit:]]\+" contained containedin=ngEntityDef
hi              ngEntityDef         ctermfg=darkcyan
hi              ngGrRegExp          ctermfg=magenta
hi              ngGrAssign          ctermfg=cyan cterm=bold
hi              ngGrReference       ctermfg=blue cterm=bold
hi              ngGrInvocation      ctermfg=blue
hi              ngGrLabel           ctermfg=lightred

"" composition operators
syn match       ngGrOperator        "[()_#|?+%/]" contained containedin=ngEntityDef
syn match       ngGrOperator        "\*\([[:space:]]*-\?[[:digit:]]\)\@!" contained containedin=ngEntityDef
syn match       ngGrOperator        "\^\([[:space:]]\)\@=" contained containedin=ngEntityDef
syn match       ngGrOperator        "\(<?\|?>\|:=\)" contained containedin=ngEntityDef
syn match       ngGrAssertion       "&" contained containedin=ngEntityDef
syn match       ngGrNegation        "!" contained containedin=ngEntityDef
hi              ngGrOperator        ctermfg=cyan
hi              ngGrAssertion       ctermfg=red
hi              ngGrNegation        ctermfg=red cterm=standout
