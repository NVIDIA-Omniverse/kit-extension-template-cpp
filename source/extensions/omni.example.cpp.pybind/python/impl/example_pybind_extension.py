import omni.ext
from .._example_pybind_bindings import *

# Global public interface object.
_bound_interface = None

# Public API.
def get_bound_interface() -> IExampleBoundInterface:
    return _bound_interface


# Use the extension entry points to acquire and release the interface.
class ExamplePybindExtension(omni.ext.IExt):
    def __init__(self):
        global _bound_interface
        _bound_interface = acquire_bound_interface()

    def on_shutdown(self):
        global _bound_interface
        release_bound_interface(_bound_interface)
        _bound_interface = None
