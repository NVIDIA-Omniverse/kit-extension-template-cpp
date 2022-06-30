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

#include <omni/ext/IExt.h>

#include <omni/example/cpp/pybind/IExampleBoundInterface.h>

#include <unordered_map>

const struct carb::PluginImplDesc pluginImplDesc = { "omni.example.cpp.pybind.plugin",
                                                     "An example C++ extension.", "NVIDIA",
                                                     carb::PluginHotReload::eEnabled, "dev" };

namespace omni
{
namespace example
{
namespace cpp
{
namespace pybind
{

class ExampleBoundImplementation : public IExampleBoundInterface
{
public:
    void registerBoundObject(carb::ObjectPtr<IExampleBoundObject>& object) override
    {
        if (object)
        {
            m_registeredObjectsById[object->getId()] = object;
        }
    }

    void deregisterBoundObject(carb::ObjectPtr<IExampleBoundObject>& object) override
    {
        if (object)
        {
            const auto& it = m_registeredObjectsById.find(object->getId());
            if (it != m_registeredObjectsById.end())
            {
                m_registeredObjectsById.erase(it);
            }
        }
    }

    carb::ObjectPtr<IExampleBoundObject> findBoundObject(const char* id) const override
    {
        const auto& it = m_registeredObjectsById.find(id);
        if (it != m_registeredObjectsById.end())
        {
            return it->second;
        }

        return carb::ObjectPtr<IExampleBoundObject>();
    }

private:
    std::unordered_map<std::string, carb::ObjectPtr<IExampleBoundObject>> m_registeredObjectsById;
};

}
}
}
}

CARB_PLUGIN_IMPL(pluginImplDesc, omni::example::cpp::pybind::ExampleBoundImplementation)

void fillInterface(omni::example::cpp::pybind::ExampleBoundImplementation& iface)
{
}
