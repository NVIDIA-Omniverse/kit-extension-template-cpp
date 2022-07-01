// Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.
//
// NVIDIA CORPORATION and its licensors retain all intellectual property
// and proprietary rights in and to this software, related documentation
// and any modifications thereto. Any use, reproduction, disclosure or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA CORPORATION is strictly prohibited.
//
#pragma once

#include <DocCppWidget.h>

#include <omni/ui/bind/BindWidget.h>

// clang-format off
#define OMNIUI_PYBIND_INIT_CppWidget                                                                                   \
    OMNIUI_PYBIND_INIT_CAST(thickness, setThickness, float)                                                            \
    OMNIUI_PYBIND_INIT_Widget

#define OMNIUI_PYBIND_KWARGS_DOC_CppWidget                                                                             \
    "\n    `thickness : float`\n        "                                                                              \
    OMNIUI_PYBIND_DOC_CppWidget_thickness                                                                              \
    OMNIUI_PYBIND_KWARGS_DOC_Widget
// clang-format on
