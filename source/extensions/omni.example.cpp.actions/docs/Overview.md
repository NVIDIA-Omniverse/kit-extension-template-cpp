# Overview

An example C++ extension that can be used as a reference/template for creating new extensions.

Demonstrates how to create actions in C++ that can then be executed from either C++ or Python.

See the omni.kit.actions.core extension for extensive documentation about actions themselves.


# C++ Usage Examples


## Defining Custom Actions


```
using namespace omni::kit::actions::core;

class ExampleCustomAction : public Action
{
public:
    static carb::ObjectPtr<IAction> create(const char* extensionId,
                                           const char* actionId,
                                           const MetaData* metaData)
    {
        return carb::stealObject<IAction>(new ExampleCustomAction(extensionId, actionId, metaData));
    }

    ExampleCustomAction(const char* extensionId, const char* actionId, const MetaData* metaData)
        : Action(extensionId, actionId, metaData), m_executionCount(0)
    {
    }

    carb::variant::Variant execute(const carb::variant::Variant& args = {},
                                   const carb::dictionary::Item* kwargs = nullptr) override
    {
        ++m_executionCount;
        printf("Executing %s (execution count = %d).\n", getActionId(), m_executionCount);
        return carb::variant::Variant(m_executionCount);
    }

    void invalidate() override
    {
        resetExecutionCount();
    }

    uint32_t getExecutionCount() const
    {
        return m_executionCount;
    }

protected:
    void resetExecutionCount()
    {
        m_executionCount = 0;
    }

private:
    uint32_t m_executionCount = 0;
};
```


## Creating and Registering Custom Actions

```
// Example of creating and registering a custom action from C++.
Action::MetaData metaData;
metaData.displayName = "Example Custom Action Display Name";
metaData.description = "Example Custom Action Description.";
carb::ObjectPtr<IAction> exampleCustomAction =
    ExampleCustomAction::create("omni.example.cpp.actions", "example_custom_action_id", &metaData);
carb::getCachedInterface<omni::kit::actions::core::IActionRegistry>()->registerAction(exampleCustomAction);
```


## Creating and Registering Lambda Actions


```
auto actionRegistry = carb::getCachedInterface<omni::kit::actions::core::IActionRegistry>();

// Example of creating and registering a lambda action from C++.
omni::kit::actions::core::IAction::MetaData metaData;
metaData.displayName = "Example Lambda Action Display Name";
metaData.description = "Example Lambda Action Description.";
carb::ObjectPtr<IAction> exampleLambdaAction =
    omni::kit::actions::core::LambdaAction::create(
        "omni.example.cpp.actions", "example_lambda_action_id", &metaData,
        [this](const carb::variant::Variant& args = {}, const carb::dictionary::Item* kwargs = nullptr) {
            printf("Executing example_lambda_action_id.\n");
            return carb::variant::Variant();
        }));
carb::getCachedInterface<omni::kit::actions::core::IActionRegistry>()->registerAction(exampleLambdaAction);
```


```
// Example of creating and registering (at the same time) a lambda action from C++.
carb::getCachedInterface<omni::kit::actions::core::IActionRegistry>()->registerAction(
    "omni.example.cpp.actions", "example_lambda_action_id",
    [](const carb::variant::Variant& args = {}, const carb::dictionary::Item* kwargs = nullptr) {
        printf("Executing example_lambda_action_id.\n");
        return carb::variant::Variant();
    },
    "Example Lambda Action Display Name",
    "Example Lambda Action Description.");
```


## Discovering Actions

```
auto registry = carb::getCachedInterface<omni::kit::actions::core::IActionRegistry>();

// Retrieve an action that has been registered using the registering extension id and the action id.
carb::ObjectPtr<IAction> action = registry->getAction("omni.example.cpp.actions", "example_custom_action_id");

// Retrieve all actions that have been registered by a specific extension id.
std::vector<carb::ObjectPtr<IAction>> actions = registry->getAllActionsForExtension("example");

// Retrieve all actions that have been registered by any extension.
std::vector<carb::ObjectPtr<IAction>> actions = registry->getAllActions();
```


## Deregistering Actions

```
auto actionRegistry = carb::getCachedInterface<omni::kit::actions::core::IActionRegistry>();

// Deregister an action directly...
actionRegistry->deregisterAction(exampleCustomAction);

// or using the registering extension id and the action id...
actionRegistry->deregisterAction("omni.example.cpp.actions", "example_custom_action_id");

// or deregister all actions that were registered by an extension.
actionRegistry->deregisterAllActionsForExtension("omni.example.cpp.actions");
```


## Executing Actions

```
auto actionRegistry = carb::getCachedInterface<omni::kit::actions::core::IActionRegistry>();

// Execute an action after retrieving it from the action registry.
auto action = actionRegistry->getAction("omni.example.cpp.actions", "example_custom_action_id");
action->execute();

// Execute an action indirectly (retrieves it internally).
actionRegistry->executeAction("omni.example.cpp.actions", "example_custom_action_id");

// Execute an action that was stored previously.
exampleCustomAction->execute();
```


Note: All of the above will find any actions that have been registered from either Python or C++,
and you can interact with them without needing to know anything about where they were registered.

