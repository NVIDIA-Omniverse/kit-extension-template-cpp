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

#include <omni/kit/actions/core/IActionRegistry.h>
#include <omni/kit/actions/core/LambdaAction.h>

const struct carb::PluginImplDesc pluginImplDesc = { "omni.example.cpp.actions.plugin",
                                                     "An example C++ extension.", "NVIDIA",
                                                     carb::PluginHotReload::eEnabled, "dev" };

namespace omni
{
namespace example
{
namespace cpp
{
namespace actions
{

using namespace omni::kit::actions::core;

class ExampleCustomAction : public Action
{
public:
    static carb::ObjectPtr<IAction> create(const char* extensionId,
                                           const char* actionId,
                                           const MetaData* metaData)
    {
        // Note: It is important to construct the handler using ObjectPtr<T>::InitPolicy::eSteal,
        // otherwise we end up incresing the reference count by one too many during construction,
        // resulting in a carb::ObjectPtr<IAction> whose object instance will never be destroyed.
        return carb::stealObject<IAction>(new ExampleCustomAction(extensionId, actionId, metaData));
    }

    ExampleCustomAction(const char* extensionId, const char* actionId, const MetaData* metaData)
        : Action(extensionId, actionId, metaData), m_executionCount(0)
    {
    }

    carb::variant::Variant execute(const carb::variant::Variant& args = {},
                                   const carb::dictionary::Item* kwargs = nullptr) override
    {
        ++m_executionCount;
        printf("Executing %s (execution count = %d).\n", getActionId(), m_executionCount);
        return carb::variant::Variant(m_executionCount);
    }

    void invalidate() override
    {
        resetExecutionCount();
    }

    uint32_t getExecutionCount() const
    {
        return m_executionCount;
    }

protected:
    void resetExecutionCount()
    {
        m_executionCount = 0;
    }

private:
    uint32_t m_executionCount = 0;
};

class ExampleActionsExtension : public omni::ext::IExt
{
public:
    void onStartup(const char* extId) override
    {
        auto actionRegistry = carb::getCachedInterface<IActionRegistry>();
        if (!actionRegistry)
        {
            CARB_LOG_WARN("Could not get cached IActionRegistry interface.");
            return;
        }

        // Example of registering a custom action from C++.
        Action::MetaData metaData;
        metaData.displayName = "Example Custom Action Display Name";
        metaData.description = "Example Custom Action Description.";
        m_exampleCustomAction = ExampleCustomAction::create("omni.example.cpp.actions", "example_custom_action_id", &metaData);
        actionRegistry->registerAction(m_exampleCustomAction);

        // Example of registering a lambda action from C++.
        m_exampleLambdaAction = actionRegistry->registerAction(
            "omni.example.cpp.actions", "example_lambda_action_id",
            [](const carb::variant::Variant& args = {}, const carb::dictionary::Item* kwargs = nullptr) {
                printf("Executing example_lambda_action_id.\n");
                return carb::variant::Variant();
            },
            "Example Lambda Action Display Name",
            "Example Lambda Action Description.");
    }

    void onShutdown() override
    {
        if (auto actionRegistry = carb::getCachedInterface<IActionRegistry>())
        {
            actionRegistry->deregisterAction(m_exampleLambdaAction);
            m_exampleLambdaAction = nullptr;

            actionRegistry->deregisterAction(m_exampleCustomAction);
            m_exampleCustomAction = nullptr;
        }
        else
        {
            CARB_LOG_WARN("Could not get cached IActionRegistry interface.");
        }
    }

private:
    carb::ObjectPtr<IAction> m_exampleCustomAction;
    carb::ObjectPtr<IAction> m_exampleLambdaAction;
};

}
}
}
}

CARB_PLUGIN_IMPL(pluginImplDesc, omni::example::cpp::actions::ExampleActionsExtension)

void fillInterface(omni::example::cpp::actions::ExampleActionsExtension& iface)
{
}
