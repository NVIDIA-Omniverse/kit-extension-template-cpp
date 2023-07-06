// Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.
//
// NVIDIA CORPORATION and its licensors retain all intellectual property
// and proprietary rights in and to this software, related documentation
// and any modifications thereto. Any use, reproduction, disclosure or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA CORPORATION is strictly prohibited.
//
#include <carb/BindingsUtils.h>

#include <omni/ui/bind/BindUtils.h>
#include <pybind11/pybind11.h>

// We need to be registered as Carbonite plugin because we need to use CARB_LOG_ERROR
CARB_BINDINGS("omni.example.cpp.ui_widget.python")

PYBIND11_MODULE(_example_cpp_widget, m)
{
    pybind11::module::import("omni.ui");

    OMNIUI_BIND(CppWidget);
}
