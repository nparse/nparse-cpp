cmake_minimum_required(VERSION 2.8)

set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/utils/cmake)

#
#   Project name and version.
#

project(nParse)
set(NPARSE_MAJOR 0)
set(NPARSE_MINOR 1)
set(NPARSE_PATCH 9)

#
#   Build configuration.
#

add_definitions(-DNPARSE_CMPH)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
elseif(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(PROFILING)
    if(CMAKE_COMPILER_IS_GNUCXX)
        set(COMPILER_FLAGS "-pg ${COMPILER_FLAGS}")
    endif()
    # TODO: what about profiling for other compilers?
endif()

set(COMPILER_FLAGS "-DPIC -fPIC ${COMPILER_FLAGS}")

if(MSVC)
    add_definitions(
        -D_SCL_SECURE_NO_WARNINGS
        -D_HAS_ITERATOR_DEBUGGING=0
    )
endif()

if(${CMAKE_BUILD_TYPE} MATCHES "^(Debug|Coverage)$")
    if(CMAKE_COMPILER_IS_GNUCXX)
        set(COMPILER_FLAGS "-Wall -Wno-parentheses ${COMPILER_FLAGS}")
    endif()
    set(COMPILER_FLAGS "-g -O0 ${COMPILER_FLAGS}")
    add_definitions(-DDEBUG -DDEBUG_PRINT)
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "Coverage")
    find_program(LCOV lcov)
    find_program(GENHTML genhtml)

    add_custom_target(
        coverage
        COMMAND nparse-test
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/data/tests
    )

    add_custom_target(
        TARGET coverage
        PRE_BUILD
        COMMAND ${LCOV} -q -z -d ./
    )

    set(cov_report "coverage-report")
    set(cov_stage0 "${CMAKE_BINARY_DIR}/${cov_report}.data.0")
    set(cov_stage1 "${CMAKE_BINARY_DIR}/${cov_report}.data.1")
    add_custom_command(
        TARGET coverage
        POST_BUILD
        COMMAND ${LCOV} -q -c -d ./ -o ${cov_stage0}
        COMMAND ${LCOV} -q -e ${cov_stage0} '${CMAKE_SOURCE_DIR}/include/*' '${CMAKE_SOURCE_DIR}/source/*' -o ${cov_stage1}
        COMMAND ${GENHTML} -q -o ${cov_report} ${cov_stage1}
        COMMAND ${CMAKE_COMMAND} -E remove ${cov_stage0} ${cov_stage1}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

    set(COMPILER_FLAGS "-fprofile-arcs -ftest-coverage ${COMPILER_FLAGS}")
endif()

if(${CMAKE_CXX_COMPILER_ID} MATCHES "^([aA]pple)?[cC]lang$")
    set(COMPILER_FLAGS "${COMPILER_FLAGS} -ftemplate-depth=65536")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMPILER_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_FLAGS} -std=c++03")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

#
#   Determine build number.
#

set(BUILD_NUMBER_FILE ${CMAKE_SOURCE_DIR}/.build-number)

if(EXISTS ${BUILD_NUMBER_FILE})
    file(READ ${BUILD_NUMBER_FILE} BUILD_NUMBER)
else()
    set(BUILD_NUMBER 0)
endif()

math(EXPR BUILD_NUMBER 1+${BUILD_NUMBER})
file(WRITE ${BUILD_NUMBER_FILE} ${BUILD_NUMBER})

#
#   Compute derivative version-dependent constants
#   and write them to a header file.
#

math(EXPR NPARSE_VERSION ${NPARSE_MAJOR}*100000+${NPARSE_MINOR}*1000+${NPARSE_PATCH})
file(
    WRITE include/nparse/_version.hpp
    "#ifndef NPARSE__VERSION_HPP_\n"
    "#define NPARSE__VERSION_HPP_\n"
    "\n"
    "/* This is an automatically generated file, do not edit. */\n"
    "\n"
    "#include <boost/config.hpp>\n"
    "\n"
    "#define NPARSE_MAJOR         ${NPARSE_MAJOR}\n"
    "#define NPARSE_MINOR         ${NPARSE_MINOR}\n"
    "#define NPARSE_PATCH         ${NPARSE_PATCH}\n"
    "\n"
    "#define NPARSE_VERSION       ${NPARSE_VERSION}\n"
    "#define NPARSE_VERSION_TAG   \"${NPARSE_MAJOR}.${NPARSE_MINOR}.${NPARSE_PATCH}\"\n"
    "\n"
    "#define NPARSE_STR_(x) #x\n"
    "#define NPARSE_STR(x) NPARSE_STR_(x)\n"
    "\n"
    "#define NPARSE_VERSION_STR  \\\n"
    "        \"nParse \" NPARSE_VERSION_TAG \" build ${BUILD_NUMBER} (${CMAKE_BUILD_TYPE})\\n\"  \\\n"
    "        \"built with \" BOOST_COMPILER \"\\n\"  \\\n"
    "        \"on ${CMAKE_SYSTEM}\\n\"  \\\n"
    "        \"using C++Boost version \" NPARSE_STR(BOOST_VERSION)\n"
    "\n"
    "#endif /* NPARSE__VERSION_HPP_ */\n"
)

#
#   Binary output paths.
#

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

#
#   Direct dependencies: side libraries.
#

find_package(
    Boost
    REQUIRED
    COMPONENTS
        chrono
        filesystem
        iostreams
        program_options
        python
        regex
        system
)

find_package(
    PythonLibs 2.7
)

include_directories(
    ${Boost_INCLUDE_DIRS}
    ./contrib/include
    ./contrib/libencode-0.1.0/include
    ./contrib/libplugin-0.2.0/include
    ./contrib/utf8-cpp-2.3.4/include
    ./contrib/yaml-cpp-0.5.3/include
    ./contrib/gtest-1.8.0/googlemock/include
    ./contrib/gtest-1.8.0/googletest/include
)

add_subdirectory(contrib EXCLUDE_FROM_ALL)

#
#   Optional dependencies.
#

if(Boost_IOSTREAMS_FOUND STREQUAL "ON")
    add_definitions(-DNPARSE_SWAP_FILE)
else()
    message("!! nParse: swap file for state pool is disabled")
endif()

if(Boost_PYTHON_FOUND STREQUAL "ON" AND PYTHONLIBS_FOUND)
    include_directories(${PYTHON_INCLUDE_DIRS})
endif()

#
#   Original source code.
#

include_directories(./include)
add_subdirectory(./source)

#
#   Testing.
#

add_test(
    NAME nparse-test
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/data/tests
    COMMAND nparse-test --gtest_repeat=3 --gtest_shuffle=yes
)

enable_testing()

#
#   Intallation & packaging.
#

install(DIRECTORY ./include/nparse-port DESTINATION include)

set(CPACK_GENERATOR                     "TGZ")
set(CPACK_PACKAGE_NAME                  ${CMAKE_PROJECT_NAME})
set(CPACK_PACKAGE_VERSION_MAJOR         ${NPARSE_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR         ${NPARSE_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH         ${NPARSE_PATCH})
set(CPACK_PACKAGE_VENDOR                "Alex Kudinov")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY   "general purpose parsing framework")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER     "Alex Kudinov <alex.s.kudinov@gmail.com>")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE       "https://nparse.com")

include(CPack)
set(CPACK_INSTALL_CMAKE_PROJECTS
    "bin;${CMAKE_PROJECT_NAME};nparse;/"
    "lib;${CMAKE_PROJECT_NAME};nparse-port;/"
)
