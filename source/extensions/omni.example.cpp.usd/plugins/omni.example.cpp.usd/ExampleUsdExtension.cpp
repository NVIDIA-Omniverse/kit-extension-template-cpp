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
#include <omni/timeline/ITimeline.h>
#include <omni/timeline/TimelineTypes.h>

#include <pxr/usd/usd/notice.h>
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
                             , public PXR_NS::TfWeakBase
{
protected:
    void createPrims() override
    {
        // It is important that all USD stage reads/writes happen from the main thread:
        // https ://graphics.pixar.com/usd/release/api/_usd__page__multi_threading.html
        if (!m_stage)
        {
            return;
        }

        constexpr int numPrimsToCreate = 9;
        const float rotationIncrement = 360.0f / (numPrimsToCreate - 1);
        for (int i = 0; i < numPrimsToCreate; ++i)
        {
            // Create a cube prim.
            const PXR_NS::SdfPath primPath("/World/example_prim_" + std::to_string(i));
            if (m_stage->GetPrimAtPath(primPath))
            {
                // A prim already exists at this path.
                continue;
            }
            PXR_NS::UsdPrim prim = m_stage->DefinePrim(primPath, PXR_NS::TfToken("Cube"));

            // Set the size of the cube prim.
            const double cubeSize = 0.5 / PXR_NS::UsdGeomGetStageMetersPerUnit(m_stage);
            prim.CreateAttribute(PXR_NS::TfToken("size"), PXR_NS::SdfValueTypeNames->Double).Set(cubeSize);

            // Leave the first prim at the origin and position the rest in a circle surrounding it.
            if (i == 0)
            {
                m_primsWithRotationOps.push_back({ prim });
            }
            else
            {
                PXR_NS::UsdGeomXformable xformable = PXR_NS::UsdGeomXformable(prim);

                // Setup the global rotation operation.
                const float initialRotation = rotationIncrement * static_cast<float>(i);
                PXR_NS::UsdGeomXformOp globalRotationOp = xformable.AddRotateYOp(PXR_NS::UsdGeomXformOp::PrecisionFloat);
                globalRotationOp.Set(initialRotation);

                // Setup the translation operation.
                const PXR_NS::GfVec3f translation(0.0f, 0.0f, cubeSize * 4.0f);
                xformable.AddTranslateOp(PXR_NS::UsdGeomXformOp::PrecisionFloat).Set(translation);

                // Setup the local rotation operation.
                PXR_NS::UsdGeomXformOp localRotationOp = xformable.AddRotateXOp(PXR_NS::UsdGeomXformOp::PrecisionFloat);
                localRotationOp.Set(initialRotation);

                // Store the prim and rotation ops so we can update them later in animatePrims().
                m_primsWithRotationOps.push_back({ prim, localRotationOp, globalRotationOp });
            }
        }

        // Subscribe to timeline events so we know when to start or stop animating the prims.
        if (auto timeline = omni::timeline::getTimeline())
        {
            m_timelineEventsSubscription = carb::events::createSubscriptionToPop(
                timeline->getTimelineEventStream(),
                [this](carb::events::IEvent* timelineEvent) {
                onTimelineEvent(static_cast<omni::timeline::TimelineEventType>(timelineEvent->type));
            });
        }
    }

    void removePrims() override
    {
        if (!m_stage)
        {
            return;
        }

        // Release all event subscriptions.
        PXR_NS::TfNotice::Revoke(m_usdNoticeListenerKey);
        m_timelineEventsSubscription = nullptr;
        m_updateEventsSubscription = nullptr;

        // Remove all prims.
        for (auto& primWithRotationOps : m_primsWithRotationOps)
        {
            m_stage->RemovePrim(primWithRotationOps.m_prim.GetPath());
        }
        m_primsWithRotationOps.clear();
    }

    void printStageInfo() const override
    {
        if (!m_stage)
        {
            return;
        }

        printf("---Stage Info Begin---\n");

        // Print the USD stage's up-axis.
        const PXR_NS::TfToken stageUpAxis = PXR_NS::UsdGeomGetStageUpAxis(m_stage);
        printf("Stage up-axis is: %s.\n", stageUpAxis.GetText());

        // Print the USD stage's meters per unit.
        const double metersPerUnit = PXR_NS::UsdGeomGetStageMetersPerUnit(m_stage);
        printf("Stage meters per unit: %f.\n", metersPerUnit);

        // Print the USD stage's prims.
        const PXR_NS::UsdPrimRange primRange = m_stage->Traverse();
        for (const PXR_NS::UsdPrim& prim : primRange)
        {
            printf("Stage contains prim: %s.\n", prim.GetPath().GetString().c_str());
        }

        printf("---Stage Info End---\n\n");
    }

    void startTimelineAnimation() override
    {
        if (auto timeline = omni::timeline::getTimeline())
        {
            timeline->play();
        }
    }

    void stopTimelineAnimation() override
    {
        if (auto timeline = omni::timeline::getTimeline())
        {
            timeline->stop();
        }
    }

    void onDefaultUsdStageChanged(long stageId) override
    {
        PXR_NS::TfNotice::Revoke(m_usdNoticeListenerKey);
        m_timelineEventsSubscription = nullptr;
        m_updateEventsSubscription = nullptr;
        m_primsWithRotationOps.clear();
        m_stage.Reset();

        if (stageId)
        {
            m_stage = PXR_NS::UsdUtilsStageCache::Get().Find(PXR_NS::UsdStageCache::Id::FromLongInt(stageId));
            m_usdNoticeListenerKey = PXR_NS::TfNotice::Register(PXR_NS::TfCreateWeakPtr(this), &ExampleCppUsdExtension::onObjectsChanged);
        }
    }

    void onObjectsChanged(const PXR_NS::UsdNotice::ObjectsChanged& objectsChanged)
    {
        // Check whether any of the prims we created have been (potentially) invalidated.
        // This may be too broad a check, but handles prims being removed from the stage.
        for (auto& primWithRotationOps : m_primsWithRotationOps)
        {
            if (!primWithRotationOps.m_invalid &&
                objectsChanged.ResyncedObject(primWithRotationOps.m_prim))
            {
                primWithRotationOps.m_invalid = true;
            }
        }
    }

    void onTimelineEvent(omni::timeline::TimelineEventType timelineEventType)
    {
        switch (timelineEventType)
        {
        case omni::timeline::TimelineEventType::ePlay:
        {
            startAnimatingPrims();
        }
        break;
        case omni::timeline::TimelineEventType::eStop:
        {
            stopAnimatingPrims();
        }
        break;
        default:
        {

        }
        break;
        }
    }

    void startAnimatingPrims()
    {
        if (m_updateEventsSubscription)
        {
            // We're already animating the prims.
            return;
        }

        // Subscribe to update events so we can animate the prims.
        if (omni::kit::IApp* app = carb::getCachedInterface<omni::kit::IApp>())
        {
            m_updateEventsSubscription = carb::events::createSubscriptionToPop(app->getUpdateEventStream(), [this](carb::events::IEvent*)
            {
                onUpdateEvent();
            });
        }
    }

    void stopAnimatingPrims()
    {
        m_updateEventsSubscription = nullptr;
        onUpdateEvent(); // Reset positions.
    }

    void onUpdateEvent()
    {
        // It is important that all USD stage reads/writes happen from the main thread:
        // https ://graphics.pixar.com/usd/release/api/_usd__page__multi_threading.html
        if (!m_stage)
        {
            return;
        }

        // Update the value of each local and global rotation operation to (crudely) animate the prims around the origin.
        const size_t numPrims = m_primsWithRotationOps.size();
        const float initialLocalRotationIncrement = 360.0f / (numPrims - 1); // Ignore the first prim at the origin.
        const float initialGlobalRotationIncrement = 360.0f / (numPrims - 1); // Ignore the first prim at the origin.
        const float currentAnimTime = omni::timeline::getTimeline()->getCurrentTime() * m_stage->GetTimeCodesPerSecond();
        for (size_t i = 1; i < numPrims; ++i) // Ignore the first prim at the origin.
        {
            if (m_primsWithRotationOps[i].m_invalid)
            {
                continue;
            }

            PXR_NS::UsdGeomXformOp& localRotationOp = m_primsWithRotationOps[i].m_localRotationOp;
            const float initialLocalRotation = initialLocalRotationIncrement * static_cast<float>(i);
            const float currentLocalRotation = initialLocalRotation + (360.0f * (currentAnimTime / 100.0f));
            localRotationOp.Set(currentLocalRotation);

            PXR_NS::UsdGeomXformOp& globalRotationOp = m_primsWithRotationOps[i].m_globalRotationOp;
            const float initialGlobalRotation = initialGlobalRotationIncrement * static_cast<float>(i);
            const float currentGlobalRotation = initialGlobalRotation - (360.0f * (currentAnimTime / 100.0f));
            globalRotationOp.Set(currentGlobalRotation);
        }
    }

private:
    struct PrimWithRotationOps
    {
        PXR_NS::UsdPrim m_prim;
        PXR_NS::UsdGeomXformOp m_localRotationOp;
        PXR_NS::UsdGeomXformOp m_globalRotationOp;
        bool m_invalid = false;
    };

    PXR_NS::UsdStageRefPtr m_stage;
    PXR_NS::TfNotice::Key m_usdNoticeListenerKey;
    std::vector<PrimWithRotationOps> m_primsWithRotationOps;
    carb::events::ISubscriptionPtr m_updateEventsSubscription;
    carb::events::ISubscriptionPtr m_timelineEventsSubscription;
};

}
}
}
}

CARB_PLUGIN_IMPL(pluginImplDesc, omni::example::cpp::usd::ExampleCppUsdExtension)

void fillInterface(omni::example::cpp::usd::ExampleCppUsdExtension& iface)
{
}
