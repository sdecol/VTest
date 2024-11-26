#include "ArgumentParser/ArgumentParser.hpp"
#include "ArgumentParser/BoundsArgumentParser.hpp"
#include "ArgumentParser/IntArgumentParser.hpp"
#include "ServerApplication/ServerApplication.hpp"

#include <iostream>
#include <vector>

int main(int argc, char** argv)
{
    std::vector<nCommon::ArgumentParser*> parsers;

    nCommon::IntArgumentParser portParser("port", "4242");
    portParser.AddValidArguments({"-p", "--port"});
    parsers.push_back(&portParser);

    nCommon::IntArgumentParser limitParser("limit", "-1");
    limitParser.AddValidArguments({"-l", "--limit"});
    parsers.push_back(&limitParser);

    nCommon::BoundsArgumentParser boundsParser("bounds", "1,100");
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

                 parser->Activate();
                 if(i < argc-1)
                     parser->ParseValue(argv[i+1]);

                 argumentFound = true;

             }
         }

         if(!argumentFound)
             std::cout<<"Invalid argument: "<<argument<<std::endl;
     }

    //We start the server application and let it run its cycle

    auto bounds = boundsParser.Value();
    nApplication::ServerApplication app(portParser.Value(), limitParser.Value(), bounds.first, bounds.second);
    app.Start();

    return 0;
}