add_library(nparse_script
    src/static.cpp
    src/observer_sg.cpp
    src/markers.cpp
    src/tokenizer.cpp
    src/hashed_string.cpp
    src/script/staging.cpp
    src/script/grammar.cpp
    src/script/comment.cpp
    src/script/expression.cpp
    src/script/statement.cpp
    src/script/footprint.cpp
    src/script/decls/import.cpp
    src/script/decls/namespace.cpp
    src/script/decls/node.cpp
    src/script/decls/rule.cpp
    src/script/decls/function.cpp
    src/script/terms/operand.cpp
    src/script/terms/variable.cpp
    src/script/terms/placeholder.cpp
    src/script/terms/call.cpp
    src/script/constructs/block.cpp
    src/script/constructs/marking.cpp
    src/script/constructs/ifelse.cpp
    src/script/constructs/while.cpp
    src/script/constructs/dowhile.cpp
    src/script/constructs/for.cpp
    src/script/constructs/foreach.cpp
    src/script/constructs/switch.cpp
    src/script/constructs/control.cpp
    src/script/constructs/return.cpp
    src/script/joints/_joints.cpp
    src/script/joints/operand.cpp
    src/script/joints/functor.cpp
    src/script/joints/prefix.cpp
    src/script/joints/postfix.cpp
    src/script/joints/alternation.cpp
    src/script/joints/composition.cpp
    src/script/joints/conditional.cpp
    src/script/joints/sequencing.cpp
    src/script/joints/prioritizing.cpp
)

target_link_libraries(nparse_script
    encode
    plugin
    ${Boost_REGEX_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
)
