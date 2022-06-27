-- Get the info associated with this extension.
local ext = get_current_extension_info()

-- Link folders that should be packaged with the extension.
project_ext(ext)
repo_build.prebuild_link {
    { "data", ext.target_dir.."/data" },
    { "docs", ext.target_dir.."/docs" },
}

-- Build the C++ plugin that will be loaded by the extension.
-- The plugin must implement the omni::ext::IExt interface to
-- be automatically loaded by the extension system at startup.
project_ext_plugin(ext, "omni.example.cpp.hello_world.plugin")
    local plugin_name = "omni.example.cpp.hello_world"
    add_files("source", "plugins/"..plugin_name)
    includedirs { "plugins/"..plugin_name }
