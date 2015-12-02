cmake_minimum_required (VERSION 3.2)
project (template_hls)

set (CMAKE_CXX_STANDARD 11)

IF(CMAKE_COMPILER_IS_GNUCC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# It is common to label loops in HLS code, but it isn't
# a problem if we don't use them.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-label")
ENDIF(CMAKE_COMPILER_IS_GNUCC)


include_directories( ../include )

# Creates an executable and adds it as a test.
function(add_executable_test PREFIX NAME)
add_executable(${NAME}_test ${NAME}_test.cpp)
add_test(${PREFIX}_${NAME}_test ${EXECUTABLE_OUTPUT_PATH}/${NAME}_test)

endfunction(add_executable_test)

enable_testing()

add_subdirectory(fp_ops)


