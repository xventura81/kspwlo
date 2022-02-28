
cmake_minimum_required(VERSION 3.15)

project(KSPWLO VERSION 1.0 LANGUAGES CXX)

find_package(   Boost 
                COMPONENTS 
                    graph
                    program_options 
                    regex
                REQUIRED )

add_executable(kspwlo)

set(MODEL model/graph.cpp)
set(TOOLS tools/dijkstra.cpp tools/astar.cpp algorithms/completeness.cpp)
set(ALGORITHMS algorithms/skyline.cpp algorithms/onepass.cpp algorithms/multipass.cpp algorithms/onepass_plus.cpp algorithms/svp_plus.cpp algorithms/esx.cpp)

target_sources( kspwlo
                PRIVATE
                    ${MODEL}
                    ${TOOLS}
                    ${ALGORITHMS}
                    main.cpp)

target_link_libraries(  kspwlo
                        PRIVATE
                        Boost::graph
                        Boost::headers
                        Boost::program_options 
                        Boost::regex)

target_compile_features(kspwlo 
                        PRIVATE 
                            cxx_std_17)