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
#include <carb/scripting/IScripting.h>

#include <omni/ext/ExtensionsUtils.h>
#include <omni/ext/IExt.h>
#include <omni/kit/IApp.h>

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/stageCache.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usdGeom/metrics.h>
#include <pxr/usd/usdGeom/xform.h>
#include <pxr/usd/usdUtils/stageCache.h>

const struct carb::PluginImplDesc pluginImplDesc = { "omni.example.cpp.usd.plugin",
                                                     "An example C++ extension.", "NVIDIA",
                                                     carb::PluginHotReload::eEnabled, "dev" };

CARB_PLUGIN_IMPL_DEPS(omni::kit::IApp, carb::scripting::IScripting)

namespace omni
{
namespace example
{
namespace cpp
{
namespace usd
{

int getDefaultUsdStageId()
{
    // Get the id of the 'default' USD stage (ie. the one open in the main viewport).
    // Calling into Python to get this is not ideal, but necessary for the time being
    // because the equivalent omni.usd C++ API isn't yet ready for public consumption.
    carb::scripting::IScripting* scripting =
        carb::getFramework()->acquireInterface<carb::scripting::IScripting>("carb.scripting-python.plugin");
    carb::scripting::Context* scriptingContext = scripting->createContext();

    const char* importCommand = R"(from omni.usd import get_context)";
    scripting->executeString(scriptingContext, importCommand, 0, nullptr);

    carb::scripting::Object* usdContext = scripting->createObject();
    scripting->executeFunction(scriptingContext, "get_context", usdContext, 0);

    carb::scripting::Object* stageId = scripting->createObject();
    scripting->executeMethod(scriptingContext, usdContext, "get_stage_id", stageId, 0);

    const int returnValue = stageId ? scripting->getObjectAsInt(stageId) : 0;
    scripting->destroyObject(stageId);
    scripting->destroyObject(usdContext);
    scripting->destroyContext(scriptingContext);
    return returnValue;
}

pxr::UsdStageRefPtr findDefaultUsdStage()
{
    // Find the 'default' USD stage (ie. the one open in the main viewport).
    const int defaultUsdStageId = getDefaultUsdStageId();
    return pxr::UsdUtilsStageCache::Get().Find(pxr::UsdStageCache::Id::FromLongInt(defaultUsdStageId));
}

pxr::UsdStageRefPtr openExampleUSDStage(const char* extId)
{
    // Open the example USD stage included with this extension.
    omni::kit::IApp* app = carb::getCachedInterface<omni::kit::IApp>();
    const std::string extPath = getExtensionPath(app->getExtensionManager(), extId);
    const std::string stagePath = extPath + "/data/example_usd_stage.usd";
    return pxr::UsdStage::Open(stagePath.c_str());
}

class ExampleCppUsdExtension : public omni::ext::IExt
{
public:
    void onStartup(const char* extId) override
    {
        // First try to find the 'default' USD stage (ie. the one open in the main viewport).
        m_stage = findDefaultUsdStage();
        if (!m_stage)
        {
            // Otherwise try to open the example USD stage included with this extension.
            printf("Could not find the default USD stage, opening the example one.\n");
            m_stage = openExampleUSDStage(extId);
            if (!m_stage)
            {
                printf("Could not open the example USD stage.\n");
                return;
            }
        }

        printStageInfo();
        createExamplePrims();
        printStageInfo();
    }

    void onShutdown() override
    {
        m_stage.Reset();
    }

protected:
    void printStageInfo() const
    {
        if (!m_stage)
        {
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

    void createExamplePrims()
    {
        if (!m_stage)
        {
            return;
        }

        constexpr int numPrimsToCreate = 9;
        for (int i = 0; i < numPrimsToCreate; ++i)
        {
            // Create a sphere prim.
            const std::string primPath = "/example_prim_" + std::to_string(i);
            pxr::UsdPrim prim = m_stage->DefinePrim(pxr::SdfPath(primPath), pxr::TfToken("Sphere"));

            // Set the radius of the sphere prim.
            constexpr double sphereRadius = 20.0;
            prim.CreateAttribute(pxr::TfToken("radius"), pxr::SdfValueTypeNames->Double).Set(sphereRadius);

            // Leave the first prim at the origin...
            if (i > 0)
            {
                // ...then position all other prims in a circle surrounding the first one.
                const double translateX = 0.0;
                const double translateY = 0.0;
                const double translateZ = 2.0 / pxr::UsdGeomGetStageMetersPerUnit(m_stage);
                const pxr::GfVec3d translation(translateX, translateY, translateZ);

                constexpr double rotationIncrement = 360.0 / (numPrimsToCreate - 1);
                const double rotationY = static_cast<double>(i) * rotationIncrement;

                // ToDo: Fix to rotate around the origin instead of locally.
                pxr::UsdGeomXformable xformable = pxr::UsdGeomXformable(prim);
                xformable.AddRotateYOp().Set(rotationY);
                xformable.AddTranslateOp().Set(translation);
            }
        }

        // ToDo: Move them in update, and change their colour?
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
