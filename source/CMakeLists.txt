add_subdirectory(libnparse_script)
add_subdirectory(libnparse_factory)
add_subdirectory(libnparse_runtime)
add_subdirectory(nparse)
add_subdirectory(nparse-test)
add_subdirectory(nparse-port)
if(NPARSE_EXAMPLES)
    add_subdirectory(examples)
endif()
