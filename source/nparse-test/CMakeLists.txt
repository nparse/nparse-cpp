add_executable(nparse-test
    src/test_core.cpp
    src/test_dsel_arithmetic.cpp
    src/test_dsel_casts.cpp
    src/test_libencode.cpp
    src/test_ndl.cpp
    src/test_range_add.cpp
    src/test_sas.cpp
    src/test_script.cpp
)

target_link_libraries(nparse-test
    nparse-port
    yaml-cpp
    gtest
    gtest_main
)
