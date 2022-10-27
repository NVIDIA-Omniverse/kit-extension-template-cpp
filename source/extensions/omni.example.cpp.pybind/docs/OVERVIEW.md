# Overview

An example C++ extension that can be used as a reference/template for creating new extensions.

Demonstrates how to reflect C++ code using pybind11 so that it can be called from Python code.

The IExampleBoundInterface located in `include/omni/example/cpp/pybind/IExampleBoundInterface.h` is:
- Implemented in `plugins/omni.example.cpp.pybind/ExamplePybindExtension.cpp`.
- Reflected in `bindings/python/omni.example.cpp.pybind/ExamplePybindBindings.cpp`.
- Accessed from Python in `python/tests/test_pybind_example.py` via `python/impl/example_pybind_extension.py`.

