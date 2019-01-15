string(REPLACE "/cygdrive/e" "e:/" THLS_SOURCE_DIR "${CMAKE_SOURCE_DIR}" )


configure_file(dot_product/sweep_types.hpp dot_product/sweep_types.hpp COPYONLY)
configure_file(dot_product/dot_product_par4_tl.hpp dot_product/dot_product_par4_tl.hpp COPYONLY)
configure_file(dot_product/dot_product.hpp dot_product/dot_product.hpp COPYONLY)
function(add_benchmark_dot_par4 VARIANT)
configure_file(dot_product/dot_product_par4_tl_${VARIANT}.cpp dot_product/dot_product_par4_tl_${VARIANT}.cpp COPYONLY)
endfunction(add_benchmark_dot_par4)


add_benchmark_dot_par4(hls16_hls16_hls16_round )
add_benchmark_dot_par4(hls24_hls16_hls16_round )
add_benchmark_dot_par4(hls24_hls24_hls24_round )
add_benchmark_dot_par4(hls32_hls16_hls16_round )
add_benchmark_dot_par4(hls32_hls16_hls16_full )
add_benchmark_dot_par4(hls32_hls32_hls16_round )
add_benchmark_dot_par4(hls32_hls32_hls32_round )
add_benchmark_dot_par4(hls48_hls24_hls24_round )
add_benchmark_dot_par4(hls48_hls48_hls48_round )
add_benchmark_dot_par4(hls64_hls32_hls32_round )
add_benchmark_dot_par4(hls64_hls32_hls32_full )
add_benchmark_dot_par4(hls64_hls64_hls32_round )
add_benchmark_dot_par4(hls64_hls64_hls64_round )

add_benchmark_dot_par4(nat16_nat16_nat16_round )
add_benchmark_dot_par4(nat32_nat16_nat16_round )
add_benchmark_dot_par4(nat32_nat32_nat16_round )
add_benchmark_dot_par4(nat32_nat32_nat32_round )
add_benchmark_dot_par4(nat64_nat32_nat32_round )
add_benchmark_dot_par4(nat64_nat64_nat32_round )
add_benchmark_dot_par4(nat64_nat64_nat64_round )

add_benchmark_dot_par4(soft32_soft32_soft32_round )

