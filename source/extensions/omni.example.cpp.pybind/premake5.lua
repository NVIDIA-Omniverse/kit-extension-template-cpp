-- Setup the extension.
local ext = get_current_extension_info()
project_ext(ext)

-- Link folders that should be packaged with the extension.
repo_build.prebuild_link {
    { "data", ext.target_dir.."/data" },
    { "docs", ext.target_dir.."/docs" },
}

-- Build the C++ plugin that will be loaded by the extension.
project_ext_plugin(ext, "omni.example.cpp.pybind.plugin")
    add_files("include", "include/omni/example/cpp/pybind")
    add_files("source", "plugins/omni.example.cpp.pybind")
    includedirs { "include", "plugins/omni.example.cpp.pybind" }

-- Build Python bindings that will be loaded by the extension.
project_ext_bindings {
    ext = ext,
    project_name = "omni.example.cpp.pybind.python",
    module = "_example_pybind_bindings",
    src = "bindings/python/omni.example.cpp.pybind",
    target_subdir = "omni/example/cpp/pybind"
}
    includedirs { "include" }
    repo_build.prebuild_link {
        { "python/impl", ext.target_dir.."/omni/example/cpp/pybind/impl" },
        { "python/tests", ext.target_dir.."/omni/example/cpp/pybind/tests" },
    }

-- Build the C++ plugin that will be loaded by the tests.
project_ext_tests(ext, "omni.example.cpp.pybind.tests")
    add_files("source", "plugins/omni.example.cpp.pybind.tests")
    includedirs { "include", "plugins/omni.example.cpp.pybind.tests", "%{target_deps}/doctest/include" }

    -- Temp: Should have been set by project_ext_tests
    libdirs {
        "%{kit_sdk}/exts/omni.kit.test/bin",
    }
