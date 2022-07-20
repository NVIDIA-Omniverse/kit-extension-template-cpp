// Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.
//
// NVIDIA CORPORATION and its licensors retain all intellectual property
// and proprietary rights in and to this software, related documentation
// and any modifications thereto.  Any use, reproduction, disclosure or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA CORPORATION is strictly prohibited.
//
#pragma once

#include <carb/Interface.h>

namespace omni
{
namespace example
{
namespace cpp
{
namespace usd
{

/**
 * Interface used to interact with the example C++ USD plugin from Python.
 */
class IExampleUsdInterface
{
public:
    CARB_PLUGIN_INTERFACE("omni::example::cpp::usd::IExampleUsdInterface", 1, 0);

    /**
     * Creates some example prims using C++.
     */
    virtual void createPrims() = 0;

    /**
     * Print some info about the currently open USD stage from C++.
     */
    virtual void printStageInfo() const = 0;

    /**
     * Get whether the example prims are being animated.
     *
     * @return True if the example prims are being animated, false otherwise.
     */
    virtual bool getAnimatePrims() const = 0;

    /**
     * Set whether to animate the example prims.
     *
     * @param value True if the example prims should be animated, false otherwise.
     */
    virtual void setAnimatePrims(bool value) = 0;

    /**
     * Called when the default USD stage (ie. the one open in the main viewport) changes.
     * Necessary for now until the omni.usd C++ API becomes ready for public consumption.
     *
     * @param stageId The id of the new default USD stage.
     */
    virtual void onDefaultUsdStageChanged(long stageId) = 0;
};

}
}
}
}
