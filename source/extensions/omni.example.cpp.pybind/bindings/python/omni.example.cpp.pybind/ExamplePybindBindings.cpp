// Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.
//
// NVIDIA CORPORATION and its licensors retain all intellectual property
// and proprietary rights in and to this software, related documentation
// and any modifications thereto.  Any use, reproduction, disclosure or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA CORPORATION is strictly prohibited.
//

#include <carb/BindingsPythonUtils.h>

#include <omni/example/cpp/pybind/ExampleBoundObject.h>
#include <omni/example/cpp/pybind/IExampleBoundInterface.h>

#include <string>

CARB_BINDINGS("omni.example.cpp.pybind.python")

DISABLE_PYBIND11_DYNAMIC_CAST(omni::example::cpp::pybind::IExampleBoundInterface)
DISABLE_PYBIND11_DYNAMIC_CAST(omni::example::cpp::pybind::IExampleBoundObject)

namespace
{

/**
 * Concrete bound object class that will be reflected to Python.
 */
class PythonBoundObject : public omni::example::cpp::pybind::ExampleBoundObject
{
public:
    /**
     * Factory.
     *
     * @param id Id of the bound action.
     *
     * @return The bound object that was created.
     */
    static carb::ObjectPtr<PythonBoundObject> create(const char* id)
    {
        // Note: It is important to construct the handler using ObjectPtr<T>::InitPolicy::eSteal,
        // otherwise we end up incresing the reference count by one too many during construction,
        // resulting in carb::ObjectPtr<T> instance whose wrapped object will never be destroyed.
        return carb::stealObject<PythonBoundObject>(new PythonBoundObject(id));
    }

    /**
     * Constructor.
     *
     * @param id Id of the bound object.
     */
    PythonBoundObject(const char* id)
        : ExampleBoundObject(id)
        , m_memberInt(0)
        , m_memberBool(false)
        , m_memberString()
    {
    }

    // To deomnstrate binding a fuction that accepts an argument.
    void multiplyIntProperty(int value)
    {
        m_memberInt *= value;
    }

    // To deomnstrate binding a fuction that returns a value.
    bool toggleBoolProperty()
    {
        m_memberBool = !m_memberBool;
        return m_memberBool;
    }

    // To deomnstrate binding a fuction that accepts an argument and returns a value.
    const char* appendStringProperty(const char* value)
    {
        m_memberString += value;
        return m_memberString.c_str();
    }

    // To deomnstrate binding properties using accessors.
    const char* getMemberString() const
    {
        return m_memberString.c_str();
    }

    // To deomnstrate binding properties using accessors.
    void setMemberString(const char* value)
    {
        m_memberString = value;
    }

    // To deomnstrate binding properties directly.
    int m_memberInt;
    bool m_memberBool;

private:
    // To deomnstrate binding properties using accessors.
    std::string m_memberString;
};

// Define the pybind11 module using the same name specified in premake5.lua
PYBIND11_MODULE(_example_pybind_bindings, m)
{
    using namespace omni::example::cpp::pybind;

    m.doc() = "pybind11 omni.example.cpp.pybind bindings";

    carb::defineInterfaceClass<IExampleBoundInterface>(
        m, "IExampleBoundInterface", "acquire_bound_interface", "release_bound_interface")
        .def("register_bound_object", &IExampleBoundInterface::registerBoundObject,
             R"(
             Register a bound object.

             Args:
                 object: The bound object to register.
             )",
             py::arg("object"))
        .def("deregister_bound_object", &IExampleBoundInterface::deregisterBoundObject,
             R"(
             Deregister a bound object.

             Args:
                 object: The bound object to deregister.
             )",
             py::arg("object"))
        .def("find_bound_object", &IExampleBoundInterface::findBoundObject, py::return_value_policy::reference,
             R"(
             Find a bound object.

             Args:
                 id: Id of the bound object.

             Return:
                 The bound object if it exists, an empty object otherwise.
             )",
             py::arg("id"))
        /**/;

    py::class_<IExampleBoundObject, carb::ObjectPtr<IExampleBoundObject>>(m, "IExampleBoundObject")
        .def_property_readonly("id", &IExampleBoundObject::getId, py::return_value_policy::reference,
            R"(
             Get the id of this bound object.

             Return:
                 The id of this bound object.
             )")
        /**/;

    py::class_<PythonBoundObject, IExampleBoundObject, carb::ObjectPtr<PythonBoundObject>>(m, "BoundObject")
        .def(py::init([](const char* id) { return PythonBoundObject::create(id); }),
             R"(
             Create a bound object.

             Args:
                 id: Id of the bound object.

             Return:
                 The bound object that was created.
             )",
             py::arg("id"))
        .def_readwrite("property_int", &PythonBoundObject::m_memberInt,
             R"(
             Int property bound directly.
             )")
        .def_readwrite("property_bool", &PythonBoundObject::m_memberBool,
             R"(
             Bool property bound directly.
             )")
        .def_property("property_string", &PythonBoundObject::getMemberString, &PythonBoundObject::setMemberString, py::return_value_policy::reference,
             R"(
             String property bound using accessors.
             )")
        .def("multiply_int_property", &PythonBoundObject::multiplyIntProperty,
             R"(
             Bound fuction that accepts an argument.

             Args:
                 value_to_multiply: The value to multiply by.
             )",
             py::arg("value_to_multiply"))
        .def("toggle_bool_property", &PythonBoundObject::toggleBoolProperty,
             R"(
             Bound fuction that returns a value.

             Return:
                 The toggled bool value.
             )")
        .def("append_string_property", &PythonBoundObject::appendStringProperty, py::return_value_policy::reference,
             R"(
             Bound fuction that accepts an argument and returns a value.

             Args:
                 value_to_append: The value to append.

             Return:
                 The new string value.
             )",
             py::arg("value_to_append"))
        /**/;
}
}
