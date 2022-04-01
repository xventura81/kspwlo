#include "reporter.h"

#include "graph_adaptor.h"

#include <boost/concept/assert.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/graphviz.hpp>

BOOST_CONCEPT_ASSERT(( boost::VertexAndEdgeListGraphConcept<Path> ));
BOOST_CONCEPT_ASSERT(( boost::VertexIndexGraphConcept<Path> ));

// helper type for the visitor
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace fs = std::filesystem;

Reporter::Reporter( NodeID source, NodeID target, const std::vector<Path>& results, const std::string& format )
    : mSource{ source }
    , mTarget{ target }
    , mResults{ results }
{
    if( format == "dot" )
        mFormat = Dot{};
    if( format == "plain" )
        mFormat = Plain{};
}

void Reporter::printSummary()
{
    if( mResults.empty() )
        return;

    std::cout << mSource << "\t" << mTarget << "\t[" << mResults[0].length;
    for(unsigned int j = 1;j<mResults.size();j++) {
    	std::cout << "," << mResults[j].length;
    }
    std::cout << "]" << std::endl;
    std::cout << std::endl;
}

void Reporter::printResultDetails()
{
    auto index = int{ 0 };
    for( const auto& path : mResults ) {
        std::cout << "--Path " << index++ << "--" << std::endl;
        std::visit( overloaded {
            []( auto ) { },
            [&path]( const Dot& ) { boost::write_graphviz(std::cout, path); },
            [&path]( const Plain& ) {
                for( const auto& node : path.nodes ) {
                    std::cout << node << " ";
                }
             }
        }, mFormat );
        std::cout << std::endl;
    }

    std::cout << std::endl;
}

void Reporter::exportToFile( const std::filesystem::path& outputFilePath )
{
    const auto generatedFiles = exportToFileInternal(outputFilePath);
    if( generatedFiles.empty() )
        return;

    std::cout << "Generated files:" << std::endl;
    auto index = int{ 0 };
    for( const auto& path : generatedFiles ) {
        std::cout << path << std::endl;
    }
    std::cout << std::endl;
}

std::vector<std::filesystem::path> Reporter::exportToFileInternal( const std::filesystem::path& outputFilePath )
{
    std::vector<fs::path> generatedFiles;

    std::visit( overloaded {
        []( auto ) { },
        [this, &generatedFiles, &outputFilePath]( const Dot& ) { 
            auto index = int{ 0 };
            for( const auto& path : mResults ) {
                fs::path resFilePath{ outputFilePath };
                resFilePath.replace_filename( outputFilePath.stem().string() + "_path-" + std::to_string( index++ ) + ".dot" );
                std::ofstream stream{ resFilePath };
                boost::write_graphviz( stream, path );
                generatedFiles.push_back(resFilePath);
            }
         },
        [this, &generatedFiles, &outputFilePath]( const Plain& ) {
            fs::path resFilePath{ outputFilePath };
            resFilePath.replace_extension( "txt" );
            std::ofstream stream{ resFilePath };
            for( const auto& path : mResults ) {
                for( const auto& node : path.nodes ) {
                    stream << node << " ";
                }
                stream << std::endl;
            }
            generatedFiles.push_back(resFilePath);
        }
    } , mFormat );

    return generatedFiles;
}
