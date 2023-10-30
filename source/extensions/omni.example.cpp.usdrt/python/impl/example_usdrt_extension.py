## Copyright (c) 2023, NVIDIA CORPORATION.  All rights reserved.
##
## NVIDIA CORPORATION and its licensors retain all intellectual property
## and proprietary rights in and to this software, related documentation
## and any modifications thereto.  Any use, reproduction, disclosure or
## distribution of this software and related documentation without an express
## license agreement from NVIDIA CORPORATION is strictly prohibited.
##
import math
import random
from ctypes import alignment

import omni.ext
import omni.ui as ui
import omni.usd
from usdrt import Gf, Rt, Sdf, Usd, Vt

from .._example_usdrt_bindings import *

# Global public interface object.
_example_usdrt_interface = None

# Public API.
def get_example_usdrt_interface() -> IExampleUsdrtInterface:
    return _example_usdrt_interface

def get_selected_prim_path():
    """Return the path of the first selected prim"""
    context = omni.usd.get_context()
    selection = context.get_selection()
    paths = selection.get_selected_prim_paths()

    return None if not paths else paths[0]

def get_stage_id():
    """Return the stage Id of the current stage"""
    context = omni.usd.get_context()
    return context.get_stage_id()


# Any class derived from `omni.ext.IExt` in top level module (defined in `python.modules` of `extension.toml`) will be
# instantiated when extension gets enabled and `on_startup(ext_id)` will be called. Later when extension gets disabled
# on_shutdown() is called.
class ExampleUsdrtExtension(omni.ext.IExt):
    # ext_id is current extension id. It can be used with extension manager to query additional information, like where
    # this extension is located on filesystem.
    def on_startup(self, ext_id):
        # Acquire the example USDRT interface.
        global _example_usdrt_interface
        _example_usdrt_interface = acquire_example_usdrt_interface()

        print("[omni.example.cpp.usdrt] startup")

        self._window = ui.Window(
            "What's in Fabric?", width=300, height=300, dockPreference=ui.DockPreference.RIGHT_BOTTOM
        )
        self._t = 0
        with self._window.frame:
            with ui.VStack():
                frame = ui.ScrollingFrame()
                with frame:
                    label = ui.Label("Select a prim and push a button", alignment=ui.Alignment.LEFT_TOP)

                def get_fabric_data():
                    selected_prim_path = get_selected_prim_path()
                    (err, data) = _example_usdrt_interface.get_attributes_for_prim(
                        get_stage_id(), selected_prim_path
                    )
                    if err:
                        label.text = err
                    else:
                        result = f"Fabric data for prim at path {selected_prim_path}\n\n\n"
                        for attr in data:
                            try:
                                data = attr.Get()
                                datastr = str(data)
                                if data is None:
                                    datastr = "<no value>"

                            except TypeError:
                                # Some data types not yet supported in Python
                                datastr = "<no Python conversion>"

                            result += "{} ({}): {}\n".format(
                                attr.GetName(), str(attr.GetTypeName().GetAsToken()), datastr
                            )
                        label.text = result

                def rotate_prim():
                    selected_prim_path = get_selected_prim_path()
                    (err, rotation) = _example_usdrt_interface.apply_random_rotation(
                        get_stage_id(), selected_prim_path
                    )
                    label.text = err if err else f"Set new world orientation on {selected_prim_path} to {rotation}"

                def deform_prim():
                    label.text = _example_usdrt_interface.deform_mesh(
                        get_stage_id(), get_selected_prim_path(), self._t
                    )
                    self._t += 1

                ui.Button("What's in Fabric?", clicked_fn=get_fabric_data, height=0)
                ui.Button("Rotate it in Fabric!", clicked_fn=rotate_prim, height=0)
                ui.Button("Deform it!", clicked_fn=deform_prim, height=0)

    def on_shutdown(self):
        global _example_usdrt_interface

        # Release the example USDRT interface.
        release_example_usdrt_interface(_example_usdrt_interface)
        _example_usdrt_interface = None

        print("[omni.example.cpp.usdrt] shutdown")
