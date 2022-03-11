set( MODEL model/graph.cpp )
set( TOOLS 
        algorithms/completeness.cpp 
        tools/astar.cpp 
        tools/dijkstra.cpp 
)
set( ALGORITHMS 
        algorithms/esx.cpp 
        algorithms/multipass.cpp 
        algorithms/onepass_plus.cpp 
        algorithms/onepass.cpp 
        algorithms/skyline.cpp 
        algorithms/svp_plus.cpp 
)

target_sources( kspwlolib
    PRIVATE
        ${MODEL}
        ${TOOLS}
        ${ALGORITHMS}
)