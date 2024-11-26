#include "ArgumentParser/ArgumentManager.hpp"
#include "ArgumentParser/BoolArgumentParser.hpp"
#include "ArgumentParser/IntArgumentParser.hpp"
#include "ArgumentParser/StringArgumentParser.hpp"

#include "ClientApplication/ClientApplication.hpp"

int main(int argc, char** argv)
{
    std::vector<nCommon::ArgumentParser*> parsers;

    nCommon::StringArgumentParser hostParser("host", "localhost");
    hostParser.AddValidArguments({"-h", "--host"});
    parsers.push_back(&hostParser);

    nCommon::IntArgumentParser portParser("port", "4242");
    portParser.AddValidArguments({"-p", "--port"});
    parsers.push_back(&portParser);

    nCommon::StringArgumentParser nameParser("name", "");
    nameParser.AddValidArguments({"-n", "--name"});
    parsers.push_back(&nameParser);

    nCommon::BoolArgumentParser autoParser("auto", "");
    autoParser.AddValidArguments({"-a", "--auto"});
    parsers.push_back(&autoParser);

    std::string argument;

    //We start bind checking arguments received
    nCommon::ArgumentManager::ParseArguments(argc, argv, {&hostParser, &portParser, &nameParser, &autoParser});

    //Empty name found, just return
    if(nameParser.Value().empty())
    {
        std::cout<<"Enter a valid name"<<std::endl;
        return 0;
    }

    //Starting application
    nApplication::ClientApplication app(nameParser.Value(), hostParser.Value(), portParser.Value());

    app.Start();

    while(app.IsRunning())
    {
        app.Run();
    }

    return 0;
}