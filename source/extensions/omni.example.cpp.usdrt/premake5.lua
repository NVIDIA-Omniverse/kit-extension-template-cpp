-- Setup the extension.
local ext = get_current_extension_info()
project_ext(ext)

-- Link folders that should be packaged with the extension.
repo_build.prebuild_link {
    { "data", ext.target_dir.."/data" },
    { "docs", ext.target_dir.."/docs" },
}

-- Build the C++ plugin that will be loaded by the extension.
project_ext_plugin(ext, "omni.example.cpp.usdrt.plugin")
    add_files("include", "include/omni/example/cpp/usdrt")
    add_files("source", "plugins/omni.example.cpp.usdrt")
    includedirs {
        "include",
        "plugins/omni.example.cpp.usdrt",
        "%{target_deps}/cuda",
        "%{target_deps}/pybind11/include",
        "%{target_deps}/python/include",
        "%{kit_sdk}/dev/gsl/include",
        "%{kit_sdk}/dev/fabric/include",
     }
    libdirs { "%{target_deps}/python/libs", "%{target_deps}/cuda/lib/x64"  }
    defines { "NOMINMAX", "NDEBUG" }
    cppdialect "C++17"
    runtime "Release"
    rtti "On"

    filter { "system:linux" }
        libdirs {"%{target_deps}/cuda/lib64/stubs"}
        links { "dl", "pthread", "rt" }
        exceptionhandling "On"
        staticruntime "Off"
        includedirs { "%{target_deps}/cuda" }
        buildoptions { "-pthread -lstdc++fs -Wno-error" }
        linkoptions { "-Wl,--disable-new-dtags -Wl,-rpath,%{target_deps}/python/lib:" }
    filter { "system:windows" }
        buildoptions { "/wd4244 /wd4305 /wd4530" }
        links { "carb", "cudart_static"}
    filter {}

-- Build Python bindings that will be loaded by the extension.
project_ext_bindings {
    ext = ext,
    project_name = "omni.example.cpp.usdrt.python",
    module = "_example_usdrt_bindings",
    src = "bindings/python/omni.example.cpp.usdrt",
    target_subdir = "omni/example/cpp/usdrt"
}
    includedirs { "include",
        "%{target_deps}/pybind11/include",
        "%{target_deps}/python/include",
        "%{kit_sdk}/dev/gsl/include",
        "%{kit_sdk}/dev/fabric/include",
    }

    cppdialect "C++17"

    filter { "system:linux" }
        libdirs {"%{target_deps}/cuda/lib64/stubs"}
        links { "dl", "pthread", "rt" }
        exceptionhandling "On"
        staticruntime "Off"
        buildoptions { "-Wno-deprecated-declarations -Wno-deprecated -Wno-unused-variable -pthread -lstdc++fs -Wno-undef" }
        linkoptions { "-Wl,--disable-new-dtags -Wl,-rpath,%{target_deps}/python/lib:" }
    filter { "system:windows" }
        buildoptions { "/wd4244 /wd4305 /wd4530" }
    filter {}
    repo_build.prebuild_link {
        { "python/impl", ext.target_dir.."/omni/example/cpp/usdrt/impl" },
        { "python/tests", ext.target_dir.."/omni/example/cpp/usdrt/tests" },
    }
