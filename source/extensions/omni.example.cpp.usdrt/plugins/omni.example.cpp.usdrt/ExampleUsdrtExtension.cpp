// Copyright (c) 2023, NVIDIA CORPORATION. All rights reserved.
//
// NVIDIA CORPORATION and its licensors retain all intellectual property
// and proprietary rights in and to this software, related documentation
// and any modifications thereto.  Any use, reproduction, disclosure or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA CORPORATION is strictly prohibited.
//

#define CARB_EXPORTS

#include <carb/PluginUtils.h>

#include <omni/example/cpp/usdrt/IExampleUsdrtInterface.h>
#include <omni/ext/ExtensionsUtils.h>
#include <omni/ext/IExt.h>
#include <omni/kit/IApp.h>

#include <usdrt/scenegraph/usd/usd/stage.h>
#include <usdrt/scenegraph/usd/usd/prim.h>
#include <usdrt/scenegraph/usd/rt/xformable.h>
#include <usdrt/scenegraph/usd/sdf/path.h>
#include <usdrt/scenegraph/base/gf/vec3f.h>
#include <usdrt/scenegraph/base/gf/quatf.h>
#include <usdrt/scenegraph/base/vt/array.h>
#include <usdrt/scenegraph/usd/usd/attribute.h>

#include <omni/fabric/IFabric.h>

#include <vector>
#include <random>

const struct carb::PluginImplDesc pluginImplDesc = { "omni.example.cpp.usdrt.plugin",
                                                     "An example C++ USDRT extension.", "NVIDIA",
                                                     carb::PluginHotReload::eEnabled, "dev" };

namespace omni
{
namespace example
{
namespace cpp
{
namespace usdruntime
{

class ExampleCppUsdrtExtension : public IExampleUsdrtInterface
{
protected:
    /* 
    Get the Fabric data for a path.
    */
    std::string get_attributes_for_prim(long int stageId, usdrt::SdfPath* path, std::vector<usdrt::UsdAttribute>* data) override;

    /*
    Apply a random world space rotation to a prim in Fabric.
    */
    std::string apply_random_rotation(long int stageId, usdrt::SdfPath* path, usdrt::GfQuatf* rot) override;

    /*
    Deform a Mesh prim
    */
    std::string deform_mesh(long int stageId, usdrt::SdfPath* path, int time) override;

private:
};


inline std::string ExampleCppUsdrtExtension::get_attributes_for_prim(long int stageId, usdrt::SdfPath* path, std::vector<usdrt::UsdAttribute>* data)
{
    
    if (!path || path->IsEmpty()) {
        return "Nothing selected";
    }

    if (data == nullptr) {
        return "Invalid data";
    }

    usdrt::UsdStageRefPtr stage = usdrt::UsdStage::Attach(omni::fabric::UsdStageId(stageId));

    // If a prim does not already exist in Fabric,
    // it will be fetched from USD by simply creating the
    // Usd.Prim object. At this time, only the attributes that have
    // authored opinions will be fetch into Fabric.
    usdrt::UsdPrim prim = stage->GetPrimAtPath(*path);
    if (!prim) {
        return "Prim at path " + path->GetString() + " is not in Fabric";
                
    }
    // This diverges a bit from USD - only attributes
    // that exist in Fabric are returned by this API
    std::vector<usdrt::UsdAttribute> attrs = prim.GetAttributes();
    *data = attrs;

    return "";
}
 
inline std::string ExampleCppUsdrtExtension::apply_random_rotation(long int stageId, usdrt::SdfPath* path, usdrt::GfQuatf* rot) {
    //Apply a random world space rotation to a prim in Fabric
    if (!path || path->IsEmpty()) {
        return "Nothing selected";
    }

    if (rot == nullptr) {
        return "Invalid data";
    }

    usdrt::UsdStageRefPtr stage = usdrt::UsdStage::Attach(omni::fabric::UsdStageId(stageId));
    usdrt::UsdPrim prim = stage->GetPrimAtPath(*path);
    if (!prim) {
        return "Prim at path " + path->GetString() + " is not in Fabric";
    }

    usdrt::RtXformable rtxformable = usdrt::RtXformable(prim);
    if (!rtxformable.HasWorldXform()) {
        rtxformable.SetWorldXformFromUsd();
    }

    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    float angle = dist(gen) * M_PI * 2;
    usdrt::GfVec3f axis = usdrt::GfVec3f(dist(gen), dist(gen), dist(gen)).GetNormalized();
    float halfangle = angle / 2.0;
    float shalfangle = sin(halfangle);
    usdrt::GfQuatf rotation = usdrt::GfQuatf(cos(halfangle), axis[0] * shalfangle, axis[1] * shalfangle, axis[2] * shalfangle);

    rtxformable.GetWorldOrientationAttr().Set(rotation);
    *rot = rotation;

    return "";
}


inline std::string  ExampleCppUsdrtExtension::deform_mesh(long int stageId, usdrt::SdfPath* path, int time)  {
    // Deform a Mesh prim
    if (!path || path->IsEmpty()) {
        return "Nothing selected";
    }

    usdrt::UsdStageRefPtr stage = usdrt::UsdStage::Attach(omni::fabric::UsdStageId(stageId));
    usdrt::UsdPrim prim = stage->GetPrimAtPath(*path);
    if (!prim) {
        return "Prim at path " + path->GetString() + " is not in Fabric";

    }

    if (!prim.HasAttribute("points")) {
        return "Prim at path " + path->GetString() + " does not have points attribute";
    }

    // Tell OmniHydra to render points from Fabric
    if (!prim.HasAttribute("Deformable")) {
        prim.CreateAttribute("Deformable", usdrt::SdfValueTypeNames->PrimTypeTag, true);
    }
    usdrt::UsdAttribute points = prim.GetAttribute("points");
    usdrt::VtArray<usdrt::GfVec3f> pointsarray;
    points.Get(&pointsarray);

    // Deform points
    // In the python example, this uses warp to run the deformation on GPU
    // The more correct C++ equivalent would be to write a CUDA kernel for this
    // but for simplicity of this example, do the deformation here on CPU.
    for (usdrt::GfVec3f& point : pointsarray) {
        float offset = -sin(point[0]);
        float scale = sin(time) * 10.0;

        point = point + usdrt::GfVec3f(0.0, offset * scale, 0.0);
    }

    points.Set(pointsarray);

    return "Deformed points on prim " + path->GetString();
}

}
}
}
}

CARB_PLUGIN_IMPL(pluginImplDesc, omni::example::cpp::usdruntime::ExampleCppUsdrtExtension)

void fillInterface(omni::example::cpp::usdruntime::ExampleCppUsdrtExtension& iface)
{
}
