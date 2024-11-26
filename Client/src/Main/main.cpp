#include "ArgumentParser/ArgumentManager.hpp"
#include "ArgumentParser/BoolArgumentParser.hpp"
#include "ArgumentParser/IntArgumentParser.hpp"
#include "ArgumentParser/StringArgumentParser.hpp"

#include "ClientApplication/ClientApplication.hpp"

int main(int argc, char** argv)
{
    nCommon::StringArgumentParser hostParser("host", "localhost");
    hostParser.AddValidArguments({"-h", "--host"});

    nCommon::IntArgumentParser portParser("port", "4242");
    portParser.AddValidArguments({"-p", "--port"});

    nCommon::StringArgumentParser nameParser("name", "");
    nameParser.AddValidArguments({"-n", "--name"});

    nCommon::BoolArgumentParser autoParser("auto", "");
    autoParser.AddValidArguments({"-a", "--auto"});

    //We start bind checking arguments received
    nCommon::ArgumentManager::ParseArguments(argc, argv, {&hostParser, &portParser, &nameParser, &autoParser});

    //Empty name found, just return
    if(nameParser.Value().empty())
    {
        std::cout<<"Enter a valid name"<<std::endl;
        return 0;
    }

    //Starting application
    nApplication::ClientApplication app(nameParser.Value(), hostParser.Value(), portParser.Value(), autoParser.Value());

    app.Start();

    while(app.IsRunning())
    {
        app.Run();
    }

    return 0;
}