## Copyright (c) 2022, NVIDIA CORPORATION.  All rights reserved.
##
## NVIDIA CORPORATION and its licensors retain all intellectual property
## and proprietary rights in and to this software, related documentation
## and any modifications thereto.  Any use, reproduction, disclosure or
## distribution of this software and related documentation without an express
## license agreement from NVIDIA CORPORATION is strictly prohibited.
##
import omni.ext
import omni.ui as ui
from omni.example.cpp.ui_widget import CppWidget


class ExamplePythonUIExtension(omni.ext.IExt):
    def on_startup(self, ext_id):
        print(f"ExamplePythonUIExtension starting up (ext_id: {ext_id}).")

        self._count = 0

        self._window = ui.Window("Example Window", width=300, height=300)
        with self._window.frame:
            with ui.VStack():
                label = ui.Label("")

                def on_click():
                    self._count += 1
                    label.text = f"count: {self._count}"

                def on_reset():
                    self._count = 0
                    label.text = "empty"

                on_reset()

                with ui.HStack():
                    ui.Button("Add", clicked_fn=on_click)
                    ui.Button("Reset", clicked_fn=on_reset)

                # Use a widget that was defined in C++
                STYLE = {"CppWidget": {"color": ui.color.red}}
                CppWidget(thickness=2, style=STYLE)

    def on_shutdown(self):
        print(f"ExamplePythonUIExtension shutting down.")
        self._count = 0
