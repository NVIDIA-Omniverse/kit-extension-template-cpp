// Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.
//
// NVIDIA CORPORATION and its licensors retain all intellectual property
// and proprietary rights in and to this software, related documentation
// and any modifications thereto.  Any use, reproduction, disclosure or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA CORPORATION is strictly prohibited.
//
#include <OgnExampleNodeDatabase.h>

// Helpers to explicit shorten names you know you will use
using omni::graph::core::Type;
using omni::graph::core::BaseDataType;

namespace omni {
namespace example {
namespace cpp {
namespace omnigraph_node {

class OgnExampleNode
{
public:
    static bool compute(OgnExampleNodeDatabase& db)
    {
        // The database has a few useful utilities that simplify common operations such as converting strings to tokens
        static auto pointsToken = db.stringToToken("points");

        // This is how to extract a single-valued attribute. The return values for db.inputs.XXX() are
        // const-ref accessors that provide an interface to the actual data. For simple values it can be
        // considered to be the POD type so this line would be equivalent to:
        //    const bool& disableOffset = db.inputs.disable();
        const auto& disableOffset = db.inputs.disable();

        // This is how to extract an output array attribute. It returns an accessor that behaves basically the same as
        // the std::array type, except that the data is managed by Fabric. It further adds the ability to set the array
        // size explicitly, required to ensure that Fabric has enough space allocated for the attribute data.
        // Since this is an output the reference is no longer a const, enabling modification of the data to which it
        // provides access.
        auto& points = db.outputs.points();

        // This is how to extract an input bundle attribute. It returns an accessor that lets you inspect the bundle
        // contents and the values of the attributes in the bundle.
        const auto& bundleWithGeometry = db.inputs.geometry();
        auto pointsAccessor = bundleWithGeometry.attributeByName(pointsToken);
        // The bundle member accessor has a boolean cast to help determine if an attribute name was found in one step
        if (! pointsAccessor)
        {
            // This is just a warning because technically the node did what it said it would - output the result of
            // offsetting a points attribute in the bundle. No attribute was found, no work was done, as expected.
            db.logWarning("No 'points' attribute found in the bundle. No compute happened.");
            return true;
        }
        // Confirm that the type matches, as well as the name. The "get<>()" function returns an accessor that
        // provides a boolean operatortop confirm type compatibility, and an indirection operator that returns a
        // reference to data of the type that was matched (array of float[3] in this case)
        auto inputPoints = pointsAccessor.get<float[][3]>();
        if (inputPoints)
        {
            db.logWarning(
                "The 'points' attribute was type %s, not float[3][]. No compute happened.",
                pointsAccessor.typeName().c_str()
            );
            return true;
        }

        // Handle the trivial case of modifying an empty point array first as the other values don't matter
        if (inputPoints.size() == 0)
        {
            points.resize(0);
            return true;
        }

        // By default the generated code will cast to the USD data types, however you can use any binary compatible
        // types you have. See the .ogn documentation on type configurations to see how to change types.
        pxr::GfVec3f pointOffset{ 0.0f, 0.0f,  0.0f };

        // This is how to extract a variable-typed input attribute. By default if the type has not been resolved to
        // one of the legal types, in this case float[3] or float, the compute() will not be called. You can
        // add "unvalidated":true to the attribute definition and handle the case of unresolved types here.
        // The return value is an accessor that provides type information and the cast operators to get the
        // resolved data types.
        const auto& offsetValue = db.inputs.offset();
        if (auto floatOffset = offsetValue.get<float>())
        {
            // The data received back from the "get<>()" method is an accessor that provides a boolean operator,
            // which did the type compatibility test that got us into this section of the "if", and an indirection
            // operator that returns a reference to data of the type that was matched (float in this case).
            auto const& floatValue = *floatOffset
            pointOffset[0] = floatValue;
            pointOffset[1] = floatValue;
            pointOffset[2] = floatValue;
            std::cout << "Got a float value of " << floatValue << std::endl;
        }
        // Repeat the same process of checking and applying for the other accepted value type of float[3]
        else if (auto float3Offset = offsetValue.get<float[3]>())
        {
            pointOffset = *float3Offset;
            std::cout << "Got a float[3] value of " << float3Value[0] << ", " << float3Value[1] << ", " << float3Value[2] << std::endl;
        }
        else
        {
            // If the resolved type was not one of the recognized one then something went wrong and the node is
            // incapable of computing so log an error and return false.
            db.logError("Unrecognized offset type %s", offsetValue.typeName().c_str());
            return false;
        }

        // -------------------------------------------------------------------------------------------------------------
        // With only a few accepted data types you can use the cascading "if" cast method above. If you have a lot of
        // types being accepted then you should use a switch statement on the attribute type like this, as it has much
        // better performance.
        // const auto& type = offsetValue.type();
        // switch (type.baseDataType)
        // {
        //     case BaseDataType::eFloat:
        //         if ((type.componentCount == 1) and (type.arrayDepth == 0))
        //         {
        //             return handleFloatValue();
        //         }
        //         if ((type.componentCount == 3) and (type.arrayDepth == 0))
        //         {
        //             return handleFloat3Value();
        //         }
        // }
        // db.logError("Unrecognized offset type %s", offsetValue.typeName().c_str());
        // return false;
        // -------------------------------------------------------------------------------------------------------------

        // Now that all values are accessible the actual computation can happen.
        //
        // Output arrays must always be first resized to be the total number of entries they will eventually
        // contain. Repeated resizing can be expensive so it's best to do it once up front.
        points.resize(inputPoints.size());

        // The array accessor provides the common definitions that allow range-based for loops
        size_t pointIndex{ 0 };
        for (auto const& pointToModify : inputPoints)
        {
            points[pointIndex] = pointToModify + pointOffset;
        }

        // Returning true tells Omnigraph that the compute was successful and the output value is now valid.
        return true;
    }
};

REGISTER_OGN_NODE()

} // omnigraph_node
} // cpp
} // example
} // omni
