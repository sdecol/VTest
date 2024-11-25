#include <complex>

#include "ArgumentParser.hpp"

#include <iostream>
#include <vector>

int main(int argc, char** argv)
{
    std::vector<nCommon::ArgumentParser*> parsers;

    nCommon::ArgumentParser portParser("port", nCommon::ArgumentParser::Number, "4242");
    portParser.AddValidArguments({"-p", "--port"});
    parsers.push_back(&portParser);

    nCommon::ArgumentParser limitParser("limit", nCommon::ArgumentParser::Number, "-1");
    limitParser.AddValidArguments({"-l", "--limit"});
    parsers.push_back(&limitParser);

    nCommon::ArgumentParser boundsParser("bounds", nCommon::ArgumentParser::Range, "1,100");
    boundsParser.AddValidArguments({"-b", "--bounds"});
    parsers.push_back(&boundsParser);

    std::string argument;

    //We start bind checking arguments received
    for(int i = 1;i < argc; i+=2) {

        argument = argv[i];

        bool argumentFound = false;

        for(auto* parser : parsers)
        {
            if(parser->IsValidArgument(argument)) {

                if(i < argc-1)
                    parser->ParseValue(argv[i+1]);

                argumentFound = true;

            }
        }

        if(!argumentFound)
            std::cout<<"Invalid argument: "<<argument<<std::endl;
    }

    return 0;
}