-- Get the info associated with this extension.
local ext = get_current_extension_info()

-- Link folders that should be packaged with the extension.
project_ext(ext)
    repo_build.prebuild_link {
        { "data", ext.target_dir.."/data" },
        { "docs", ext.target_dir.."/docs" },
        { "omni", ext.target_dir.."/omni" },
    }
