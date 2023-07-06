// Copyright (c) 2020-2021, NVIDIA CORPORATION. All rights reserved.
//
// NVIDIA CORPORATION and its licensors retain all intellectual property
// and proprietary rights in and to this software, related documentation
// and any modifications thereto.  Any use, reproduction, disclosure or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA CORPORATION is strictly prohibited.
//

#include <omni/kit/actions/core/IActionRegistry.h>

#include <doctest/doctest.h>

#include <carb/BindingsUtils.h>

CARB_BINDINGS("omni.example.cpp.actions.tests")

TEST_SUITE("omni.example.cpp.actions.tests")
{
    using namespace omni::kit::actions::core;

    TEST_CASE("Execute Example C++ Action")
    {
        auto actionRegistry = carb::getCachedInterface<IActionRegistry>();
        REQUIRE(actionRegistry != nullptr);

        carb::ObjectPtr<IAction> action;
        SUBCASE("Custom")
        {
            action = actionRegistry->getAction("omni.example.cpp.actions", "example_custom_action_id");
            REQUIRE(action.get() != nullptr);

            auto result = action->execute();
            CHECK(result.hasValue());
            CHECK(result.getValue<uint32_t>() == 1);

            result = action->execute();
            CHECK(result.hasValue());
            CHECK(result.getValue<uint32_t>() == 2);
        }

        SUBCASE("Lambda")
        {
            action = actionRegistry->getAction("omni.example.cpp.actions", "example_lambda_action_id");
            REQUIRE(action.get() != nullptr);

            auto result = action->execute();
            CHECK(!result.hasValue());
        }
    }
}
