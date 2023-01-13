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
#include <carb/tasking/ITasking.h>
#include <carb/tasking/TaskingUtils.h>

#include <omni/ext/IExt.h>
#include <omni/kit/IApp.h>

const struct carb::PluginImplDesc pluginImplDesc = { "omni.example.cpp.tasking.plugin",
                                                     "An example C++ extension.", "NVIDIA",
                                                     carb::PluginHotReload::eEnabled, "dev" };

CARB_PLUGIN_IMPL_DEPS(omni::kit::IApp)

namespace omni
{
namespace example
{
namespace cpp
{
namespace tasking
{

class ExampleTaskingExtension;

void exampleStandaloneFunctionTask(ExampleTaskingExtension* exampleTaskingExtension);

class ExampleTaskingExtension : public omni::ext::IExt
{
public:
    void onStartup(const char* extId) override
    {
        // Get the tasking interface from the Carbonite Framework.
        carb::tasking::ITasking* tasking = carb::getCachedInterface<carb::tasking::ITasking>();

        // Add a task defined by a standalone function.
        tasking->addTask(carb::tasking::Priority::eDefault, {}, &exampleStandaloneFunctionTask, this);

        // Add a task defined by a member function.
        tasking->addTask(carb::tasking::Priority::eDefault, {}, &ExampleTaskingExtension::exampleMemberFunctionTask, this);

        // Add a task defined by a lambda function.
        tasking->addTask(carb::tasking::Priority::eDefault, {}, [this] {
            // Artifical wait to ensure this task finishes first.
            carb::getCachedInterface<carb::tasking::ITasking>()->sleep_for(std::chrono::milliseconds(1000));
            printHelloFromTask("exampleLambdaFunctionTask");
        });
    }

    void onShutdown() override
    {
        std::lock_guard<carb::tasking::MutexWrapper> lock(m_helloFromTaskCountMutex);
        m_helloFromTaskCount = 0;
    }

    void exampleMemberFunctionTask()
    {
        // Artifical wait to ensure this task finishes second.
        carb::getCachedInterface<carb::tasking::ITasking>()->sleep_for(std::chrono::milliseconds(2000));
        printHelloFromTask("exampleMemberFunctionTask");
    }

    void printHelloFromTask(const char* taskName)
    {
        std::lock_guard<carb::tasking::MutexWrapper> lock(m_helloFromTaskCountMutex);
        ++m_helloFromTaskCount;
        printf("Hello from task: %s\n"
               "%d tasks have said hello since extension startup.\n\n",
               taskName, m_helloFromTaskCount);
    }

private:
    // We must use a fiber aware mutex: https://docs.omniverse.nvidia.com/kit/docs/carbonite/latest/docs/tasking/TaskingBestPractices.html#mutexes
    carb::tasking::MutexWrapper m_helloFromTaskCountMutex;
    int m_helloFromTaskCount = 0;
};

void exampleStandaloneFunctionTask(ExampleTaskingExtension* exampleTaskingExtension)
{
    // Artifical wait to ensure this task finishes last.
    carb::getCachedInterface<carb::tasking::ITasking>()->sleep_for(std::chrono::milliseconds(3000));
    exampleTaskingExtension->printHelloFromTask("exampleStandaloneFunctionTask");
}

}
}
}
}

CARB_PLUGIN_IMPL(pluginImplDesc, omni::example::cpp::tasking::ExampleTaskingExtension)

void fillInterface(omni::example::cpp::tasking::ExampleTaskingExtension& iface)
{
}
