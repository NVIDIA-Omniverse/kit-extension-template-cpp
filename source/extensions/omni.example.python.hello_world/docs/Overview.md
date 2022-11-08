# Overview

An example Python extension that can be used as a reference/template for creating new extensions.

Demonstrates how to create a Python module that will startup / shutdown along with the extension.

Also demonstrates how to expose Python functions so that they can be called from other extensions.


# Python Usage Examples


## Defining Extensions


```
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
```

