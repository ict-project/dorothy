cmake_minimum_required(VERSION 2.6)

project(dorothy)

set(CMAKE_CXX_STANDARD 11)

add_executable(dorothy ../libict/source/utf8.cpp ../libict/source/options.cpp ../libict/source/base64.cpp global.cpp os.cpp dorothy.cpp main.cpp)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../.git)
  find_package(Git)
  if(GIT_FOUND)
    execute_process(
      COMMAND ${GIT_EXECUTABLE} describe
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      OUTPUT_VARIABLE "GIT_VERSION"
      ERROR_QUIET
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    message( STATUS "Git version: ${GIT_VERSION}" )
  else(GIT_FOUND)
    set(GIT_VERSION unknown)
  endif(GIT_FOUND)
else(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../.git)
 SET(GIT_VERSION unknown)
endif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../.git)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/git_version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/git_version.h @ONLY)

install(TARGETS dorothy RUNTIME DESTINATION bin)

#enable_testing()
#set(CMAKE_CURRENT_TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../tests)

#add_test(NAME global.help WORKING_DIRECTORY ${CMAKE_CURRENT_TESTS_DIR}  COMMAND dorothy --help)