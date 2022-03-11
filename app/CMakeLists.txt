
cmake_minimum_required( VERSION 3.15 )

project( KSPWLOAPP VERSION 1.0 LANGUAGES CXX )

find_package( Boost 
    COMPONENTS 
        graph
        program_options 
        regex
    REQUIRED 
)

add_executable( kspwlo )

target_sources( kspwlo
    PRIVATE
        main.cpp
)

target_link_libraries( kspwlo
    PRIVATE
        Boost::graph
        Boost::headers
        Boost::program_options 
        Boost::regex
        kspwlolib
)

target_compile_features( kspwlo PRIVATE cxx_std_17 )

