#include <ll/api/plugin/NativePlugin.h>

#include "Plugin.h"

#include <iostream>
#include <string>

#include <ll/api/memory/Hook.h>
#include <ll/api/memory/Memory.h>
#include <ll/api/service/Bedrock.h>
#include <mc/entity/utilities/ActorType.h>
#include <ll/api/command/CommandHandle.h>
#include <ll/api/command/CommandRegistrar.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandPermissionLevel.h>
#include <mc/world/level/Level.h>
#include <utility>
#include <sys/timeb.h>

#include "SystemProfilier.h"
#include "Utils.h"

int tick = 0;
long timeOld = 0;
long timeNew = 0;

long getTimestamp() {
	timeb t;
	ftime(&t);
	return (long)t.time * 1000 + t.millitm;
}

std::string getTps() {
	double input = 1000.0 * (20.0 / (timeNew - timeOld));
    if (input > 20.0) input = 20.0;
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << input;
	std::string output = stream.str();
	if (input >= 18.5) {
		return "[§a#§r] TPS: §a" + output;
	}
	else if (input > 15.0) {
		return "[§a#§r] TPS: §6" + output;
	}
	else return "[§a#§r] TPS: §c" + output;
}

namespace plugin {

Plugin::Plugin(ll::plugin::NativePlugin& self) : mSelf(self) {
    SystemProfilier::init();
    mSelf.getLogger().info("Loaded!");
}

bool Plugin::enable() {
    auto& logger = mSelf.getLogger();

    //auto commandRegistry = ll::service::getCommandRegistry();
    //if (!commandRegistry) {
    //    throw std::runtime_error("failed to get command registry");
    //}

    auto& tpsCommand = ll::command::CommandRegistrar::getInstance().getOrCreateCommand("tps", "Get server tps", CommandPermissionLevel::Any, CommandFlagValue::None);

    tpsCommand.overload().execute<[&](CommandOrigin const&, CommandOutput& output) {
        output.success(getTps());
    }>();

    auto& maidCommand = ll::command::CommandRegistrar::getInstance().getOrCreateCommand("maid", "§dServer maid", CommandPermissionLevel::GameDirectors, CommandFlagValue::None);

    maidCommand.overload().execute<[&](CommandOrigin const&, CommandOutput& output) {
        auto level = ll::service::getLevel();
        std::string message = "§r[§a#§r] Tritium health info!\n";
        message += getTps();
        message += "\n§r[§a#§r] Players: §b" + std::to_string(level->getActivePlayerCount());
        message += "\n§r[§a#§r] Entities: §b" + std::to_string(level->getEntities().size());
        message += "\n§r[§a#§r] CPU Usage: §b" + SystemProfilier::getCpuUsage() + "%%";
        message += "\n§r[§a#§r] Virtual memory: §b" + Utils::formatDataUnit(SystemProfilier::getVirtualMemory()) + "/" + Utils::formatDataUnit(SystemProfilier::getVirtualMemoryUsed());
        message += "\n§r[§a#§r] Physical memory: §b" + Utils::formatDataUnit(SystemProfilier::getPhysicalMemory()) + "/" + Utils::formatDataUnit(SystemProfilier::getPhysicalMemoryUsed());
        output.success(message);
    }>();

    return true;
}

bool Plugin::disable() {
    return true;
}

} // namespace plugin

// Hook Level tick func
LL_AUTO_STATIC_HOOK(
    LevelTickHook,
    ll::memory::HookPriority::Normal,
    "?tick@Level@@UEAAXXZ",
    void*,
    void* self
){
    try {
		tick++;
		if (tick == 20.0) {
			tick = 0;
			timeOld = timeNew;
			timeNew = getTimestamp();
		}
	} catch (const char*& err) {
		std::cout << err << std::endl;
	}
    return origin(self);
}