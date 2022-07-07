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

#include <omni/ext/ExtensionsUtils.h>
#include <omni/ext/IExt.h>
#include <omni/kit/IApp.h>

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usdGeom/metrics.h>

const struct carb::PluginImplDesc pluginImplDesc = { "omni.example.cpp.usd.plugin",
                                                     "An example C++ extension.", "NVIDIA",
                                                     carb::PluginHotReload::eEnabled, "dev" };

CARB_PLUGIN_IMPL_DEPS(omni::kit::IApp)

namespace omni
{
namespace example
{
namespace cpp
{
namespace usd
{

class ExampleCppUsdExtension : public omni::ext::IExt
{
public:
    void onStartup(const char* extId) override
    {
        // Open the example USD stage included with this extension.
        omni::kit::IApp* app = carb::getCachedInterface<omni::kit::IApp>();
        const std::string extPath = getExtensionPath(app->getExtensionManager(), extId);
        const std::string stagePath = extPath + "/data/example_usd_stage.usd";

        m_stage = pxr::UsdStage::Open(stagePath.c_str());
        if (!m_stage)
        {
            printf("Could not open USD stage: %s.\n", stagePath.c_str());
            return;
        }

        // Print the USD stage's up-axis.
        const pxr::TfToken stageUpAxis = pxr::UsdGeomGetStageUpAxis(m_stage);
        printf("Stage up-axis is: %s.\n", stageUpAxis.GetText());

        // Print the USD stage's meters per unit.
        const double metersPerUnit = pxr::UsdGeomGetStageMetersPerUnit(m_stage);
        printf("Stage meters per unit: %f.\n", metersPerUnit);

        // Print the USD stage's prims.
        const pxr::UsdPrimRange primRange = m_stage->Traverse();
        for (const pxr::UsdPrim& prim : primRange)
        {
            printf("Stage contains prim: %s.\n", prim.GetPath().GetString().c_str());
        }
    }

    void onShutdown() override
    {
        m_stage.Reset();
    }

private:
    pxr::UsdStageRefPtr m_stage;
};

}
}
}
}

CARB_PLUGIN_IMPL(pluginImplDesc, omni::example::cpp::usd::ExampleCppUsdExtension)

void fillInterface(omni::example::cpp::usd::ExampleCppUsdExtension& iface)
{
}