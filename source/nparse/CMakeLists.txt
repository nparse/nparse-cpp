add_executable(nparse
	src/main.cpp
	src/nparse_app.cpp
)

target_link_libraries(nparse
	nparse_script
	nparse_factory
	nparse_runtime
	yaml-cpp
	${BOOST_IOSTREAMS}
	boost_program_options
	boost_chrono
	boost_filesystem
	boost_system
)

install(TARGETS nparse DESTINATION bin)
