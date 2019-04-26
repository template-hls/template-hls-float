cmake_minimum_required (VERSION 3.1)
project (template_hls)

set (CMAKE_CXX_STANDARD 11)

enable_testing()

IF(CMAKE_COMPILER_IS_GNUCC)

# Unused variables happen a lot in templated code, esp. for debugability
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-variable -Wno-unused-local-typedefs")

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

set(CMAKE_CXX_FLAGS_RELWITHASSERTS "-O3 -g")

find_path(MPFR_INCLUDE_DIR mpfr.h       HINTS /usr/include /usr/local/include )
find_library(MPFR_LIBRARY NAMES mpfr    HINTS /usr/lib /usr/local/lib )

find_path(GMP_INCLUDE_DIR NAMES gmp.h   HINTS /usr/include /usr/local/include /usr/include/x86_64-linux-gnu)
find_library(GMP_LIBRARIES NAMES gmp libgmp HINTS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu)

#TODO: Is this used anywhere?
#find_library(GMPXX_LIBRARIES NAMES gmpxx libgmpxx )

include_directories( include )

set(VIVADO_HLS /cygdrive/e/Usr/Xilinx/Vivado_HLS/2016.4
        CACHE FILEPATH "Location of Vivado HLS root")

set(VIVADO_ROOT /cygdrive/e/Usr/Xilinx/Vivado/2016.4
        CACHE FILEPATH "Location of Vivado root")

set(VHLS_DIR ${VIVADO_HLS})


set(USE_AP_UINT_DEFAULT OFF)
find_path(AP_INT_INCLUDE_DIR ap_int.h 
  HINTS
    "$(VIVADO_HLS)/include"
    "/mnt/c/Usr/Xilinx/Vivado/2018.2/include"
    "${CMAKE_SOURCE_DIR}/include/contrib/ap_int/2018.3"
)
if(AP_INT_INCLUDE_DIR)
  set(USE_AP_UINT_DEFAULT ON)
endif()

set(USE_AC_UINT_DEFAULT OFF)
find_path(AC_INT_INCLUDE_DIR ac_int.h )
if(AC_INT_INCLUDE_DIR)
  set(USE_AC_UINT_DEFAULT ON)
endif()

option(USE_MASKED_UINT "Enable compilation using native C++." ON)
option(USE_AP_UINT "Enable compilation using ap_uint (implies Xilinx tool-chain)" ${USE_AP_UINT_DEFAULT})
option(USE_AC_UINT "Enable compilation using axcuint (implies Intel tool-chain)" ${USE_AC_UINT_DEFAULT})

if(AP_INT_INCLUDE_DIR)
        # NOTE: The wierd form here is because Xilinx includes a "gmp.h" in the same
        # directory as "ap_int.h". If we add this directory with "-I" it appears
        # _before_ any system directories, so the system gmp.h won't be found.
        # However, we can't just "-I /usr/lib" again, as the preprocessor doesn't
        # let you re-add built-in system directories.
        # The "-idirafter" seems to be supported in both gnu and clang frontends
        # in both contemporary and old versions.
        # HACK: This is a hack, as it adds it for all executables, regardless of 
        # whether they are using ap_uint or not. However, it only gets added if
        # the header exists so should be ok.
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -idirafter ${AP_INT_INCLUDE_DIR}")
endif()



add_custom_target(all_test_files)

# Creates an executable and adds it as a test.
function(add_executable_test PREFIX NAME )
add_executable(${NAME} ${NAME}.cpp)
target_link_libraries(${NAME} ${MPFR_LIBRARY} ${GMP_LIBRARIES} stdc++)
add_test(NAME ${PREFIX}_${NAME} COMMAND $<TARGET_FILE:${NAME}>)

    add_dependencies(all_test_files ${NAME})
endfunction(add_executable_test)

if(USE_MASKED_UINT)
add_custom_target(all_test_files_masked_uint)
endif(USE_MASKED_UINT)
if(USE_AP_UINT)
add_custom_target(all_test_files_ap_uint)
endif(USE_AP_UINT)

##########################################################################################
# Creates an executable, and also adds it with each fw_uint implementation
function(add_executable_test_with_fw_uint PREFIX NAME )

#### masked_uint
if(USE_MASKED_UINT)
add_executable(${NAME}_masked_uint ${NAME}.cpp)
target_link_libraries(${NAME}_masked_uint ${MPFR_LIBRARY} ${GMP_LIBRARIES} stdc++)
target_compile_definitions(${NAME}_masked_uint PUBLIC "-DTHLS_FW_UINT_ON_MASKED_UINT=1")
add_test(NAME ${PREFIX}_${NAME}_masked_uint COMMAND $<TARGET_FILE:${NAME}_masked_uint>)

add_dependencies(all_test_files_masked_uint ${NAME}_masked_uint)
endif(USE_MASKED_UINT)

#### ap_uint
if(USE_AP_UINT)
add_executable(${NAME}_ap_uint ${NAME}.cpp)
target_link_libraries(${NAME}_ap_uint ${MPFR_LIBRARY} ${GMP_LIBRARIES})
target_compile_definitions(${NAME}_ap_uint PUBLIC "-DTHLS_FW_UINT_ON_AP_UINT=1")
add_test(${PREFIX}_${NAME}_ap_uint ${EXECUTABLE_OUTPUT_PATH}/${NAME}_ap_uint)

add_dependencies(all_test_files_ap_uint ${NAME}_ap_uint)
endif(USE_AP_UINT)

endfunction(add_executable_test_with_fw_uint)
############################################################################################

enable_testing()

add_subdirectory(src)


