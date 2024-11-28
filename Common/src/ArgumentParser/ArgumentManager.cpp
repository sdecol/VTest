#include "ArgumentParser/ArgumentManager.hpp"

#include <iostream>
#include <string>

#include "ArgumentParser/ArgumentParser.hpp"

namespace nCommon
{

    void ArgumentManager::ParseArguments(int iArgc, char** iArgv, const std::vector<ArgumentParser*>& iParsers)
    {
        std::string argument;

        //We start bind checking arguments received
        int arg = 1;
        while(arg < iArgc)
        {

            argument = iArgv[arg];

            bool argumentFound = false;

            for (auto* parser: iParsers)
            {
                if (parser->IsValidArgument(argument))
                {

                    parser->Activate();

                    if(parser->HasValue())
                    {
                        if (arg < iArgc - 1)
                        {
                            parser->ParseValue(iArgv[arg + 1]);
                        }
                        arg += 2;
                    }
                    else
                        arg ++;
                    argumentFound = true;

                }
            }

            if (!argumentFound)
            {
                std::cout << "Invalid argument: " << argument << std::endl;
                arg++;
            }
        }
    }


} // nCommon