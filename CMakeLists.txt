cmake_minimum_required (VERSION 3.1)
project (template_hls)

set (CMAKE_CXX_STANDARD 11)

enable_testing()

IF(CMAKE_COMPILER_IS_GNUCC)

# Unused variables happen a lot in templated code, esp. for debugability
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-variable")

# We use various pragmas to control HLS generation that g++ doesn't know
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")

# It is common to label loops in HLS code, but it isn't
# a problem if we don't use them.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-label")

# This is exceptionally useful for finding edge-cases due to shifts
# and other weirdness in templated data-paths
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
# Try to speed-up compilation if possible
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og")
endif()

ENDIF(CMAKE_COMPILER_IS_GNUCC)

find_path(MPFR_INCLUDE_DIR mpfr.h       PATHS /usr/include /usr/local/include )

find_library(MPFR_LIBRARY NAMES mpfr    PATHS /usr/lib /usr/local/lib )

find_path(GMP_INCLUDE_DIR NAMES gmp.h)

find_library(GMP_LIBRARIES NAMES gmp libgmp )

find_library(GMPXX_LIBRARIES NAMES gmpxx libgmpxx )


add_custom_target(all_test_files)

include_directories( include )

# Creates an executable and adds it as a test.
function(add_executable_test PREFIX NAME )
add_executable(${NAME} ${NAME}.cpp)
target_link_libraries(${NAME} ${MPFR_LIBRARY} ${GMP_LIBRARIES} stdc++)
add_test(NAME ${PREFIX}_${NAME} COMMAND $<TARGET_FILE:${NAME}>)

    add_dependencies(all_test_files ${NAME})
endfunction(add_executable_test)

set(VIVADO_HLS /cygdrive/e/Usr/Xilinx/Vivado_HLS/2016.4
        CACHE FILEPATH "Location of Vivado HLS root")

set(VIVADO_ROOT /cygdrive/e/Usr/Xilinx/Vivado/2016.4
        CACHE FILEPATH "Location of Vivado root")

set(VHLS_DIR ${VIVADO_HLS})


find_path(AP_INT_INCLUDE_DIR ap_int.h 
  HINTS
    "${CMAKE_SOURCE_DIR}/include/xilinx"
    "$(VIVADO_HLS)/include"
    "/mnt/c/Usr/Xilinx/Vivado/2018.2/include"
)

add_custom_target(all_test_files_masked_uint)

# Creates an executable, and also adds it with each fw_uint implementation
function(add_executable_test_with_fw_uint PREFIX NAME )

add_executable_test(${PREFIX} ${NAME})

add_executable(${NAME}_ap_uint ${NAME}.cpp)
target_link_libraries(${NAME}_ap_uint ${MPFR_LIBRARY} ${GMP_LIBRARIES})
target_compile_definitions(${NAME}_ap_uint PUBLIC "-DTHLS_FW_UINT_ON_AP_UINT=1")
target_include_directories(${NAME}_ap_uint PUBLIC "${AP_INT_INCLUDE_DIR}")
add_test(${PREFIX}_${NAME}_ap_uint ${EXECUTABLE_OUTPUT_PATH}/${NAME}_ap_uint)

add_executable(${NAME}_masked_uint ${NAME}.cpp)
target_link_libraries(${NAME}_masked_uint ${MPFR_LIBRARY} ${GMP_LIBRARIES} stdc++)
target_compile_definitions(${NAME}_masked_uint PUBLIC "-DTHLS_FW_UINT_ON_MASKED_UINT=1")
add_test(NAME ${PREFIX}_${NAME}_masked_uint COMMAND $<TARGET_FILE:${NAME}_masked_uint>)

add_dependencies(all_test_files_masked_uint ${NAME}_masked_uint)

endfunction(add_executable_test_with_fw_uint)

enable_testing()

add_subdirectory(src)


