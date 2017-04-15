add_executable(nparse
    src/main.cpp
    src/nparse_app.cpp
)

target_link_libraries(nparse
    nparse_script
    nparse_factory
    nparse_runtime
    yaml-cpp
    ${Boost_IOSTREAMS_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_CHRONO_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
)

install(TARGETS nparse DESTINATION bin)
