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

    std::string argument;

    //We start bind checking arguments received
    for(int i = 1;i < argc; i+=2) {

        argument = argv[i];

        bool argumentFound = false;

        for (auto *parser: parsers) {
            if (parser->IsValidArgument(argument)) {

                if (i < argc - 1)
                    parser->ParseValue(argv[i + 1]);

                argumentFound = true;

            }
        }

        if (!argumentFound)
            std::cout << "Invalid argument: " << argument << std::endl;
    }

    if(nameParser.Value().empty())
    {
        std::cout<<"Enter a valid name"<<std::endl;
        return 0;
    }

    nApplication::ClientApplication app(nameParser.Value(), hostParser.Value(), portParser.Value());

    app.Start();

    while(app.IsRunning())
    {
        app.Run();
    }

    return 0;
}