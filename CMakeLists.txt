
cmake_minimum_required( VERSION 3.15 )

project( KSPWLO VERSION 1.0 LANGUAGES CXX )

add_subdirectory( app )
add_subdirectory( lib )

option( BUILD_BENCHMARKS "Enable benchmark build." OFF )
if( BUILD_BENCHMARKS )
    add_subdirectory( benchmark )
endif()
