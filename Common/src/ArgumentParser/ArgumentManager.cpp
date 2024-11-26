#include "ArgumentParser/ArgumentManager.hpp"

#include <iostream>
#include <string>

#include "ArgumentParser/ArgumentParser.hpp"

namespace nCommon {

void ArgumentManager::ParseArguments(int iArgc, char **iArgv, const std::vector<ArgumentParser *> &iParsers)
{
    std::string argument;

    //We start bind checking arguments received
    for(int i = 1;i < iArgc; i+=2) {

        argument = iArgv[i];

        bool argumentFound = false;

        for (auto *parser: iParsers) {
            if (parser->IsValidArgument(argument)) {

                parser->Activate();
                if (i < iArgc - 1)
                    parser->ParseValue(iArgv[i + 1]);

                argumentFound = true;

            }
        }

        if (!argumentFound)
            std::cout << "Invalid argument: " << argument << std::endl;
    }
}


} // nCommon