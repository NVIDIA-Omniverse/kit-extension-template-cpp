# Overview

An example C++ extension that can be used as a reference/template for creating new extensions.

Demonstrates how to create a C++ object that will startup / shutdown along with the extension.


# C++ Usage Examples


## Defining Extensions


```
// When this extension is enabled, any class that derives from omni.ext.IExt
// will be instantiated and 'onStartup(extId)' called. When the extension is
// later disabled, a matching 'onShutdown()' call will be made on the object.
class ExampleCppHelloWorldExtension : public omni::ext::IExt
{
public:
    void onStartup(const char* extId) override
    {
        printf("ExampleCppHelloWorldExtension starting up (ext_id: %s).\n", extId);

        if (omni::kit::IApp* app = carb::getFramework()->acquireInterface<omni::kit::IApp>())
        {
            // Subscribe to update events.
            m_updateEventsSubscription =
                carb::events::createSubscriptionToPop(app->getUpdateEventStream(), [this](carb::events::IEvent*) {
                onUpdate();
            });
        }
    }

    void onShutdown() override
    {
        printf("ExampleCppHelloWorldExtension shutting down.\n");

        // Unsubscribe from update events.
        m_updateEventsSubscription = nullptr;
    }

    void onUpdate()
    {
        if (m_updateCounter % 1000 == 0)
        {
            printf("Hello from the omni.example.cpp.hello_world extension! %d updates counted.\n", m_updateCounter);
        }
        m_updateCounter++;
    }

private:
    carb::events::ISubscriptionPtr m_updateEventsSubscription;
    int m_updateCounter = 0;
};
```

