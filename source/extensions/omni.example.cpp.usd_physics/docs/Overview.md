# Overview

An [example C++ extension](http://omniverse-docs.s3-website-us-east-1.amazonaws.com/kit-extension-template-cpp/106.0.1/index.html) that can be used as a reference/template for creating new extensions.

Demonstrates how to create a C++ plugin that can add physics to the current USD stage by:
- Using the UsdStageCache to get a USD stage from C++.
- Adding a UsdPhysicsScene to the USD stage from C++.
- Adding a static body box to the USD stage from C++.
- Adding a rigid body box to the USD stage from C++.

Note: It is important that all USD stage reads/writes happen from the main thread:

[https://graphics.pixar.com/usd/release/api/_usd__page__multi_threading.html](https://graphics.pixar.com/usd/release/api/_usd__page__multi_threading.html)


# C++ Usage Examples


![Rigid Body Falling](rigid_body_falling.gif)


## Get the 'Active' USD Stage

```
const std::vector<PXR_NS::UsdStageRefPtr> allStages = PXR_NS::UsdUtilsStageCache::Get().GetAllStages();
PXR_NS::UsdStageRefPtr activeStage;
if (allStages.size() > 1)
{
    activeStage = allStages[0];
}
```


## Create a USD Physics Scene


```
static const PXR_NS::SdfPath kPhysicsScenePath("/World/PhysicsScene");
if (!activeStage->GetPrimAtPath(kPhysicsScenePath))
{
    static constexpr PXR_NS::GfVec3f kGravityDirection = { 0.0f, -1.0f, 0.0f };
    static constexpr float kGravityMagnitude = 981.0f;

    PXR_NS::UsdPhysicsScene physicsScene = PXR_NS::UsdPhysicsScene::Define(activeStage, kPhysicsScenePath);
    physicsScene.CreateGravityDirectionAttr().Set(kGravityDirection);
    physicsScene.CreateGravityMagnitudeAttr().Set(kGravityMagnitude);
}
```


## Create a Static Collision Box

```
static const PXR_NS::SdfPath kStaticBoxPath("/World/StaticBox");
if (!activeStage->GetPrimAtPath(kStaticBoxPath))
{
    const double kSize = 5.0 / PXR_NS::UsdGeomGetStageMetersPerUnit(activeStage);
    const PXR_NS::GfVec3f kColour = { 1.0f, 1.0f, 1.0f };
    const PXR_NS::GfVec3f kPosition = { 0.0f, -(float)kSize * 0.5f, 0.0f };

    PXR_NS::UsdGeomCube geomPrim = PXR_NS::UsdGeomCube::Define(activeStage, kStaticBoxPath);
    geomPrim.CreateSizeAttr().Set(kSize);
    geomPrim.AddTranslateOp(PXR_NS::UsdGeomXformOp::PrecisionFloat).Set(kPosition);
    geomPrim.CreateDisplayColorAttr().Set(PXR_NS::VtArray<PXR_NS::GfVec3f>({ kColour }));

    PXR_NS::UsdPhysicsCollisionAPI::Apply(geomPrim.GetPrim());
}
```


## Create a Rigid Body Box

```
const PXR_NS::SdfPath kRigidBodyBoxPath("/World/RigidBodyBox");
if (!activeStage->GetPrimAtPath(kRigidBodyBoxPath))
{
    const double kSize = 0.5 / PXR_NS::UsdGeomGetStageMetersPerUnit(activeStage);
    const PXR_NS::GfVec3f kColour = { 0.0f, 0.0f, 1.0f };
    const PXR_NS::GfVec3f kPosition = { 0.0f, (float)kSize * 5.0f, 0.0f };

    PXR_NS::UsdGeomCube geomPrim = PXR_NS::UsdGeomCube::Define(activeStage, kRigidBodyBoxPath);
    geomPrim.CreateSizeAttr().Set(kSize);
    geomPrim.AddTranslateOp(PXR_NS::UsdGeomXformOp::PrecisionFloat).Set(kPosition);
    geomPrim.CreateDisplayColorAttr().Set(PXR_NS::VtArray<PXR_NS::GfVec3f>({ kColour }));

    static constexpr PXR_NS::GfVec3f kVelocity = { 2.0f, 1.0f, 2.0f };
    static constexpr PXR_NS::GfVec3f kAngularVelocity = { 180.0f, 0.0f, 0.0f };

    PXR_NS::UsdPhysicsCollisionAPI::Apply(geomPrim.GetPrim());
    PXR_NS::UsdPhysicsMassAPI::Apply(geomPrim.GetPrim());
    auto rigidBodyAPI = PXR_NS::UsdPhysicsRigidBodyAPI::Apply(geomPrim.GetPrim());
    rigidBodyAPI.CreateVelocityAttr().Set(kVelocity);
    rigidBodyAPI.CreateAngularVelocityAttr().Set(kAngularVelocity);
}
```
