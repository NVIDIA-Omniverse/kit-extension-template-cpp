// Copyright (c) 2023, NVIDIA CORPORATION. All rights reserved.
//
// NVIDIA CORPORATION and its licensors retain all intellectual property
// and proprietary rights in and to this software, related documentation
// and any modifications thereto. Any use, reproduction, disclosure or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA CORPORATION is strictly prohibited.
//

#include <carb/BindingsPythonUtils.h>

#include <omni/example/cpp/usdrt/IExampleUsdrtInterface.h>

#include <usdrt/scenegraph/base/gf/quatf.h>
#include <usdrt/scenegraph/usd/sdf/path.h>
#include <usdrt/scenegraph/usd/usd/attribute.h>

CARB_BINDINGS("omni.example.cpp.usdrt.python")

DISABLE_PYBIND11_DYNAMIC_CAST(
    omni::example::cpp::usdruntime::IExampleUsdrtInterface)

namespace {

// Define the pybind11 module using the same name specified in premake5.lua
PYBIND11_MODULE(_example_usdrt_bindings, m) {
  using namespace omni::example::cpp::usdruntime;

  m.doc() = "pybind11 omni.example.cpp.usdrt bindings";

  carb::defineInterfaceClass<IExampleUsdrtInterface>(
      m, "IExampleUsdrtInterface", "acquire_example_usdrt_interface",
      "release_example_usdrt_interface")
      .def("get_attributes_for_prim",
           [](IExampleUsdrtInterface &self, long int id,
              usdrt::SdfPath* path) {
             std::vector<usdrt::UsdAttribute> data;
             std::string err = self.get_attributes_for_prim(id, path, &data);
             return std::make_tuple(err, data);
           },
           py::arg("stageId"), py::arg("primPath"))
      .def("apply_random_rotation",
           [](IExampleUsdrtInterface &self,long int id,
              usdrt::SdfPath* path) {
             usdrt::GfQuatf rot;
             std::string err = self.apply_random_rotation(id, path, &rot);
             return std::make_tuple(err, rot);
           },
           py::arg("stageId"), py::arg("primPath"))
      .def("deform_mesh",
           &IExampleUsdrtInterface::deform_mesh, py::arg("stageId"),
           py::arg("primPath"), py::arg("time"));
}
} // namespace
