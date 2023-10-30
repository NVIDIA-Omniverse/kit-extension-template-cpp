// Copyright (c) 2023, NVIDIA CORPORATION. All rights reserved.
//
// NVIDIA CORPORATION and its licensors retain all intellectual property
// and proprietary rights in and to this software, related documentation
// and any modifications thereto.  Any use, reproduction, disclosure or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA CORPORATION is strictly prohibited.
//
#pragma once

#include <carb/Interface.h>

#include <usdrt/scenegraph/usd/usd/attribute.h>
#include <usdrt/scenegraph/usd/sdf/path.h>
#include <usdrt/scenegraph/base/gf/quatf.h>

namespace omni
{
namespace example
{
namespace cpp
{
namespace usdruntime
{

/**
 * Interface used to interact with the example C++ USDRT plugin from Python. 
 */
class IExampleUsdrtInterface
{
public:
    /// @private
    CARB_PLUGIN_INTERFACE("omni::example::cpp::usdruntime::IExampleUsdrtInterface", 1, 0);

    /*
    Get the Fabric data for a path.
    */
    virtual std::string get_attributes_for_prim(long int stageId, usdrt::SdfPath* path, std::vector<usdrt::UsdAttribute>* data) = 0;
    
    /*
    Apply a random world space rotation to a prim in Fabric.
    */
    virtual std::string apply_random_rotation(long int stageId, usdrt::SdfPath* path, usdrt::GfQuatf* rot) = 0;
    
    /*
    Deform a Mesh prim
    */
    virtual std::string deform_mesh(long int stageId, usdrt::SdfPath* path, int time) = 0;
};

}
}
}
}
