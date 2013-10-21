cmake_minimum_required(VERSION 2.6.2)

#
#	Project name and version.
#

project(nParse)
set(NPARSE_MAJOR 0)
set(NPARSE_MINOR 1)
set(NPARSE_PATCH 3)

#
#	Build configuration.
#

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
elseif(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
endif()

if(PROFILING)
	if(CMAKE_COMPILER_IS_GNUCXX)
		set(COMPILER_FLAGS "-pg ${COMPILER_FLAGS}")
	endif()
	# @todo: what about profiling for other compilers?
endif()

set(COMPILER_FLAGS "-DPIC -fPIC ${COMPILER_FLAGS}")

if(MSVC)
	add_definitions(
		-D_SCL_SECURE_NO_WARNINGS
		-D_HAS_ITERATOR_DEBUGGING=0
	)
endif()

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
	if(CMAKE_COMPILER_IS_GNUCXX)
		set(COMPILER_FLAGS "-Wall -Wno-parentheses ${COMPILER_FLAGS}")
	endif()
	add_definitions(-DDEBUG -DDEBUG_PRINT)
endif()

set(CMAKE_CXX_FLAGS "${COMPILER_FLAGS} -ansi")
set(CMAKE_C_FLAGS   "${COMPILER_FLAGS}")

#
#	Determine build number.
#

set(NPARSE_BUILD_FILE ${CMAKE_SOURCE_DIR}/build/index)

if(EXISTS ${NPARSE_BUILD_FILE})
	file(READ ${NPARSE_BUILD_FILE} NPARSE_BUILD)
else()
	set(NPARSE_BUILD 0)
endif()

math(EXPR NPARSE_BUILD 1+${NPARSE_BUILD})
file(WRITE ${NPARSE_BUILD_FILE} ${NPARSE_BUILD})

#
#	Compute derivative version-dependent constants
#	and write them to a header file.
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
	"#define NPARSE_VERSION_STR\\\n"
	"			\"${CMAKE_PROJECT_NAME} ${NPARSE_MAJOR}.${NPARSE_MINOR}.${NPARSE_PATCH}\"\\\n"
	"			\" ${CMAKE_BUILD_TYPE} #${NPARSE_BUILD} \" BOOST_COMPILER\\\n"
	"			\" ${CMAKE_SYSTEM}\"\n"
	"\n"
	"#define NPARSE_VERSION_TAG  \"${NPARSE_MAJOR}.${NPARSE_MINOR}.${NPARSE_PATCH}\"\n"
	"#define NPARSE_VERSION       ${NPARSE_VERSION}\n"
	"\n"
	"#endif /* NPARSE__VERSION_HPP_ */\n"
)

#
#	Binary output paths.
#

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

#
#	Direct dependencies: side libraries.
#

include_directories(
	./contrib/include
	./contrib/libencode-0.1.0/include
	./contrib/libplugin-0.1.0/include
	./contrib/utf8-cpp-2.3.4/include
	./contrib/yaml-cpp-0.5.0/include
	./contrib/gtest-1.6.0/include
)

add_subdirectory(contrib EXCLUDE_FROM_ALL)

#
#	Optional dependencies.
#

find_library(BOOST_IOSTREAMS boost_iostreams)
if(BOOST_IOSTREAMS MATCHES "NOTFOUND")
	set(BOOST_IOSTREAMS "")
	message("!! nParse: swap file for state pool is disabled")
else()
	add_definitions(
		-DNPARSE_SWAP_FILE
	)
endif()

find_library(BOOST_PYTHON boost_python)
find_package(PythonLibs 2.7)
if(NOT PYTHONLIBS_FOUND OR BOOST_PYTHON MATCHES "NOTFOUND")
	set(BOOST_PYTHON "")
	set(PYTHON_LIBRARIES "")
	set(PYTHON_INCLUDE_DIRS "")
	message("!! nParse: Python support is disabled")
else()
	include_directories(${PYTHON_INCLUDE_DIRS})
endif()

#
#	Original source code.
#

include_directories(./include)
add_subdirectory(./source)

#
#	Testing.
#

add_test(
	NAME nparse-test
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/data/tests
	COMMAND nparse-test --gtest_repeat=10 --gtest_shuffle=yes
)

enable_testing()

#
#	Intallation & packaging.
#

install(DIRECTORY ./include/nparse-port DESTINATION ./include)

set(CPACK_GENERATOR						"TGZ") 
set(CPACK_PACKAGE_NAME					${CMAKE_PROJECT_NAME})
set(CPACK_PACKAGE_VERSION_MAJOR			${NPARSE_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR			${NPARSE_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH			${NPARSE_PATCH})
set(CPACK_PACKAGE_VENDOR				"Alex S Kudinov")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY	"general purpose parsing framework")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER		"Alex S Kudinov <alex@nparse.com>")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE		"http://nparse.com")

include(CPack)
set(CPACK_INSTALL_CMAKE_PROJECTS	"bin;${CMAKE_PROJECT_NAME};nparse;/")
set(CPACK_INSTALL_CMAKE_PROJECTS	"${CPACK_INSTALL_CMAKE_PROJECTS};lib;${CMAKE_PROJECT_NAME};nparse-port;/")
