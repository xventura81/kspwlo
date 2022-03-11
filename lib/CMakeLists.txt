
cmake_minimum_required( VERSION 3.15 )

project( KSPWLOLIB VERSION 1.0 LANGUAGES CXX )

find_package( Boost REQUIRED )

add_library( kspwlolib STATIC )

target_include_directories( kspwlolib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries( kspwlolib
    PUBLIC
        Boost::headers
)

add_subdirectory( src )