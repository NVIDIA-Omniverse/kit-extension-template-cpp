// SPDX-FileCopyrightText: Copyright (c) 2024, NVIDIA CORPORATION
// SPDX-License-Identifier: Apache-2.0

#define CARB_EXPORTS

#include <carb/PluginUtils.h>

#include <omni/ext/IExt.h>
#include <omni/kit/IApp.h>
#include <carb/eventdispatcher/IEventDispatcher.h>

// The CUDA kernel launcher defined in HelloWorldKernel.cu
extern "C" bool launchCudaHelloKernel(void);

//------------------------------------------------------------------------------------------
// Standard plugin definition required by Carbonite.
//------------------------------------------------------------------------------------------
static const struct carb::PluginImplDesc pluginImplDesc = {
    "omni.example.cpp.hello_world_cuda.plugin",                    // Unique name of the plugin
    "A CUDA Hello World C++ extension.",                           // Description
    "NVIDIA",                                                      // Author
    carb::PluginHotReload::eEnabled,                                // Hot-reload support
    "dev"                                                          // Version tag
};

// This extension depends on the EventDispatcher interface (used for update callbacks).
CARB_PLUGIN_IMPL_DEPS(carb::eventdispatcher::IEventDispatcher)

namespace omni
{
namespace example
{
namespace cpp
{
namespace hello_world_cuda
{

//------------------------------------------------------------------------------------------
// ExampleCppHelloWorldCudaExtension
//------------------------------------------------------------------------------------------
class ExampleCppHelloWorldCudaExtension : public omni::ext::IExt
{
public:
    // Called once when the extension is enabled.
    void onStartup(const char* extId) override
    {
        printf("ExampleCppHelloWorldCudaExtension starting up (ext_id: %s).\n", extId);

        // Launch the CUDA kernel.
        if (launchCudaHelloKernel())
        {
            printf("CUDA kernel executed successfully.\n");
        }
        else
        {
            printf("CUDA kernel execution failed!\n");
        }

        // Observe update events so we can periodically print.
        if (carb::eventdispatcher::IEventDispatcher* dispatcher =
                carb::getCachedInterface<carb::eventdispatcher::IEventDispatcher>())
        {
            _updateEventsSubscription = dispatcher->observeEvent(
                carb::RStringKey("Hello World CUDA Extension"),
                carb::eventdispatcher::kDefaultOrder,
                omni::kit::kGlobalEventPostUpdate,
                [this](const carb::eventdispatcher::Event&) { onUpdate(); });
        }
    }

    // Called once when the extension is disabled.
    void onShutdown(void) override
    {
        printf("ExampleCppHelloWorldCudaExtension shutting down.\n");
        _updateEventsSubscription.reset();
    }

private:
    // Called every update tick (post-update phase).
    void onUpdate(void)
    {
        if (_updateCounter % 1000 == 0)
        {
            printf("Hello from the omni.example.cpp.hello_world_cuda extension! %d updates counted.\n", _updateCounter);
        }
        ++_updateCounter;
    }

    carb::eventdispatcher::ObserverGuard _updateEventsSubscription;
    int _updateCounter = 0;
};

} // namespace hello_world_cuda
} // namespace cpp
} // namespace example
} // namespace omni

//------------------------------------------------------------------------------------------
// Plugin entry points.
//------------------------------------------------------------------------------------------
CARB_PLUGIN_IMPL(pluginImplDesc, omni::example::cpp::hello_world_cuda::ExampleCppHelloWorldCudaExtension)

void fillInterface(omni::example::cpp::hello_world_cuda::ExampleCppHelloWorldCudaExtension& iface)
{
}