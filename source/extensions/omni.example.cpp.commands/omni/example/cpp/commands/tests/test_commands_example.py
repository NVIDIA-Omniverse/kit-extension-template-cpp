## Copyright (c) 2022, NVIDIA CORPORATION.  All rights reserved.
##
## NVIDIA CORPORATION and its licensors retain all intellectual property
## and proprietary rights in and to this software, related documentation
## and any modifications thereto.  Any use, reproduction, disclosure or
## distribution of this software and related documentation without an express
## license agreement from NVIDIA CORPORATION is strictly prohibited.
##
import omni.kit.test

import omni.kit.commands
import omni.kit.undo


class TestCommandsExample(omni.kit.test.AsyncTestCase):
    async def setUp(self):
        omni.kit.undo.clear_stack()

    async def tearDown(self):
        omni.kit.undo.clear_stack()

    async def test_cpp_commands(self):
        # Execute
        res = omni.kit.commands.execute("ExampleCpp")
        self.assertEqual(res, (True, None))

        # Undo
        res = omni.kit.undo.undo()
        self.assertTrue(res)

        # Redo
        res = omni.kit.undo.redo()
        self.assertTrue(res)

        # Repeat
        res = omni.kit.undo.repeat()
        self.assertTrue(res)

        # Undo
        res = omni.kit.undo.undo()
        self.assertTrue(res)

        # Undo
        res = omni.kit.undo.undo()
        self.assertTrue(res)

        # Undo (empty command stack)
        res = omni.kit.undo.undo()
        self.assertFalse(res)
