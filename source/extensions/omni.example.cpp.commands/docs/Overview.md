# Overview

An example C++ extension that can be used as a reference/template for creating new extensions.

Demonstrates how to create commands in C++ that can then be executed from either C++ or Python.

See the omni.kit.commands extension for extensive documentation about commands themselves.


# C++ Usage Examples


## Defining Commands

```
using namespace omni::kit::commands;

class ExampleCppCommand : public Command
{
public:
    static carb::ObjectPtr<ICommand> create(const char* extensionId,
                                            const char* commandName,
                                            const carb::dictionary::Item* kwargs)
    {
        return carb::stealObject<ICommand>(new ExampleCppCommand(extensionId, commandName, kwargs));
    }

    static void populateKeywordArgs(carb::dictionary::Item* defaultKwargs,
                                    carb::dictionary::Item* optionalKwargs,
                                    carb::dictionary::Item* requiredKwargs)
    {
        if (carb::dictionary::IDictionary* iDictionary = carb::getCachedInterface<carb::dictionary::IDictionary>())
        {
            iDictionary->makeAtPath(defaultKwargs, "x", 9);
            iDictionary->makeAtPath(defaultKwargs, "y", -1);
        }
    }

    ExampleCppCommand(const char* extensionId, const char* commandName, const carb::dictionary::Item* kwargs)
        : Command(extensionId, commandName)
    {
        if (carb::dictionary::IDictionary* iDictionary = carb::getCachedInterface<carb::dictionary::IDictionary>())
        {
            m_x = iDictionary->get<int32_t>(kwargs, "x");
            m_y = iDictionary->get<int32_t>(kwargs, "y");
        }
    }

    void doCommand() override
    {
        printf("Executing command '%s' with params 'x=%d' and 'y=%d'.\n", getName(), m_x, m_y);
    }

    void undoCommand() override
    {
        printf("Undoing command '%s' with params 'x=%d' and 'y=%d'.\n", getName(), m_x, m_y);
    }

private:
    int32_t m_x = 0;
    int32_t m_y = 0;
};
```


## Registering Commands

```
auto commandBridge = carb::getCachedInterface<omni::kit::commands::ICommandBridge>());
commandBridge->registerCommand(
    "omni.example.cpp.commands", "ExampleCppCommand", ExampleCppCommand::create, ExampleCppCommand::populateKeywordArgs);
// Note that the command name (in this case "ExampleCppCommand") is arbitrary and does not need to match the C++ class
```


## Executing Commands

```
auto commandBridge = carb::getCachedInterface<omni::kit::commands::ICommandBridge>());

// Create the kwargs dictionary.
auto iDictionary = carb::getCachedInterface<carb::dictionary::IDictionary>();
carb::dictionary::Item* kwargs = iDictionary->createItem(nullptr, "", carb::dictionary::ItemType::eDictionary);
iDictionary->makeIntAtPath(kwargs, "x", 7);
iDictionary->makeIntAtPath(kwargs, "y", 9);

// Execute the command using its name...
commandBridge->executeCommand("ExampleCppCommand", kwargs);
// or without the 'Command' postfix just like Python commands...
commandBridge->executeCommand("ExampleCpp", kwargs);
// or fully qualified if needed to disambiguate (works with or without the 'Command)' postfix.
commandBridge->executeCommand("omni.example.cpp.commands", "ExampleCppCommand", kwargs);

// Destroy the kwargs dictionary.
iDictionary->destroyItem(kwargs);

// The C++ command can be executed from Python exactly like any Python command,
// and we can also execute Python commands from C++ in the same ways as above:
commandBridge->executeCommand("SomePythonCommand", kwargs);
// etc.
```


## Undo/Redo/Repeat Commands

```
auto commandBridge = carb::getCachedInterface<omni::kit::commands::ICommandBridge>());

// It doesn't matter whether the command stack contains Python commands, C++ commands,
// or a mix of both, and the same stands for when undoing/redoing commands from Python.
commandBridge->undoCommand();
commandBridge->redoCommand();
commandBridge->repeatCommand();
```


## Deregistering Commands

```
auto commandBridge = carb::getCachedInterface<omni::kit::commands::ICommandBridge>());
commandBridge->deregisterCommand("omni.example.cpp.commands", "ExampleCppCommand");
```

