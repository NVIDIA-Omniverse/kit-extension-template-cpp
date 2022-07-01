// Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.
//
// NVIDIA CORPORATION and its licensors retain all intellectual property
// and proprietary rights in and to this software, related documentation
// and any modifications thereto. Any use, reproduction, disclosure or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA CORPORATION is strictly prohibited.
//
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <CppWidget.h>

OMNIUI_CPPWIDGET_NAMESPACE_OPEN_SCOPE

CppWidget::CppWidget() : Widget{}
{
}

CppWidget::~CppWidget()
{
}

void CppWidget::_drawContent(float elapsedTime)
{
    ImVec2 start = ImGui::GetCursorScreenPos();
    float computedWidth = this->getComputedContentWidth();
    float computedHeight = this->getComputedContentHeight();

    // Draw a rect
    ImVec2 rectMax{ start.x + computedWidth, start.y + computedHeight };

    // We need to scale the thickness to look similar on all the monitors.
    float scaledThickness = this->getThickness() * this->getDpiScale();

    // Determine which color we need from the style. The style should look like
    // this to set red color:
    // {"CppWidget": {"color": ui.color.red}}
    uint32_t color = 0xff000000;
    this->_resolveStyleProperty(StyleColorProperty::eColor, &color);

    ImGui::GetWindowDrawList()->AddRect(start, rectMax, 0xff0000ff, 0.0f, 0, scaledThickness);
}

OMNIUI_CPPWIDGET_NAMESPACE_CLOSE_SCOPE
