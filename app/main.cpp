/*
Copyright (c) 2017 Theodoros Chondrogiannis
*/

#include "kspwlolib/graph.hpp"
#include "kspwlolib/kspwlo.hpp"
#include "reporter.h"

#include <boost/algorithm/string.hpp>
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
    string outputFormat = "dot";
    string outputPath = "";
    bool quiet = false;
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
		("algorithm,a", po::value<string>(&algo), "")
        ("outputFormat,o", po::value<string>(&outputFormat), "")
        ("outputPath,p", po::value<string>(&outputPath), "")
        ("quiet,q", "");

	po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    quiet = vm.count("quiet");

    if (vm.count("help")) {
		cout << desc << "\n";
		return 1;
	}
	
	//Input checking	
	if(graphFile == "" ) {
        if( !quiet )
    	    cerr << "Wrong arguments. Define graph file correctly." << endl;
    	exit(1);
    }
    
	if(k < 1) {
        if( !quiet )
    	    cerr << "Define k between [1,+inf)" << endl;
    	exit(2);
    }
    
    if(theta < 0 || theta > 1) {
        if( !quiet )
    	    cerr << "Define theta between [0,1]" << endl;
    	exit(3);
    }
    
    if(source == target) {
        if( !quiet )
    	    cerr << "Source and target are the same node" << endl;
    	exit(4);
    }
    
    if( !quiet ) {
        cout << "[LOG] Network: " << graphFile << endl;
        cout << "[LOG] Source node: " << source << endl;
        cout << "[LOG] Target node: " << target << endl;
        cout << "[LOG] Number of paths k: " << k << endl;
        cout << "[LOG] Similarity threshold theta: " << theta << endl;
        cout << "[LOG] Algorithm: " << algo << endl;
    }
    
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
        if( !quiet )
            cout << "No solution found!" << endl;
        return 0;
    }

    Reporter reporter{ source, target, result, outputFormat };
    if( !quiet ) {
        reporter.printSummary();
        reporter.printResultDetails();
    }

    const auto basePath = std::invoke( [&outputPath, &graphFile](){
        if( outputPath.empty() )
            return fs::path{ graphFile }.parent_path();
        return fs::path{ outputPath };
    } );

    const auto baseFilePath = basePath / ( fs::path{ graphFile }.stem().string() 
                                        + "_nodes" + std::to_string( source ) + "-" + std::to_string(target) 
                                        + "_thres-" + std::to_string(theta) 
                                        + "_algo-" + algo
                                        + ".dummy" );

    reporter.exportToFile( baseFilePath, quiet );

    return 0;
}
