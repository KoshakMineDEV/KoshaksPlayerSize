/**
 * @file Plugin.cpp
 * @brief The main file of the plugin
 */
#include <fstream>
#include <LoggerAPI.h>
#include <LLAPI.h>
#include <regCommandAPI.h>
#include <EventAPI.h>
#include <MC/SynchedActorData.hpp>
#include <third-party\Nlohmann\json.hpp>
#include <MC/SynchedActorDataComponent.hpp>

Logger logger;
using json = nlohmann::json;


class SizeCMD : public Command {
    float size;
public: 
    void execute(CommandOrigin const& ori, CommandOutput& outp) const {

        std::ifstream conf("plugins\\kps\\config.json");
        json parsed;
        conf >> parsed;
        float minSize = parsed["minSize"];
        float maxSize = parsed["maxSize"];
        if (size >= minSize && size <= maxSize) {
            ori.getPlayer()->getEntityData().set(ActorDataKeys::SCALE, size);
        }
        else {
            std::ostringstream sssize;
            sssize << size;
            std::string strsize(sssize.str());

            std::ostringstream ssminsize;
            ssminsize << minSize;
            std::string strminsize(ssminsize.str());

            std::ostringstream ssmaxsize;
            ssmaxsize << maxSize;
            std::string strmaxsize(ssmaxsize.str());
            ori.getPlayer()->sendText("You set the size higher or lower than allowed(" + strsize + "). Maximum size: " + strmaxsize + ", minimum size : " + strminsize);
        }
    }

    static void setup(CommandRegistry* registry)
    {
        using RegisterCommandHelper::makeMandatory;
        registry->registerCommand(
            "size",
            "Change your size",
            CommandPermissionLevel::Any,
            { (CommandFlagValue)0 },
            { (CommandFlagValue)0x80 });
        registry->registerOverload<SizeCMD>(
            "Size",
            makeMandatory(&SizeCMD::size, "size"));
    }
};

void CommandEvent() {
    Event::RegCmdEvent::subscribe([](Event::RegCmdEvent ev) {
        SizeCMD::setup(ev.mCommandRegistry);
        return true;
        });
}


inline bool configExists(const std::string& name) {
    if (FILE* file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    }
    else {
        return false;
    }
}

void PluginInit()
{

    CommandEvent();
    if (!configExists("plugins\\kps\\config.json")) {
        logger.warn("[KPS] Config file not found, creating one...");
            std::ofstream file;
            std::filesystem::create_directory("plugins\\kps");
            file.open("plugins\\kps\\config.json");
            file << "{\n\"minSize\": 0.5,\n\"maxSize\": 2\n}";
            logger.warn("[KPS] Created!");
            file.close();
      
    }
}
