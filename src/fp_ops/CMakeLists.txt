set(MPFR_MPIR_DIR ../MpfrMpir/Static)

message(${MPFR_MPIR_DIR})

find_path(MPFR_INCLUDE_DIR mpfr.h PATHS
 /usr/include /usr/local/include
 ${MPFR_MPIR_DIR}
 ${MPFR_MPIR_DIR}/Mpfr
 )

find_library(MPFR_LIBRARY NAMES mpfr  PATHS
    /usr/lib /usr/local/lib
    ${MPFR_MPIR_DIR}/Lib64
)


include_directories( ${MPFR_INCLUDE_DIR} )

add_executable(test_add test_add.cpp)
target_link_libraries(test_add ${MPFR_LIBRARY})
