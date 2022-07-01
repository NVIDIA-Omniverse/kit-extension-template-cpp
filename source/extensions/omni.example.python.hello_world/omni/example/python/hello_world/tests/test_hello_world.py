## Copyright (c) 2022, NVIDIA CORPORATION.  All rights reserved.
##
## NVIDIA CORPORATION and its licensors retain all intellectual property
## and proprietary rights in and to this software, related documentation
## and any modifications thereto.  Any use, reproduction, disclosure or
## distribution of this software and related documentation without an express
## license agreement from NVIDIA CORPORATION is strictly prohibited.
##

# omni.kit.test is primarily Python's standard unittest module
# with additional wrapping to add suport for async/await tests.
# Please see: https://docs.python.org/3/library/unittest.html
import omni.kit.test

# The Python module we are testing, imported with an absolute
# path to simulate using it from a different Python extension.
import omni.example.python.hello_world


# Any class that dervives from 'omni.kit.test.AsyncTestCase'
# declared at the root of the module will be auto-discovered,
class ExamplePythonHelloWorldTest(omni.kit.test.AsyncTestCase):
    # Called before running each test.
    async def setUp(self):
        pass

    # Called after running each test.
    async def tearDown(self):
        pass

    # Example test case (notice it is an 'async' function, so 'await' can be used if needed).
    async def test_hello_from(self):
        result = omni.example.python.hello_world.hello_from("test_hello_world")
        self.assertEqual(result, "Hello back from omni.example.python.hello_world!")

    # Example test case (notice it is an 'async' function, so 'await' can be used if needed).
    async def test_hello_squared(self):
        result = omni.example.python.hello_world.hello_squared(4)
        self.assertEqual(result, 256)
