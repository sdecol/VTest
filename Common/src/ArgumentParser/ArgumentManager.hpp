#pragma once

#include <memory>
#include <vector>

namespace nCommon {

class ArgumentParser;

class ArgumentManager {

public:

static void ParseArguments(int iArgc, char** iArgv, const std::vector<ArgumentParser*>& iParsers);

};

} // nCommon
