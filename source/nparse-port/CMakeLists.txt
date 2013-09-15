if(EXISTS ${BOOST_PYTHON})

	add_library(nparse-port SHARED
		src/parser.cpp
		src/variable.cpp
		src/variable_iterator.cpp
		src/nparse-python.cpp
	)

else()

	add_library(nparse-port SHARED
		src/parser.cpp
		src/variable.cpp
		src/variable_iterator.cpp
	)

endif()

target_link_libraries(nparse-port
	nparse_script
	nparse_factory
	nparse_runtime
	${BOOST_PYTHON}
	${BOOST_IOSTREAMS}
	boost_filesystem
	boost_system
	${PYTHON_LIBRARIES}
)

set_target_properties(nparse-port PROPERTIES
	VERSION			"${NPARSE_MAJOR}.${NPARSE_MINOR}.${NPARSE_PATCH}"
	SOVERSION		"${NPARSE_MAJOR}.${NPARSE_MINOR}"
	PROJECT_LABEL	shared
)

install(TARGETS nparse-port DESTINATION lib)
