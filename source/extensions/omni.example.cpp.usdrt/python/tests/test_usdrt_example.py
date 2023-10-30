## Copyright (c) 2023, NVIDIA CORPORATION.  All rights reserved.
##
## NVIDIA CORPORATION and its licensors retain all intellectual property
## and proprietary rights in and to this software, related documentation
## and any modifications thereto.  Any use, reproduction, disclosure or
## distribution of this software and related documentation without an express
## license agreement from NVIDIA CORPORATION is strictly prohibited.
##
import omni.example.cpp.usdrt
import omni.kit.app
import omni.kit.test
import omni.usd

from usdrt import Sdf

class TestUsdrtExample(omni.kit.test.AsyncTestCase):
    async def setUp(self):
        # Cache the example usdrt interface.
        self.example_usdrt_interface = omni.example.cpp.usdrt.get_example_usdrt_interface()

        # Open a new USD stage.
        omni.usd.get_context().new_stage()
        self.usd_stage = omni.usd.get_context().get_stage()
        self.stage_id = omni.usd.get_context().get_stage_id()

        # Create a torus
        (success, pathString) = omni.kit.commands.execute("CreateMeshPrimWithDefaultXformCommand", prim_type="Torus")
        self.assertTrue(success)
        self.prim_path = Sdf.Path(pathString)
        
    async def tearDown(self):
        # Close the USD stage.
        await omni.usd.get_context().close_stage_async()
        self.usd_stage = None

        # Clear the example usdrt interface.
        self.example_usdrt_interface = None

    async def test_get_attributes_for_prim(self):
       (err, attrs) = self.example_usdrt_interface.get_attributes_for_prim(self.stage_id, self.prim_path)
       self.assertTrue(err == "")
       self.assertTrue(attrs)

       # test invalid prim
       (err, attrs) = self.example_usdrt_interface.get_attributes_for_prim(self.stage_id, Sdf.Path("/invalidPrim"))
       self.assertTrue(err == "Prim at path /invalidPrim is not in Fabric")
       self.assertFalse(attrs)

       # test empty path
       (err, attrs) = self.example_usdrt_interface.get_attributes_for_prim(self.stage_id, Sdf.Path(""))
       self.assertTrue(err == "Nothing selected")
       self.assertFalse(attrs)

    async def test_apply_random_rotation(self):
        (err, rotation) = self.example_usdrt_interface.apply_random_rotation(self.stage_id, self.prim_path)
        self.assertTrue(err == "")
        self.assertTrue(rotation)

        # test invalid prim
        (err, rotation) = self.example_usdrt_interface.apply_random_rotation(self.stage_id, Sdf.Path("/invalidPrim"))
        self.assertTrue(err == "Prim at path /invalidPrim is not in Fabric")

        # test empty path
        (err, rotation) = self.example_usdrt_interface.apply_random_rotation(self.stage_id, Sdf.Path(""))
        self.assertTrue(err == "Nothing selected")


    async def test_deform_mesh(self):
        t = 0
        result = self.example_usdrt_interface.deform_mesh(self.stage_id, self.prim_path, t)
        self.assertTrue(result == f"Deformed points on prim {self.prim_path}")

        # test invalid prim
        result = self.example_usdrt_interface.deform_mesh(self.stage_id, Sdf.Path("/invalidPrim"), t)
        self.assertTrue(result == "Prim at path /invalidPrim is not in Fabric")

        # test empty path
        result =self.example_usdrt_interface.deform_mesh(self.stage_id, Sdf.Path(""), t)
        self.assertTrue(result == "Nothing selected")


