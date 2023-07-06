// Copyright (c) 2020-2021, NVIDIA CORPORATION. All rights reserved.
//
// NVIDIA CORPORATION and its licensors retain all intellectual property
// and proprietary rights in and to this software, related documentation
// and any modifications thereto.  Any use, reproduction, disclosure or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA CORPORATION is strictly prohibited.
//

#include <omni/example/cpp/pybind/IExampleBoundInterface.h>
#include <omni/example/cpp/pybind/ExampleBoundObject.h>

#include <doctest/doctest.h>

#include <carb/BindingsUtils.h>

CARB_BINDINGS("omni.example.cpp.pybind.tests")

namespace omni
{
namespace example
{
namespace cpp
{
namespace pybind
{

class ExampleCppObject : public ExampleBoundObject
{
public:
    static carb::ObjectPtr<IExampleBoundObject> create(const char* id)
    {
        return carb::stealObject<IExampleBoundObject>(new ExampleCppObject(id));
    }

    ExampleCppObject(const char* id)
        : ExampleBoundObject(id)
    {
    }
};

class ExamplePybindTestFixture
{
public:
    static constexpr const char* k_registeredObjectId = "example_bound_object";

    ExamplePybindTestFixture()
        : m_exampleBoundInterface(carb::getCachedInterface<omni::example::cpp::pybind::IExampleBoundInterface>())
        , m_exampleBoundObject(ExampleCppObject::create(k_registeredObjectId))
    {
        m_exampleBoundInterface->registerBoundObject(m_exampleBoundObject);
    }

    ~ExamplePybindTestFixture()
    {
        m_exampleBoundInterface->deregisterBoundObject(m_exampleBoundObject);
    }

protected:
    IExampleBoundInterface* getExampleBoundInterface()
    {
        return m_exampleBoundInterface;
    }

    carb::ObjectPtr<IExampleBoundObject> getExampleBoundObject()
    {
        return m_exampleBoundObject;
    }

private:
    IExampleBoundInterface* m_exampleBoundInterface = nullptr;
    carb::ObjectPtr<IExampleBoundObject> m_exampleBoundObject;
};

}
}
}
}

TEST_SUITE("omni.example.cpp.pybind.tests")
{
    using namespace omni::example::cpp::pybind;

    TEST_CASE_FIXTURE(ExamplePybindTestFixture, "Get Example Bound Interface")
    {
        CHECK(getExampleBoundInterface() != nullptr);
    }

    TEST_CASE_FIXTURE(ExamplePybindTestFixture, "Get Example Bound Object")
    {
        CHECK(getExampleBoundObject().get() != nullptr);
    }

    TEST_CASE_FIXTURE(ExamplePybindTestFixture, "Find Example Bound Object")
    {
        SUBCASE("Registered")
        {
            carb::ObjectPtr<IExampleBoundObject> foundObject = getExampleBoundInterface()->findBoundObject(k_registeredObjectId);
            CHECK(foundObject.get() == getExampleBoundObject().get());
            CHECK(foundObject.get() != nullptr);
        }

        SUBCASE("Unregistered")
        {
            carb::ObjectPtr<IExampleBoundObject> foundObject = getExampleBoundInterface()->findBoundObject("unregistered_object_id");
            CHECK(foundObject.get() != getExampleBoundObject().get());
            CHECK(foundObject.get() == nullptr);
        }
    }
}
