-- Get the info associated with this extension.
local ext = get_current_extension_info()

-- Link folders that should be packaged with the extension.
project_ext(ext)
repo_build.prebuild_link {
    { "data", ext.target_dir.."/data" },
    { "docs", ext.target_dir.."/docs" },
}

-- Build the C++ plugin that will be loaded by the extension.
project_ext_plugin(ext, "omni.example.cpp.pybind.plugin")
    add_files("include", "%{root}/include/omni/example/cpp/pybind")
    local plugin_name = "omni.example.cpp.pybind"
    add_files("source", "plugins/"..plugin_name)
    includedirs { "plugins/"..plugin_name }

-- Build Python bindings that will be loaded by the extension.
project_ext_bindings {
    ext = ext,
    project_name = "omni.example.cpp.pybind.python",
    module = "_example_pybind_bindings",
    src = "bindings/python/omni.example.cpp.pybind",
    target_subdir = "omni/example/cpp/pybind"
}
    repo_build.prebuild_link {
        { "python/impl", ext.target_dir.."/omni/example/cpp/pybind/impl" },
        { "python/tests", ext.target_dir.."/omni/example/cpp/pybind/tests" },
    }
