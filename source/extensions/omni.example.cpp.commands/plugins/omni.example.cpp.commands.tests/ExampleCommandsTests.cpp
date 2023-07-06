// Copyright (c) 2020-2021, NVIDIA CORPORATION. All rights reserved.
//
// NVIDIA CORPORATION and its licensors retain all intellectual property
// and proprietary rights in and to this software, related documentation
// and any modifications thereto.  Any use, reproduction, disclosure or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA CORPORATION is strictly prohibited.
//

#include <doctest/doctest.h>

#include <carb/BindingsUtils.h>

#include <omni/kit/commands/ICommandBridge.h>

CARB_BINDINGS("omni.example.cpp.commands.tests")

TEST_SUITE("omni.example.cpp.commands.tests")
{
    using namespace omni::kit::commands;

    TEST_CASE("Execute Example C++ Command")
    {
        auto commandBridge = carb::getCachedInterface<ICommandBridge>();
        REQUIRE(commandBridge != nullptr);

        // Execute
        bool result = commandBridge->executeCommand("ExampleCpp");
        CHECK(result);

        // Undo
        result = commandBridge->undoCommand();
        CHECK(result);

        // Redo
        result = commandBridge->redoCommand();
        CHECK(result);

        // Undo
        result = commandBridge->undoCommand();
        CHECK(result);

        // Execute with args
        carb::dictionary::IDictionary* iDictionary = carb::getCachedInterface<carb::dictionary::IDictionary>();
        REQUIRE(iDictionary != nullptr);
        carb::dictionary::Item* kwargs = iDictionary->createItem(nullptr, "", carb::dictionary::ItemType::eDictionary);
        REQUIRE(kwargs != nullptr);
        iDictionary->makeAtPath(kwargs, "x", -9);
        iDictionary->makeAtPath(kwargs, "y", 99);
        result = commandBridge->executeCommand("ExampleCpp", kwargs);
        iDictionary->destroyItem(kwargs);
        CHECK(result);

        // Repeat
        result = commandBridge->repeatCommand();
        CHECK(result);

        // Undo
        result = commandBridge->undoCommand();
        CHECK(result);

        // Undo
        result = commandBridge->undoCommand();
        CHECK(result);

        // Undo (empty command stack)
        result = commandBridge->undoCommand();
        CHECK(!result);
    }
}
