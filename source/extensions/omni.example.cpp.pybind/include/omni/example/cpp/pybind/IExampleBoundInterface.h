// Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.
//
// NVIDIA CORPORATION and its licensors retain all intellectual property
// and proprietary rights in and to this software, related documentation
// and any modifications thereto.  Any use, reproduction, disclosure or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA CORPORATION is strictly prohibited.
//
#pragma once

#include <omni/example/cpp/pybind/IExampleBoundObject.h>

#include <carb/Interface.h>

namespace omni
{
namespace example
{
namespace cpp
{
namespace pybind
{

/**
 * An example interface to demonstrate reflection using pybind.
 */
class IExampleBoundInterface
{
public:
    /// @private
    CARB_PLUGIN_INTERFACE("omni::example::cpp::pybind::IExampleBoundInterface", 1, 0);

    /**
     * Register a bound object.
     *
     * @param object The bound object to register.
     */
    virtual void registerBoundObject(carb::ObjectPtr<IExampleBoundObject>& object) = 0;

    /**
     * Deregister a bound object.
     *
     * @param object The bound object to deregister.
     */
    virtual void deregisterBoundObject(carb::ObjectPtr<IExampleBoundObject>& object) = 0;

    /**
     * Find a bound object.
     *
     * @param id Id of the bound object.
     *
     * @return The bound object if it exists, an empty ObjectPtr otherwise.
     */
    virtual carb::ObjectPtr<IExampleBoundObject> findBoundObject(const char* id) const = 0;
};

}
}
}
}
