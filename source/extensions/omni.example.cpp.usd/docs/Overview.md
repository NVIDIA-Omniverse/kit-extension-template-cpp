# Overview

An example C++ extension that can be used as a reference/template for creating new extensions.

Demonstrates how to create a C++ plugin that can modify the current USD stage.

It is important that all USD stage reads/writes happen from the main thread:

https ://graphics.pixar.com/usd/release/api/_usd__page__multi_threading.html

