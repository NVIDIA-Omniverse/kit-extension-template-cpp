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

#include <vector>

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

        // Add a task defined by a standalone function, set to run in 3 seconds
        m_tasks.push_back(tasking->addTaskIn(std::chrono::seconds(3), carb::tasking::Priority::eDefault, {}, &exampleStandaloneFunctionTask, this));

        // Add a task defined by a member function, set to run in 2 seconds
        m_tasks.push_back(tasking->addTaskIn(std::chrono::seconds(2), carb::tasking::Priority::eDefault, {}, &ExampleTaskingExtension::exampleMemberFunctionTask, this));

        // Add a task defined by a lambda function, set to run in 1 second.
        m_tasks.push_back(tasking->addTaskIn(std::chrono::seconds(1), carb::tasking::Priority::eDefault, {}, [this] {
            printHelloFromTask("exampleLambdaFunctionTask");
        }));
    }

    void onShutdown() override
    {
        // Get the tasking interface from the Carbonite Framework.
        carb::tasking::ITasking* tasking = carb::getCachedInterface<carb::tasking::ITasking>();

        // Try to cancel tasks or wait for them to complete.
        for (auto& task : m_tasks)
        {
            if (!tasking->tryCancelTask(*task.task_if()))
                task.wait();
        }

        // Clear/Reset state
        m_tasks.clear();
        m_helloFromTaskCount = 0;
    }

    void exampleMemberFunctionTask()
    {
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
    std::vector<carb::tasking::Future<>> m_tasks;

    // We must use a fiber aware mutex: https://docs.omniverse.nvidia.com/kit/docs/carbonite/latest/docs/tasking/TaskingBestPractices.html#mutexes
    carb::tasking::MutexWrapper m_helloFromTaskCountMutex;
    int m_helloFromTaskCount = 0;
};

void exampleStandaloneFunctionTask(ExampleTaskingExtension* exampleTaskingExtension)
{
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
