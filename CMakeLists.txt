project(scalc CXX)
cmake_minimum_required(VERSION 2.8)

find_program(PROGRAM_FLEX flex)
if (PROGRAM_FLEX)
	message("-- Looking program flex - found")
else (PROGRAM_FLEX)
	message(SEND_ERROR "Not found program flex")
endif(PROGRAM_FLEX)

find_program(PROGRAM_BISON bison)
if (PROGRAM_BISON)
	message("-- Looking program bison - found")
else (PROGRAM_BISON)
	message(SEND_ERROR "Not found program bison")
endif(PROGRAM_BISON)

find_program(PROGRAM_SOAPCPP2 soapcpp2)
if (PROGRAM_SOAPCPP2)
	message("-- Looking program soapcpp2 - found")
else (PROGRAM_SOAPCPP2)
	message(SEND_ERROR "Not found program soapcpp2")
endif(PROGRAM_SOAPCPP2)

find_program(PROGRAM_APACHE2 apache2 )
if (NOT PROGRAM_APACHE2)
	message("Not found apache, required for testing")
endif(NOT PROGRAM_APACHE2)

find_file(MODULE_APACHE2_FASTCGI mod_fastcgi.so 
	/usr/lib/apache2/modules 
	/usr/lib/apache/modules 
	/usr/local/lib/apache2/modules 
	/usr/local/lib/apache/modules
)
if (NOT MODULE_APACHE2_FASTCGI)
	message("Not found module apache mod_fastcgi.so, required for testing")
endif(NOT MODULE_APACHE2_FASTCGI)

include(CheckIncludeFileCXX)

check_include_file_cxx(boost/thread/thread.hpp HAVE_BOOST_THREAD_HPP)
if (NOT HAVE_BOOST_THREAD_HPP)
	message(SEND_ERROR "Not found header file boost/thread/thread.hpp")
endif(NOT HAVE_BOOST_THREAD_HPP)

check_include_file_cxx(boost/thread/shared_mutex.hpp HAVE_BOOST_SHARED_MUTEX_HPP)
if (NOT HAVE_BOOST_SHARED_MUTEX_HPP)
	message(SEND_ERROR "Not found header file boost/thread/shared_mutex.hpp")
endif(NOT HAVE_BOOST_SHARED_MUTEX_HPP)

check_include_file_cxx(fastcgi.h HAVE_FASTCGI_H)
if (NOT HAVE_FASTCGI_H)
	message(SEND_ERROR "Not found header file fastcgi.h")
endif(NOT HAVE_FASTCGI_H)

find_library(HAVE_BOOST_THREAD boost_thread)
if (HAVE_BOOST_THREAD)
	message("-- Looking library boost_thread - found")
	set(EXTRA_LIBS ${EXTRA_LIBS} boost_thread)
else(HAVE_BOOST_THREAD)
	message(SEND_ERROR "Not found library boost_thread")
endif(HAVE_BOOST_THREAD)

find_library(HAVE_FCGI fcgi)
if (HAVE_FCGI)
	message("-- Looking library fcgi - found")
	set(EXTRA_LIBS ${EXTRA_LIBS} fcgi)
else(HAVE_FCGI)
	message(SEND_ERROR "Not found library fcgi")
endif(HAVE_FCGI)

configure_file (
  "${PROJECT_SOURCE_DIR}/tests/launch.in"
  "${PROJECT_BINARY_DIR}/tests/launch"
)

configure_file (
  "${PROJECT_SOURCE_DIR}/tests/httpd.conf.in"
  "${PROJECT_BINARY_DIR}/tests/httpd.conf"
)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/calc.nsmap 
		${CMAKE_CURRENT_BINARY_DIR}/calc.wsdl
		${CMAKE_CURRENT_BINARY_DIR}/ns.xsd 
		${CMAKE_CURRENT_BINARY_DIR}/soapcalcProxy.cpp 
		${CMAKE_CURRENT_BINARY_DIR}/soapcalcProxy.h 
		${CMAKE_CURRENT_BINARY_DIR}/soapcalcService.cpp
		${CMAKE_CURRENT_BINARY_DIR}/soapcalcService.h 
		${CMAKE_CURRENT_BINARY_DIR}/soapC.cpp 
		${CMAKE_CURRENT_BINARY_DIR}/soapH.h 
		${CMAKE_CURRENT_BINARY_DIR}/soapStub.h 
		${CMAKE_CURRENT_BINARY_DIR}/calc.evalute.req.xml  
		${CMAKE_CURRENT_BINARY_DIR}/calc.evalute.res.xml
	COMMAND ${PROGRAM_SOAPCPP2} -i calc.h
	DEPENDS calc.h
)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cc 
		${CMAKE_CURRENT_BINARY_DIR}/parser.tab.hh 
		${CMAKE_CURRENT_BINARY_DIR}/location.hh 
		${CMAKE_CURRENT_BINARY_DIR}/position.hh 
		${CMAKE_CURRENT_BINARY_DIR}/stack.hh
	COMMAND ${PROGRAM_BISON} parser.yy
	DEPENDS parser.yy
)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.cc
	COMMAND ${PROGRAM_FLEX} scanner.ll
	DEPENDS scanner.ll
)

add_executable(calcserver 
	calcserver.cpp 
	${CMAKE_CURRENT_BINARY_DIR}/soapcalcService.h
	${CMAKE_CURRENT_BINARY_DIR}/soapcalcService.cpp 
	${CMAKE_CURRENT_BINARY_DIR}/calc.nsmap 
	calccontext.h
	calccontext.cpp
	stdsoap2.cpp 
	stdsoap2.h
	${CMAKE_CURRENT_BINARY_DIR}/soapC.cpp 
	${CMAKE_CURRENT_BINARY_DIR}/soapH.h
	driver.cpp 
	driver.h 
	${CMAKE_CURRENT_BINARY_DIR}/parser.tab.hh
	${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cc
	scanner.h
	${CMAKE_CURRENT_BINARY_DIR}/lex.yy.cc
	session.h
	session.cpp
	controlses.h
	controlses.cpp
)
target_link_libraries(calcserver ${EXTRA_LIBS})
set_target_properties(calcserver PROPERTIES COMPILE_FLAGS "-DWITH_COOKIES -DWITH_FASTCGI")


add_executable(calcclient
	calcclient.cpp 
	${CMAKE_CURRENT_BINARY_DIR}/soapcalcProxy.h
	${CMAKE_CURRENT_BINARY_DIR}/soapcalcProxy.cpp 
	${CMAKE_CURRENT_BINARY_DIR}/calc.nsmap
	stdsoap2.h
	stdsoap2.cpp
	${CMAKE_CURRENT_BINARY_DIR}/soapH.h
	${CMAKE_CURRENT_BINARY_DIR}/soapC.cpp
	
)
set_target_properties(calcclient PROPERTIES COMPILE_FLAGS -DWITH_COOKIES)

add_executable(test_speed
	test_speed.cpp 
	${CMAKE_CURRENT_BINARY_DIR}/soapcalcProxy.h
	${CMAKE_CURRENT_BINARY_DIR}/soapcalcProxy.cpp 
	${CMAKE_CURRENT_BINARY_DIR}/calc.nsmap
	stdsoap2.h
	stdsoap2.cpp
	${CMAKE_CURRENT_BINARY_DIR}/soapH.h
	${CMAKE_CURRENT_BINARY_DIR}/soapC.cpp
)
set_target_properties(test_speed PROPERTIES COMPILE_FLAGS -DWITH_COOKIES)

install(TARGETS calcserver DESTINATION cgi-bin)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/calc.wsdl DESTINATION htdocs)

enable_testing()

add_test( test_speed ./tests/launch )

# build a CPack driven installer package
include (InstallRequiredSystemLibraries)
set (CPACK_RESOURCE_FILE_LICENSE  
     "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set (CPACK_PACKAGE_VERSION_MAJOR 0)
set (CPACK_PACKAGE_VERSION_MINOR 0)
set (CPACK_PACKAGE_VERSION_PATCH 1)
set (CPACK_SOURCE_GENERATOR "TGZ")
set (CPACK_SOURCE_PACKAGE_FILE_NAME
  "${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

set ( CPACK_SOURCE_IGNORE_FILES
    "CMakeFiles;Testing;_CPack_Packages;CMakeCache.txt;Makefile;install_manifest.txt;.cmake$;.wsdl$;.cc$;.hh$;httpd.conf$;launch$;.cproject;.project;.xml$;.nsmap$;calcclient$;calcserver$;test_speed$;.xsd$;scalc-.+-.+$;soapC;soapH;soapcalcProxy;soapcalcService;soapStub;${CPACK_SOURCE_IGNORE_FILES}" )
include (CPack)


	 
	
	