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

# omni.kit.ui_test is for simulating UI interactions in tests.
import omni.kit.ui_test as ui_test

# The Python module we are testing, imported with an absolute
# path to simulate using it from a different Python extension.
import omni.example.python.ui


# Any class that dervives from 'omni.kit.test.AsyncTestCase'
# declared at the root of the module will be auto-discovered,
class Test(omni.kit.test.AsyncTestCase):
    # Called before running each test.
    async def setUp(self):
        pass

    # Called after running each test.
    async def tearDown(self):
        pass

    # Example test case that simulates UI interactions.
    async def test_window_button(self):

        # Find a label in the window.
        label = ui_test.find("Example Window//Frame/**/Label[*]")

        # Find buttons in the window.
        add_button = ui_test.find("Example Window//Frame/**/Button[*].text=='Add'")
        reset_button = ui_test.find("Example Window//Frame/**/Button[*].text=='Reset'")

        # Click the add button.
        await add_button.click()
        self.assertEqual(label.widget.text, "count: 1")

        # Click the add button.
        await add_button.click()
        self.assertEqual(label.widget.text, "count: 2")

        # Click the reset button.
        await reset_button.click()
        self.assertEqual(label.widget.text, "empty")
