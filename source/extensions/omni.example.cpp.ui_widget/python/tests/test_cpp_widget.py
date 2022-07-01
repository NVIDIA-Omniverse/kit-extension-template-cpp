## Copyright (c) 2022, NVIDIA CORPORATION.  All rights reserved.
##
## NVIDIA CORPORATION and its licensors retain all intellectual property
## and proprietary rights in and to this software, related documentation
## and any modifications thereto.  Any use, reproduction, disclosure or
## distribution of this software and related documentation without an express
## license agreement from NVIDIA CORPORATION is strictly prohibited.
##
__all__ = ["TestCppWidget"]

from omni.example.cpp.ui_widget import CppWidget
from omni.ui.tests.test_base import OmniUiTest
from pathlib import Path
import omni.kit.app
import omni.ui as ui

EXTENSION_PATH = Path(omni.kit.app.get_app().get_extension_manager().get_extension_path_by_module(__name__))
GOLDEN_PATH = EXTENSION_PATH.joinpath("data/golden")


STYLE = {"CppWidget": {"color": ui.color.red}}


class TestCppWidget(OmniUiTest):
    async def test_general(self):
        """Testing general look of CppWidget"""
        window = await self.create_test_window()

        with window.frame:
            CppWidget(thickness=2, style=STYLE)

        await self.finalize_test(golden_img_dir=GOLDEN_PATH, golden_img_name=f"test_general.png")
