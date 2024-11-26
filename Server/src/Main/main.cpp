#include "ArgumentParser/ArgumentManager.hpp"
#include "ArgumentParser/ArgumentParser.hpp"
#include "ArgumentParser/BoundsArgumentParser.hpp"
#include "ArgumentParser/IntArgumentParser.hpp"
#include "ServerApplication/ServerApplication.hpp"

#include <iostream>
#include <vector>

int main(int argc, char **argv)
{
    nCommon::IntArgumentParser portParser("port", "4242");
    portParser.AddValidArguments({"-p", "--port"});

    nCommon::IntArgumentParser limitParser("limit", "-1");
    limitParser.AddValidArguments({"-l", "--limit"});

    nCommon::BoundsArgumentParser boundsParser("bounds", "1,100");
    boundsParser.AddValidArguments({"-b", "--bounds"});

    //We start bind checking arguments received

    nCommon::ArgumentManager::ParseArguments(argc, argv, {&portParser, &limitParser, &boundsParser});

    //We start the server application and let it run its cycle

    auto bounds = boundsParser.Value();
    nApplication::ServerApplication app(portParser.Value(), limitParser.Value(), bounds.first, bounds.second);
    app.Start();

    return 0;
}
