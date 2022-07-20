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

#include <omni/example/cpp/usd/IExampleUsdInterface.h>
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

namespace omni
{
namespace example
{
namespace cpp
{
namespace usd
{

class ExampleCppUsdExtension : public IExampleUsdInterface
{
protected:
    void printStageInfo() const override
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

    void createPrims() override
    {
        if (!m_stage)
        {
            return;
        }

        constexpr int numPrimsToCreate = 9;
        for (int i = 0; i < numPrimsToCreate; ++i)
        {
            // Create a cube prim.
            const pxr::SdfPath primPath("/World/example_prim_" + std::to_string(i));
            if (m_stage->GetPrimAtPath(primPath))
            {
                // A prim already exists at this path.
                continue;
            }
            pxr::UsdPrim prim = m_stage->DefinePrim(primPath, pxr::TfToken("Cube"));

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

    bool getAnimatePrims() const override
    {
        return m_updateEvents.get();
    }

    void setAnimatePrims(bool value) override
    {
        if (value && !m_updateEvents)
        {
            // Subscribe to update events so we can animate the prims.
            omni::kit::IApp* app = carb::getCachedInterface<omni::kit::IApp>();
            m_updateEvents = carb::events::createSubscriptionToPop(app->getUpdateEventStream(), [this](carb::events::IEvent*)
            {
                animatePrims();
            });
        }
        else if (!value && m_updateEvents)
        {
            m_updateEvents = nullptr;
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

    void onDefaultUsdStageChanged(long stageId)
    {
        m_updateEvents = nullptr;
        m_globalRotationOps.clear();
        m_localRotationOps.clear();
        m_stage.Reset();

        if (stageId)
        {
            m_stage = pxr::UsdUtilsStageCache::Get().Find(pxr::UsdStageCache::Id::FromLongInt(stageId));
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
