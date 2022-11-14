// Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.
//
// NVIDIA CORPORATION and its licensors retain all intellectual property
// and proprietary rights in and to this software, related documentation
// and any modifications thereto.  Any use, reproduction, disclosure or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA CORPORATION is strictly prohibited.
//

#include <carb/BindingsPythonUtils.h>

#include <omni/example/cpp/usd/IExampleUsdInterface.h>

CARB_BINDINGS("omni.example.cpp.usd.python")

DISABLE_PYBIND11_DYNAMIC_CAST(omni::example::cpp::usd::IExampleUsdInterface)

namespace
{

// Define the pybind11 module using the same name specified in premake5.lua
PYBIND11_MODULE(_example_usd_bindings, m)
{
    using namespace omni::example::cpp::usd;

    m.doc() = "pybind11 omni.example.cpp.usd bindings";

    carb::defineInterfaceClass<IExampleUsdInterface>(
        m, "IExampleUsdInterface", "acquire_example_usd_interface", "release_example_usd_interface")
        .def("create_prims", &IExampleUsdInterface::createPrims)
        .def("remove_prims", &IExampleUsdInterface::removePrims)
        .def("print_stage_info", &IExampleUsdInterface::printStageInfo)
        .def("start_timeline_animation", &IExampleUsdInterface::startTimelineAnimation)
        .def("stop_timeline_animation", &IExampleUsdInterface::stopTimelineAnimation)
        .def("on_default_usd_stage_changed", &IExampleUsdInterface::onDefaultUsdStageChanged)
    /**/;
}
}
