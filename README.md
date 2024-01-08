# Omniverse Kit C++ Extension Template

This project contains everything necessary to develop extensions that contain C++ code, along with a number of examples demonstrating best practices for creating them.


## What Are Extensions?

While an extension can consist of a single `extension.toml` file, most contain Python code, C++ code, or a mixture of both:

```
                                                     Kit
                                                      |
                   ___________________________________|___________________________________
                  |                                   |                                   |
             Python Only                            C++ Only                            Mixed
(eg. omni.example.python.hello_world)  (eg. omni.example.cpp.hello_world)  (eg. omni.example.cpp.pybind)
```

Extensive documentation detailing what extensions are and how they work can be found [here](https://docs.omniverse.nvidia.com/py/kit/docs/guide/extensions.html).


## Getting Started

1. Clone the [GitHub repo](https://github.com/NVIDIA-Omniverse/kit-extension-template-cpp) to your local machine.
2. Open a command prompt and navigate to the root of your cloned repo.
3. Run `build.bat` to bootstrap your dev environment and build the example extensions.
4. Run `_build\{platform}\release\omni.app.example.extension_browser.bat` to open an example kit application.
    - Run `omni.app.example.viewport.bat` instead if you want the renderer and main viewport to be enabled.
    - Run `omni.app.kit.dev.bat` instead if you want the full kit developer experience to be enabled.
5. From the menu, select `Window->Extensions` to open the extension browser window.
6. Enter `omni.example.cpp` in the search bar at the top of the extension browser window to view the example extensions included with this repo.

![Extension Browser](/images/extension_browser.png)


## Debugging C++ Extensions

1. Run `build.bat` (if you haven't already) to generate the solution file.
2. Open `_compiler\vs2019\kit-extension-template-cpp.sln` using Visual Studio 2019.
3. Select `omni.app.example.extension_browser` as the startup project (if it isn't already).
    - Select `omni.app.example.viewport` instead if you want the renderer and main viewport to be enabled.
    - Select `omni.app.kit.dev` instead if you want the full kit developer experience to be enabled.
4. Run/debug the example kit application, using the extension browser window to enable/disable extensions.

![Visual Studio Solution](/images/visual_studio_solution.png)


## Creating New C++ Extensions

1. Copy one of the existing extension examples to a new folder within the `source/extensions` folder.
    - The name of the new folder will be the name of your new extension.
    - The **omni** prefix is reserved for NVIDIA applications and extensions.
2. Update the fields in your new extension's `config/extension.toml` file as necessary.
3. Update your new extension's `premake5.lua` file as necessary.
4. Update your new extension's C++ code in the `plugins` folder as necessary.
5. Update your new extension's Python code in the `python` folder as necessary.
6. Update your new extension's Python bindings in the `bindings` folder as necessary.
7. Update your new extension's documentation in the `docs` folder as necessary.
8. Run `build.bat` to build your new extension.
9. Refer to the *Getting Started* section above to open the example kit application and extension browser window.
10. Enter the name of your new extension in the search bar at the top of the extension browser window to view it.


## Generating Documentation

1. Run `repo.bat docs` to generate the documentation for the repo, including all extensions it contains.
    - You can generate the documentation for a single extension by running `repo.bat docs -p {extension_name}`
2. Open `_build/docs/kit-extension-template-cpp/latest/index.html` to view the generated documentation.

## Publishing

Developers can publish publicly hosted extensions to the community extension registry using the following steps:

1. Tag the GitHub repository with the **[omniverse-kit-extension](https://github.com/topics/omniverse-kit-extension)** tag.

2. Create a [GitHub release](https://docs.github.com/en/repositories/releasing-projects-on-github/managing-releases-in-a-repository).

3. Upload the packaged extension archives, created with `./repo.bat package` on Windows or `./repo.sh package` on Linux, to the GitHub release. You must rename the packaged extension archive to match the following convention:

    - Linux: `{github-namespace}-{github-repository}-linux-x86_64-{github-release-tag}.zip`
    - Windows: `{github-namespace}-{github-repository}-windows-x86_64-{github-release-tag}.zip`

For example, the v0.0.2 release of the extension at <https://github.com/jshrake-nvidia/kit-community-release-test/releases/tag/v0.0.2> has archives named `jshrake-nvidia-kit-community-release-test-linux-x86_64-v0.0.2.zip` and `jshrake-nvidia-kit-community-release-test-windows-x86_64-v0.0.2.zip` for Linux and Windows, respectively.

Our publishing pipeline runs nightly and discovers any publicly hosted GitHub repository with the `omniverse-kit-extension` tag. The published extensions should be visible in the community registry the day following the creation of a GitHub release.

Refer to [the kit extension documentation](https://docs.omniverse.nvidia.com/kit/docs/kit-manual/latest/guide/extensions_advanced.html#package-writetarget) for how to specify the Kit version compatibility for your extension. This ensures that the correct version of your extension is listed in any given Kit application.

## Contributing

The source code for this repository is provided as-is and we are not accepting outside contributions.
