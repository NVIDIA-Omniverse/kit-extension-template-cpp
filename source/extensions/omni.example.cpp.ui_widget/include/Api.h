// Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.
//
// NVIDIA CORPORATION and its licensors retain all intellectual property
// and proprietary rights in and to this software, related documentation
// and any modifications thereto. Any use, reproduction, disclosure or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA CORPORATION is strictly prohibited.
//
#pragma once

#ifdef _WIN32
// When we build DLL, we need the __declspec(dllexport) keyword to export class
// member functions from a DLL. __declspec(dllexport) adds the export directive
// to the object file, so you do not need to use a .def file.
#    define OMNIUI_CPPWIDGET_EXPORT __declspec(dllexport)
// When we build a third-party library that uses public symbols defined by this
// DLL, the symbols must be imported. We need to use __declspec(dllimport) on
// the declarations of the public symbols.
#    define OMNIUI_CPPWIDGET_IMPORT __declspec(dllimport)
#    define OMNIUI_CPPWIDGET_CLASS_EXPORT
#else
// It works similarly in Linux. The symbols must be visible in DSO because we
// build with the compiler option -fvisibility=hidden.
#    define OMNIUI_CPPWIDGET_EXPORT __attribute__((visibility("default")))
// But to use them we don't need to use any dirrective.
#    define OMNIUI_CPPWIDGET_IMPORT
// typeinfo of the class should be visible in DSO as well.
#    define OMNIUI_CPPWIDGET_CLASS_EXPORT __attribute__((visibility("default")))
#endif

#if defined(OMNIUI_CPPWIDGET_STATIC)
#    define OMNIUI_CPPWIDGET_API
#    define OMNIUI_CPPWIDGET_CLASS_API
#else
#    if defined(OMNIUI_CPPWIDGET_EXPORTS)
#        define OMNIUI_CPPWIDGET_API OMNIUI_CPPWIDGET_EXPORT
#        define OMNIUI_CPPWIDGET_CLASS_API OMNIUI_CPPWIDGET_CLASS_EXPORT
#    else
#        define OMNIUI_CPPWIDGET_API OMNIUI_CPPWIDGET_IMPORT
#        define OMNIUI_CPPWIDGET_CLASS_API
#    endif
#endif

#define OMNIUI_CPPWIDGET_NS omni::ui::example_cpp_widget
#define OMNIUI_CPPWIDGET_NAMESPACE_USING_DIRECTIVE using namespace OMNIUI_CPPWIDGET_NS;
#define OMNIUI_CPPWIDGET_NAMESPACE_OPEN_SCOPE                                                                          \
    namespace omni                                                                                                     \
    {                                                                                                                  \
    namespace ui                                                                                                       \
    {                                                                                                                  \
    namespace example_cpp_widget                                                                                       \
    {
#define OMNIUI_CPPWIDGET_NAMESPACE_CLOSE_SCOPE                                                                         \
    }                                                                                                                  \
    }                                                                                                                  \
    }
