#include <benchmark/benchmark.h>

#include "kspwlolib/kspwlo.hpp"

constexpr static auto filename{ "../../sample/sample.gr" };
constexpr static auto sourceNode{0};
constexpr static auto destinationNode{6};
constexpr static auto min_n{1};
constexpr static auto max_n{10};
constexpr static auto step_n{1};
constexpr static auto min_threshold{5};
constexpr static auto max_threshold{5};
constexpr static auto step_threshold{1};
constexpr static auto multiplier_threshold{0.1};

void executeAlgorithm( RoadNetwork *rN, NodeID source, NodeID target, unsigned int k, double theta, const std::string& alg ){

    if( alg == "op" )
        onepass( rN, source, target, k, theta);
    else if( alg == "mp" )
        multipass( rN, source, target, k, theta);
    else if( alg == "opplus" )
        onepass_plus( rN, source, target, k, theta);
    else if( alg == "svpplus" )
        svp_plus( rN, source, target, k, theta);
    else if( alg == "esx" )
        esx( rN, source, target, k, theta);    
    else if( alg == "svp-c" )
        svp_plus_complete( rN, source, target, k, theta);
    else if( alg == "esx-c" )
        esx_complete( rN, source, target, k, theta);
}

template <class ...Args>
static void BM_Kspwlo( benchmark::State& state, Args&&... args ) 
{
    auto args_tuple = std::make_tuple(std::move(args)...);
    // Perform setup here
    auto network = RoadNetwork{ filename };

    for (auto _ : state) {
        // This code gets timed
        executeAlgorithm( &network, sourceNode, destinationNode, state.range(0), state.range(1)*multiplier_threshold, std::get<0>(args_tuple));
    }

    state.SetComplexityN(state.range(0));
}

// Register the function as a benchmark
BENCHMARK_CAPTURE(BM_Kspwlo, ONE_PASS, std::string{"op"})->ArgsProduct({ benchmark::CreateDenseRange( min_n, max_n, step_n), benchmark::CreateDenseRange( min_threshold, max_threshold, step_threshold)})
                                                        ->Unit(benchmark::kMillisecond)
                                                        ->Complexity();
BENCHMARK_CAPTURE(BM_Kspwlo, MULTI_PASS, std::string{"mp"})->ArgsProduct({ benchmark::CreateDenseRange( min_n, max_n, step_n), benchmark::CreateDenseRange( min_threshold, max_threshold, step_threshold)})
                                                        ->Unit(benchmark::kMillisecond)
                                                        ->Complexity();
BENCHMARK_CAPTURE(BM_Kspwlo, ONE_PASS_PLUS, std::string{"opplus"})->ArgsProduct({ benchmark::CreateDenseRange( min_n, max_n, step_n), benchmark::CreateDenseRange( min_threshold, max_threshold, step_threshold)})
                                                        ->Unit(benchmark::kMillisecond)
                                                        ->Complexity();
BENCHMARK_CAPTURE(BM_Kspwlo, SVP_PLUS, std::string{"svpplus"})->ArgsProduct({ benchmark::CreateDenseRange( min_n, max_n, step_n), benchmark::CreateDenseRange( min_threshold, max_threshold, step_threshold)})
                                                        ->Unit(benchmark::kMillisecond)
                                                        ->Complexity();
BENCHMARK_CAPTURE(BM_Kspwlo, SVP_PLUS_COMPLETE, std::string{"svp-c"})->ArgsProduct({ benchmark::CreateDenseRange( min_n, max_n, step_n), benchmark::CreateDenseRange( min_threshold, max_threshold, step_threshold)})
                                                        ->Unit(benchmark::kMillisecond)
                                                        ->Complexity();                                                        
BENCHMARK_CAPTURE(BM_Kspwlo, ESX, std::string{"esx"})->ArgsProduct({ benchmark::CreateDenseRange( min_n, max_n, step_n), benchmark::CreateDenseRange( min_threshold, max_threshold, step_threshold)})
                                                        ->Unit(benchmark::kMillisecond)
                                                        ->Complexity();
BENCHMARK_CAPTURE(BM_Kspwlo, ESX_COMPLETE, std::string{"esx-c"})->ArgsProduct({ benchmark::CreateDenseRange( min_n, max_n, step_n), benchmark::CreateDenseRange( min_threshold, max_threshold, step_threshold)})
                                                        ->Unit(benchmark::kMillisecond)
                                                        ->Complexity();                                                                                                                                                                                                                                

