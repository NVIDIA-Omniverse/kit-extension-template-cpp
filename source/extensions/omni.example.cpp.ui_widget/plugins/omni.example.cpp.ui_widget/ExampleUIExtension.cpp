// Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.
//
// NVIDIA CORPORATION and its licensors retain all intellectual property
// and proprietary rights in and to this software, related documentation
// and any modifications thereto. Any use, reproduction, disclosure or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA CORPORATION is strictly prohibited.
//
#include <carb/BindingsUtils.h>

#include <omni/ui/Api.h>

#ifdef _WIN32
#    include <Windows.h>
#endif

// We need to be registered as Carbonite client because we need to use Carbonite interfaces.
CARB_BINDINGS("omni.example.cpp.ui_widget")
