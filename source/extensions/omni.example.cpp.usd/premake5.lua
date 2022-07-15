-- Setup the extension.
local ext = get_current_extension_info()
project_ext(ext)

-- Link folders that should be packaged with the extension.
repo_build.prebuild_link {
    { "data", ext.target_dir.."/data" },
    { "docs", ext.target_dir.."/docs" },
    { "omni", ext.target_dir.."/omni" },
}

-- Build the C++ plugin that will be loaded by the extension.
project_ext_plugin(ext, "omni.example.cpp.usd.plugin")
    local plugin_name = "omni.example.cpp.usd"
    add_files("source", "plugins/"..plugin_name)
    includedirs { "plugins/"..plugin_name, "%{target_deps}/nv_usd/%{cfg.buildcfg}/include" }
    libdirs { "%{target_deps}/nv_usd/%{cfg.buildcfg}/lib" }
    links { "gf", "sdf", "tf", "usd", "usdGeom", "usdUtils" }
    defines { "NOMINMAX", "TBB_USE_DEBUG=%{cfg.buildcfg == 'debug' and 1 or 0}" }
    buildoptions { "/wd4244 /wd4305" }
