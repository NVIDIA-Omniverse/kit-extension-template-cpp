-- Setup the extension.
local ext = get_current_extension_info()
project_ext(ext)

-- Link folders that should be packaged with the extension.
repo_build.prebuild_link {
    { "data", ext.target_dir.."/data" },
    { "docs", ext.target_dir.."/docs" },
}

-- Build the C++ plugin that will be loaded by the extension.
project_ext_plugin(ext, "omni.example.cpp.ui_widget")
    local plugin_name = "omni.example.cpp.ui_widget"
    add_files("source", "plugins/"..plugin_name)
    add_files("include", "include")
    defines { "OMNIUI_CPPWIDGET_EXPORTS", "IMGUI_NVIDIA" }
    exceptionhandling "On"
    rtti "On"
    includedirs {
        "include",
        "plugins/"..plugin_name,
        "%{target_deps}/boost-preprocessor",
        "%{target_deps}/imgui"
    }
    links { "carb", "%{kit_sdk}/exts/omni.ui/bin/omni.ui", "%{kit_sdk}/exts/omni.kit.renderer.imgui/bin/imgui" }

-- Build Python bindings that will be loaded by the extension.
project_ext_bindings {
    ext = ext,
    project_name = "omni.example.cpp.ui_widget.python",
    module = "_example_cpp_widget",
    src = "bindings/python/omni.example.cpp.ui_widget",
    target_subdir = "omni/example/cpp/ui_widget"
}
    includedirs {
        "include",
        "bindings/python/omni.example.cpp.ui_widget",
        "%{target_deps}/boost-preprocessor",
        "%{target_deps}/imgui"
    }
    links { "omni.example.cpp.ui_widget", "%{kit_sdk}/exts/omni.ui/bin/omni.ui" }

    repo_build.prebuild_link {
        { "python/tests", ext.target_dir.."/omni/example/cpp/ui_widget/tests" },
    }
