// Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.
//
// NVIDIA CORPORATION and its licensors retain all intellectual property
// and proprietary rights in and to this software, related documentation
// and any modifications thereto. Any use, reproduction, disclosure or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA CORPORATION is strictly prohibited.
//
#include <omni/ui/bind/BindUtils.h>

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <CppWidget.h>
#include <BindCppWidget.h>

using namespace pybind11;

OMNIUI_NAMESPACE_USING_DIRECTIVE
OMNIUI_CPPWIDGET_NAMESPACE_USING_DIRECTIVE

void wrapCppWidget(module& m)
{
    const char* cppWidgetDoc = OMNIUI_PYBIND_CLASS_DOC(CppWidget);
    const char* cppWidgetConstructorDoc = OMNIUI_PYBIND_CONSTRUCTOR_DOC(CppWidget, CppWidget);

    class_<CppWidget, omni::ui::Widget, std::shared_ptr<CppWidget>>(m, "CppWidget", cppWidgetDoc)
        .def(init([](kwargs kwargs) { OMNIUI_PYBIND_INIT(CppWidget) }), cppWidgetConstructorDoc)
        .def_property(
            "thickness", &CppWidget::getThickness, &CppWidget::setThickness, OMNIUI_PYBIND_DOC_CppWidget_thickness)
        /* */;
}
