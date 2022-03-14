
cmake_minimum_required( VERSION 3.15 )

project( KSPWLOBM VERSION 1.0 LANGUAGES CXX )

find_package( benchmark CONFIG REQUIRED )

add_executable( kspwlo_bm )

target_sources( kspwlo_bm
    PRIVATE
        benchmark.cpp     
)

target_link_libraries( kspwlo_bm
    PRIVATE
        benchmark::benchmark
        benchmark::benchmark_main
        kspwlolib
)
