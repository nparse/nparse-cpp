add_library(nparse_factory
    src/static.cpp
    src/factory.cpp
    src/acceptor_pass.cpp
    src/acceptor_end.cpp
    src/acceptor_symbol.cpp
    src/acceptor_class.cpp
    src/acceptor_regex.cpp
    src/acceptor_dictionary.cpp
    src/acceptor_string.cpp
    src/dictionary/dictionary.cpp
)

include_directories(../../contrib/utf8-cpp-2.3.4/include)
include_directories(../../contrib/cmph-2.0/include)

target_link_libraries(nparse_factory
    nparse_script
    cmph-2.0
)
