-- Setup the CUDA Hello World extension.
local ext = get_current_extension_info()
project_ext(ext)

-- Link folders that should be packaged with the extension.
repo_build.prebuild_link {
    { "data", ext.target_dir.."/data" },
    { "docs", ext.target_dir.."/docs" },
    { "omni", ext.target_dir.."/omni" },
}

-- Build the C++ plugin that will be loaded by the extension.
project_ext_plugin(ext, "omni.example.cpp.hello_world_cuda.plugin")
    local plugin_name = "omni.example.cpp.hello_world_cuda"

    -- All C++ / CUDA source for the plugin.
    add_files("source", "plugins/"..plugin_name)
    includedirs {
        "plugins/"..plugin_name,
        "%{target_deps}/cuda", -- CUDA headers
    }

    -- Link against the CUDA runtime (dynamic).
    libdirs {
        "%{target_deps}/cuda/lib64",     -- Linux
        "%{target_deps}/cuda/lib/x64",   -- Windows
    }
    links { "cudart" }

    add_cuda_build_support()