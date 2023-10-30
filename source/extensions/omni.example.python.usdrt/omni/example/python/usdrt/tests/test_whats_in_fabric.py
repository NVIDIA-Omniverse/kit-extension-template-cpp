## Copyright (c) 2022, NVIDIA CORPORATION.  All rights reserved.
##
## NVIDIA CORPORATION and its licensors retain all intellectual property
## and proprietary rights in and to this software, related documentation
## and any modifications thereto.  Any use, reproduction, disclosure or
## distribution of this software and related documentation without an express
## license agreement from NVIDIA CORPORATION is strictly prohibited.
##

import omni.example.python.usdrt

# omni.kit.test is primarily Python's standard unittest module
# with additional wrapping to add suport for async/await tests.
# Please see: https://docs.python.org/3/library/unittest.html
import omni.kit.test

# The Python module we are testing, imported with an absolute
# path to simulate using it from a different Python extension.
import omni.usd
import usdrt


# Any class that dervives from 'omni.kit.test.AsyncTestCase'
# declared at the root of the module will be auto-discovered,
class ExamplePythonUsdrtTest(omni.kit.test.AsyncTestCase):
    async def setUp(self):
        # Open a new USD stage.
        omni.usd.get_context().new_stage()
        self.usd_stage = omni.usd.get_context().get_stage()
        self.stage_id = omni.usd.get_context().get_stage_id()

        # create a torus
        (success, pathString) = omni.kit.commands.execute("CreateMeshPrimWithDefaultXformCommand", prim_type="Torus")
        self.assertTrue(success)
        self.prim_path = pathString

    async def tearDown(self):
        # Close the USD stage.
        await omni.usd.get_context().close_stage_async()
        self.usd_stage = None

    async def test_get_fabric_data_for_prim(self):
        result = omni.example.python.usdrt.get_fabric_data_for_prim(self.stage_id, self.prim_path)
        self.assertTrue("Fabric data for prim at path %s\n\n\n" % self.prim_path in result)
        for attr in ["points", "normals", "primvars:st", "extent"]:
            self.assertTrue(attr in result)

        # test invalid prim
        result = omni.example.python.usdrt.get_fabric_data_for_prim(self.stage_id, "/invalidPrim")
        self.assertTrue(result == "Prim at path /invalidPrim is not in Fabric")

        # test empty path
        result = omni.example.python.usdrt.get_fabric_data_for_prim(self.stage_id, None)
        self.assertTrue(result == "Nothing selected")

    async def test_apply_random_rotation(self):
        result = omni.example.python.usdrt.apply_random_rotation(self.stage_id, self.prim_path)
        self.assertTrue("Set new world orientation on %s to (" % self.prim_path in result)

        # test invalid prim
        result = omni.example.python.usdrt.apply_random_rotation(self.stage_id, "/invalidPrim")
        self.assertTrue(result == "Prim at path /invalidPrim is not in Fabric")

        # test empty path
        result = omni.example.python.usdrt.apply_random_rotation(self.stage_id, None)
        self.assertTrue(result == "Nothing selected")

    async def test_deform_mesh_with_warp(self):
        try:
            import warp

            t = 0
            result = omni.example.python.usdrt.deform_mesh_with_warp(self.stage_id, self.prim_path, t)
            self.assertTrue(result == f"Deformed points on prim {self.prim_path}")

            # test invalid prim
            result = omni.example.python.usdrt.deform_mesh_with_warp(self.stage_id, "/invalidPrim", t)
            self.assertTrue(result == "Prim at path /invalidPrim is not in Fabric")

            # test empty path
            result = omni.example.python.usdrt.deform_mesh_with_warp(self.stage_id, None, t)
            self.assertTrue(result == "Nothing selected")

        except ImportError:
            pass

