/*
Copyright (c) 2017 Theodoros Chondrogiannis
*/

#include "model/graph.hpp"
#include "model/graph_adaptor.h"
#include "algorithms/kspwlo.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/concept/assert.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/program_options.hpp>
#include <boost/regex.hpp>

#include <iostream>
#include <filesystem>
#include <fstream> 
#include <memory>

using namespace std;
namespace fs = std::filesystem;

int main(int argc, char **argv) {
    string graphFile = "";
    unsigned int k = 0;
    double theta = -1;
    string algo = "";
    std::unique_ptr<RoadNetwork> rN = nullptr;

    NodeID source = 0, target = 100;

	namespace po = boost::program_options;
	po::options_description desc("Allowed options");
    desc.add_options()
		("help,h", "")
		("file,f", po::value<string>(&graphFile), "")
		("source,s", po::value<unsigned int>(&source), "")
		("destination,d", po::value<unsigned int>(&target), "")
		("paths,k", po::value<unsigned int>(&k), "")
		("threshold,t", po::value<double>(&theta), "")
		("algorithm,a", po::value<string>(&algo), "");

	po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    

	if (vm.count("help")) {
		cout << desc << "\n";
		return 1;
	}
	
	//Input checking	
	if(graphFile == "" ) {
    	cerr << "Wrong arguments. Define graph file correctly." << endl;
    	exit(1);
    }
    
	if(k < 1) {
    	cerr << "Define k between [1,+inf)" << endl;
    	exit(2);
    }
    
    if(theta < 0 || theta > 1) {
    	cerr << "Define theta between [0,1]" << endl;
    	exit(3);
    }
    
    if(source == target) {
    	cerr << "Source and target are the same node" << endl;
    	exit(4);
    }
    
    cout << "[LOG] Network: " << graphFile << endl;
    cout << "[LOG] Source node: " << source << endl;
    cout << "[LOG] Target node: " << target << endl;
    cout << "[LOG] Number of paths k: " << k << endl;
    cout << "[LOG] Similarity threshold theta: " << theta << endl;
    cout << "[LOG] Algorithm: " << algo << endl;
    
    // Loading road network
    rN = std::make_unique<RoadNetwork>(graphFile.c_str());
    
    vector<Path> result;
    pair<vector<Path>,double> completeResult;

	if(boost::iequals(algo, "op")) {
    	result = onepass(rN.get(),source,target,k,theta);
    }
    else if(boost::iequals(algo, "mp")) {
    	result = multipass(rN.get(),source,target,k,theta);
    }
    else if(boost::iequals(algo, "opplus")) {
    	result = onepass_plus(rN.get(),source,target,k,theta);
    }
    else if(boost::iequals(algo, "svpplus")) {
    	result = svp_plus(rN.get(),source,target,k,theta);
    }
    else if(boost::iequals(algo, "esx")) {
    	result = esx(rN.get(),source,target,k,theta);
    }
    else if(boost::iequals(algo, "svp-c")) {
   		completeResult = svp_plus_complete(rN.get(),source,target,k,theta);
   		result = completeResult.first;
   	}
    else if(boost::iequals(algo, "esx-c")) {
   		completeResult = esx_complete(rN.get(),source,target,k,theta);
   		result = completeResult.first;
   	}

    if(result.empty()) {
        cout << "No solution found!" << endl;
        return 0;
    }

    cout << source << "\t" << target << "\t[" << result[0].length;
    for(unsigned int j = 1;j<result.size();j++) {
    	cout << "," << result[j].length;
    }
    cout << "]" << endl;

    BOOST_CONCEPT_ASSERT(( boost::VertexAndEdgeListGraphConcept<Path> ));
    BOOST_CONCEPT_ASSERT(( boost::VertexIndexGraphConcept<Path> ));

    cout << endl;
    auto index = int{0};
    for( const auto& path : result ) {
        cout << "--Path " << index++ << "--" << endl;
        boost::write_graphviz(cout, path);
        cout << endl;
    }

    index = 0;
    fs::path baseFilePath{graphFile};
    for( const auto& path : result ) {
        fs::path resFilePath{baseFilePath};
        resFilePath.replace_filename(baseFilePath.stem().string() + "_" + std::to_string(index++));
        std::ofstream stream{resFilePath};
        boost::write_graphviz(stream, path);
    }

    return 0;
}
