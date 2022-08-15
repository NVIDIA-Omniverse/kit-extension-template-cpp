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
project_ext_plugin(ext, "omni.example.cpp.omnigraph_node.plugin")
    local plugin_name = "omni.example.cpp.omnigraph_node"
    add_files("source", "plugins/"..plugin_name)
    includedirs { "plugins/"..plugin_name }
