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

#include <vector>

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

        // Print some info about the stage before and after creating some example prims.
        printStageInfo();
        createPrims();
        printStageInfo();

        // Subscribe to update events so we can animate the prims.
        omni::kit::IApp* app = carb::getCachedInterface<omni::kit::IApp>();
        m_updateEvents = carb::events::createSubscriptionToPop(app->getUpdateEventStream(), [this](carb::events::IEvent*)
        {
            animatePrims();
        });
    }

    void onShutdown() override
    {
        m_updateEvents = nullptr;
        m_globalRotationOps.clear();
        m_localRotationOps.clear();
        m_stage.Reset();
    }

protected:
    void printStageInfo() const
    {
        if (!m_stage)
        {
            return;
        }

        printf("---Stage Info Begin---\n");

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

        printf("---Stage Info End---\n\n");
    }

    void createPrims()
    {
        if (!m_stage)
        {
            return;
        }

        constexpr int numPrimsToCreate = 9;
        for (int i = 0; i < numPrimsToCreate; ++i)
        {
            // Create a cube prim.
            const std::string primPath = "/World/example_prim_" + std::to_string(i);
            pxr::UsdPrim prim = m_stage->DefinePrim(pxr::SdfPath(primPath), pxr::TfToken("Cube"));

            // Set the size of the cube prim.
            const double cubeSize = 0.5 / pxr::UsdGeomGetStageMetersPerUnit(m_stage);
            prim.CreateAttribute(pxr::TfToken("size"), pxr::SdfValueTypeNames->Double).Set(cubeSize);

            // Leave the first prim at the origin and position the rest in a circle surrounding it.
            if (i > 0)
            {
                pxr::UsdGeomXformable xformable = pxr::UsdGeomXformable(prim);

                // Setup the global rotation operation.
                const float rotationIncrement = 360.0f / (numPrimsToCreate - 1);
                const float initialRotation = rotationIncrement * static_cast<float>(i);
                pxr::UsdGeomXformOp globalRotationOp = xformable.AddRotateYOp(pxr::UsdGeomXformOp::PrecisionFloat);
                m_globalRotationOps.push_back(globalRotationOp); // Store it so we can update it later in animatePrims().
                globalRotationOp.Set(initialRotation);

                // Setup the translation operation.
                const pxr::GfVec3f translation(0.0f, 0.0f, cubeSize * 4.0f);
                xformable.AddTranslateOp(pxr::UsdGeomXformOp::PrecisionFloat).Set(translation);

                // Setup the local rotation operation.
                pxr::UsdGeomXformOp localRotationOp = xformable.AddRotateXOp(pxr::UsdGeomXformOp::PrecisionFloat);
                m_localRotationOps.push_back(localRotationOp); // Store it so we can update it later in animatePrims().
                localRotationOp.Set(initialRotation);
            }
        }
    }

    void animatePrims()
    {
        for (pxr::UsdGeomXformOp& globalRotationOp : m_globalRotationOps)
        {
            // Update the value of each global rotation operation to (crudely) animate the prims around the origin.
            float currentValue = 0.0f;
            globalRotationOp.Get(&currentValue);
            currentValue -= 1.0f;
            if (currentValue < 0.0f)
            {
                // Prevent the rotation value from getting too big and losing precision or overflowing.
                currentValue += 360.0f;
            }
            globalRotationOp.Set(currentValue);
        }

        for (pxr::UsdGeomXformOp& localRotationOp : m_localRotationOps)
        {
            // Update the value of each local rotation operation to (crudely) animate the prims around their axis.
            float currentValue = 0.0f;
            localRotationOp.Get(&currentValue);
            currentValue += 1.0f;
            if (currentValue > 360.0f)
            {
                // Prevent the rotation value from getting too big and losing precision or overflowing.
                currentValue -= 360.0f;
            }
            localRotationOp.Set(currentValue);
        }
    }

private:
    pxr::UsdStageRefPtr m_stage;
    std::vector<pxr::UsdGeomXformOp> m_localRotationOps;
    std::vector<pxr::UsdGeomXformOp> m_globalRotationOps;
    carb::ObjectPtr<carb::events::ISubscription> m_updateEvents;
};

}
}
}
}

CARB_PLUGIN_IMPL(pluginImplDesc, omni::example::cpp::usd::ExampleCppUsdExtension)

void fillInterface(omni::example::cpp::usd::ExampleCppUsdExtension& iface)
{
}
