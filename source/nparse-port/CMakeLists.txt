if(${PYTHON_LIBRARIES})

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
	${Boost_PYTHON_LIBRARY}
	${Boost_IOSTREAMS_LIBRARY}
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
	${PYTHON_LIBRARIES}
)

set_target_properties(nparse-port PROPERTIES
	VERSION			"${NPARSE_MAJOR}.${NPARSE_MINOR}.${NPARSE_PATCH}"
	SOVERSION		"${NPARSE_MAJOR}.${NPARSE_MINOR}"
	PROJECT_LABEL	shared
)

install(TARGETS nparse-port DESTINATION lib)
