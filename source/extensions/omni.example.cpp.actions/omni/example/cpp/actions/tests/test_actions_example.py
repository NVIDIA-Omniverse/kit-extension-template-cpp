## Copyright (c) 2022, NVIDIA CORPORATION.  All rights reserved.
##
## NVIDIA CORPORATION and its licensors retain all intellectual property
## and proprietary rights in and to this software, related documentation
## and any modifications thereto.  Any use, reproduction, disclosure or
## distribution of this software and related documentation without an express
## license agreement from NVIDIA CORPORATION is strictly prohibited.
##
import omni.kit.test

import omni.kit.actions.core


class TestActionsExample(omni.kit.test.AsyncTestCase):
    async def setUp(self):
        self.action_registry = omni.kit.actions.core.get_action_registry()
        self.extension_id = "omni.example.cpp.actions"

    async def tearDown(self):
        self.extension_id = None
        self.action_registry = None

    async def test_find_and_execute_custom_action(self):
        action = self.action_registry.get_action(self.extension_id, "example_custom_action_id")
        self.assertIsNotNone(action)

        result = action.execute()
        self.assertEqual(result, 3) # 3 because this was already executed twice in the C++ tests

        result = action.execute()
        self.assertEqual(result, 4)

    async def test_find_and_execute_lambda_action(self):
        action = self.action_registry.get_action(self.extension_id, "example_lambda_action_id")
        self.assertIsNotNone(action)

        result = action.execute()
        self.assertIsNone(result)
