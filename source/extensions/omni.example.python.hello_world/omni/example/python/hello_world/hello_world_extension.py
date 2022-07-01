## Copyright (c) 2022, NVIDIA CORPORATION.  All rights reserved.
##
## NVIDIA CORPORATION and its licensors retain all intellectual property
## and proprietary rights in and to this software, related documentation
## and any modifications thereto.  Any use, reproduction, disclosure or
## distribution of this software and related documentation without an express
## license agreement from NVIDIA CORPORATION is strictly prohibited.
##
import omni.ext


# Functions and variables are available to other extensions which import this module.
def hello_from(caller: str):
    print(f"[omni.example.python.hello_world] hello_from was called from {caller}.")
    return "Hello back from omni.example.python.hello_world!"


def hello_squared(x: int):
    print(f"[omni.example.python.hello_world] hello_squared was called with {x}.")
    return x**x


# When this extension is enabled, any class that derives from 'omni.ext.IExt'
# declared in the top level module (see 'python.modules' of 'extension.toml')
# will be instantiated and 'on_startup(ext_id)' called. When the extension is
# later disabled, a matching 'on_shutdown()' call will be made on the object.
class ExamplePythonHelloWorldExtension(omni.ext.IExt):

    # ext_id can be used to query the extension manager for additional information about
    # this extension, for example the location of this extension in the local filesystem.
    def on_startup(self, ext_id):
        print(f"ExamplePythonHelloWorldExtension starting up (ext_id: {ext_id}).")

    def on_shutdown(self):
        print(f"ExamplePythonHelloWorldExtension shutting down.")
