#pragma once

#include "kspwlolib/graph.hpp"

#include <filesystem>
#include <string>
#include <variant>
#include <vector>

struct Dot {};
struct Plain {};

class Reporter
{
public:
    Reporter(NodeID source, NodeID target, const std::vector<Path> &results, const std::string& format );

    void printSummary();
    void printResultDetails();
    void exportToFile( const std::filesystem::path& outputFilePath );

private:
    NodeID mSource;
    NodeID mTarget;
    std::vector<Path> mResults;
    std::variant<Dot, Plain> mFormat;

    std::vector<std::filesystem::path> exportToFileInternal( const std::filesystem::path& outputFilePath );
};