cmake_minimum_required (VERSION 3.2)
project (template_hls)

set (CMAKE_CXX_STANDARD 11)

enable_testing()

IF(CMAKE_COMPILER_IS_GNUCC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og")

# It is common to label loops in HLS code, but it isn't
# a problem if we don't use them.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-label")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

find_path(MPFR_INCLUDE_DIR mpfr.h       PATHS /usr/include /usr/local/include )

find_library(MPFR_LIBRARY NAMES mpfr    PATHS /usr/lib /usr/local/lib )

find_path(GMP_INCLUDE_DIR NAMES gmp.h)

find_library(GMP_LIBRARIES NAMES gmp libgmp )

find_library(GMPXX_LIBRARIES NAMES gmpxx libgmpxx )

include_directories( ../include )

# Creates an executable and adds it as a test.
function(add_executable_test PREFIX NAME)
add_executable(${NAME} ${NAME}.cpp)
target_link_libraries(${NAME} ${MPFR_LIBRARY} ${GMP_LIBRARIES})
add_test(${PREFIX}_${NAME} ${EXECUTABLE_OUTPUT_PATH}/${NAME})

endfunction(add_executable_test)

enable_testing()

add_subdirectory(fp_ops)


