// Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.
//
// NVIDIA CORPORATION and its licensors retain all intellectual property
// and proprietary rights in and to this software, related documentation
// and any modifications thereto.  Any use, reproduction, disclosure or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA CORPORATION is strictly prohibited.
//

#define CARB_EXPORTS

#include <carb/PluginUtils.h>

#include <omni/ext/IExt.h>

#include <omni/kit/commands/Command.h>
#include <omni/kit/commands/ICommandBridge.h>

const struct carb::PluginImplDesc pluginImplDesc = { "omni.example.cpp.commands.plugin",
                                                     "An example C++ extension.", "NVIDIA",
                                                     carb::PluginHotReload::eEnabled, "dev" };

namespace omni
{
namespace example
{
namespace cpp
{
namespace commands
{

using namespace omni::kit::commands;

class ExampleCppCommand : public Command
{
public:
    static carb::ObjectPtr<ICommand> create(const char* extensionId,
                                            const char* commandName,
                                            const carb::dictionary::Item* kwargs)
    {
        // Note: It is important to construct the handler using ObjectPtr<T>::InitPolicy::eSteal,
        // otherwise we end up incresing the reference count by one too many during construction,
        // resulting in a carb::ObjectPtr<IAction> whose object instance will never be destroyed.
        return carb::stealObject<ICommand>(new ExampleCppCommand(extensionId, commandName, kwargs));
    }

    static void populateKeywordArgs(carb::dictionary::Item* defaultKwargs,
                                    carb::dictionary::Item* optionalKwargs,
                                    carb::dictionary::Item* requiredKwargs)
    {
        if (carb::dictionary::IDictionary* iDictionary = carb::getCachedInterface<carb::dictionary::IDictionary>())
        {
            iDictionary->makeAtPath(defaultKwargs, "x", 9);
            iDictionary->makeAtPath(defaultKwargs, "y", -1);
        }
    }

    ExampleCppCommand(const char* extensionId, const char* commandName, const carb::dictionary::Item* kwargs)
        : Command(extensionId, commandName)
    {
        if (carb::dictionary::IDictionary* iDictionary = carb::getCachedInterface<carb::dictionary::IDictionary>())
        {
            m_x = iDictionary->get<int32_t>(kwargs, "x");
            m_y = iDictionary->get<int32_t>(kwargs, "y");
        }
    }

    void doCommand() override
    {
        printf("Executing command '%s' with params 'x=%d' and 'y=%d'.\n", getName(), m_x, m_y);
    }

    void undoCommand() override
    {
        printf("Undoing command '%s' with params 'x=%d' and 'y=%d'.\n", getName(), m_x, m_y);
    }

private:
    int32_t m_x = 0;
    int32_t m_y = 0;
};

class ExampleCommandsExtension : public omni::ext::IExt
{
public:
    void onStartup(const char* extId) override
    {
        auto commandBridge = carb::getCachedInterface<ICommandBridge>();
        if (!commandBridge)
        {
            CARB_LOG_WARN("Could not get cached ICommandBridge interface.");
            return;
        }

        // Example of registering a command from C++.
        commandBridge->registerCommand(
            "omni.example.cpp.commands", "ExampleCppCommand", ExampleCppCommand::create, ExampleCppCommand::populateKeywordArgs);
    }

    void onShutdown() override
    {
        if (auto commandBridge = carb::getCachedInterface<ICommandBridge>())
        {
            commandBridge->deregisterCommand("omni.example.cpp.commands", "ExampleCppCommand");
        }
        else
        {
            CARB_LOG_WARN("Could not get cached ICommandBridge interface.");
        }
    }
};

}
}
}
}

CARB_PLUGIN_IMPL(pluginImplDesc, omni::example::cpp::commands::ExampleCommandsExtension)

void fillInterface(omni::example::cpp::commands::ExampleCommandsExtension& iface)
{
}
