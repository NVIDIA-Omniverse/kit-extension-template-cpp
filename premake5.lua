-- Shared build scripts from repo_build package.
repo_build = require("omni/repo/build")

-- Repo root
root = repo_build.get_abs_path(".")

-- Insert the kit template premake configuration, which creates the solution, finds extensions, etc.
dofile("_repo/deps/repo_kit_tools/kit-template/premake5.lua")

-- Override the kit template settings to use the versions of msvc and winsdk pulled in host-deps.packman.xml.
workspace(_OPTIONS["solution-name"])
    filter { "system:windows" }
        systemversion "10.0.18362.0"

        -- system include dirs:
        local msvcInclude = host_deps.."/msvc/VC/Tools/MSVC/14.27.29110/include"
        local sdkInclude = { 
            host_deps.."/winsdk/include/winrt", 
            host_deps.."/winsdk/include/um", 
            host_deps.."/winsdk/include/ucrt", 
            host_deps.."/winsdk/include/shared" 
        }
        sysincludedirs { msvcInclude, sdkInclude }

        -- system lib dirs:
        local msvcLibs = host_deps.."/msvc/VC/Tools/MSVC/14.27.29110/lib/onecore/x64"
        local sdkLibs = { host_deps.."/winsdk/lib/ucrt/x64", host_deps.."/winsdk/lib/um/x64" }
        syslibdirs { msvcLibs, sdkLibs }

        -- system binary dirs:
        bindirs { 
            host_deps.."/msvc/VC/Tools/MSVC/14.27.29110/bin/HostX64/x64", 
            host_deps.."/msvc/MSBuild/Current/Bin", host_deps.."/winsdk/bin/x64" 
        }

-- Example application that runs kit with a minimal set of extensions enabled, including the extension browser.
define_app("omni.app.example.extension_browser")
