/**
 * @file SidePanel.h
 * @brief Side panel GUI implementation
 * @date 2025-07-12
 * @details This file contains the SidePanel class implementation for organizing GUI components
 */

#pragma once

#include <memory>
#include <vector>
#include <functional>

#include <raylib.h>

#include "GUI.h"
#include "GUIComponents.h"

namespace GUIConstants
{
    static constexpr int SIDE_PANEL_WIDTH = 200;  // Default width of the side panel
    static constexpr int SIDE_PANEL_HEIGHT = 600; // Default height of the side panel
}

// a piece of GUI that takes up a portion of the side of the screen.
class SidePanelGUI : public IGUI
{
public:
    SidePanelGUI(int x, int y, int width = GUIConstants::SIDE_PANEL_WIDTH, int height = GUIConstants::SIDE_PANEL_HEIGHT, Color backgroundColor = LIGHTGRAY)
        : m_x(x), m_y(y), m_width(width), m_height(height), m_backgroundColor(backgroundColor)
    {
    }

    void Init() override
    {
        // Initialize side panel components here
        int yOffset = 20;

        // Add a title label
        auto titleLabel = std::make_unique<GUILabel>(
            Rectangle{10, (float)yOffset, (float)(m_width - 20), 30},
            "Tile Editor",
            BLACK,
            24);
        m_components.push_back(std::move(titleLabel));
        yOffset += 40;

        // Add brush size slider
        auto brushSizeSlider = std::make_unique<GUISlider>(
            Rectangle{10, (float)yOffset, (float)(m_width - 20), 20},
            1.0f, 10.0f, 1.0f,
            "Brush Size:");
        brushSizeSlider->SetOnValueChanged(
            [this](float value)
            {
                m_brushSize = static_cast<int>(value);
                if (m_onBrushSizeChanged)
                    m_onBrushSizeChanged(m_brushSize);
            });
        m_components.push_back(std::move(brushSizeSlider));
        yOffset += 50;

        // Add clear button
        auto clearButton = std::make_unique<GUIButton>(
            Rectangle{10, (float)yOffset, (float)(m_width - 20), 30},
            "Clear All",
            RED);
        clearButton->SetOnClick([this]()
                                {
            if (m_onClearCallback) m_onClearCallback(); });
        m_components.push_back(std::move(clearButton));
        yOffset += 40;

        // Add save button
        auto saveButton = std::make_unique<GUIButton>(
            Rectangle{10, (float)yOffset, (float)(m_width - 20), 30},
            "Save",
            GREEN);

        saveButton->SetOnClick([this]()
                               {
            if (m_onSaveCallback) m_onSaveCallback(); });
        m_components.push_back(std::move(saveButton));
        yOffset += 40;

        // Add grid toggle checkbox
        auto gridCheckbox = std::make_unique<GUICheckbox>(
            Rectangle{10, (float)yOffset, (float)(m_width - 20), 25},
            "Show Grid",
            true);
        gridCheckbox->SetOnChanged([this](bool checked)
                                   {
            if (m_onGridToggleCallback) m_onGridToggleCallback(checked); });
        m_components.push_back(std::move(gridCheckbox));
        yOffset += 35;

        // Add image browser for tile textures
        auto imageBrowser = std::make_unique<GUIImageBrowser>(
            Rectangle{10, (float)yOffset, (float)(m_width - 20), 150});
        // Add some example images (you can modify these paths)
        imageBrowser->AddImage("assets/owo.png");
        m_components.push_back(std::move(imageBrowser));
    }

    void SetPosition(int x, int y)
    {
        m_x = x;
        m_y = y;
    }

    void HandleInput() override
    {
        Vector2 offset = {static_cast<float>(m_x), static_cast<float>(m_y)};
        // Handle input for all components
        for (auto &component : m_components)
        {
            component->HandleInput(offset);
        }
    }

    void Render() override
    {
        // render the panel itself
        Vector2 offset = {static_cast<float>(m_x), static_cast<float>(m_y)};

        DrawRectangle(m_x, m_y, m_width, m_height, m_backgroundColor);
        DrawRectangleLinesEx({0, 0, (float)m_width, (float)m_height}, 2, BLACK);

        // render panel components
        for (auto &component : m_components)
        {
            component->Render(offset);
        }
    }

    void Cleanup() override
    {
        // Cleanup resources if needed
        m_components.clear();
    }

    void AddComponent(std::unique_ptr<IGUIComponent> component) override
    {
        m_components.push_back(std::move(component));
    }

    void RemoveComponent(IGUIComponent *component) override
    {
        if (!component)
            return;

        try
        {

            m_components.erase(std::remove_if(m_components.begin(), m_components.end(),
                                              [component](const std::unique_ptr<IGUIComponent> &c)
                                              { return c.get() == component; }),
                               m_components.end());
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error removing GUI component: " << e.what() << std::endl;
        }
    }

    // Callback setters
    void SetOnClearCallback(std::function<void()> callback) { m_onClearCallback = callback; }
    void SetOnSaveCallback(std::function<void()> callback) { m_onSaveCallback = callback; }
    void SetOnGridToggleCallback(std::function<void(bool)> callback) { m_onGridToggleCallback = callback; }
    void SetOnBrushSizeChanged(std::function<void(int)> callback) { m_onBrushSizeChanged = callback; }

private:
    using Components = std::vector<std::unique_ptr<IGUIComponent>>;
    int m_x, m_y, m_width, m_height; // Position and size of the side panel
    Color m_backgroundColor;         // Background color of the side panel
    Components m_components;         // Components in the side panel
    int m_brushSize = 1;             // Current brush size
    enum class BrushType
    {
        Circle,
        Square
    }; // Type of brush to use
    BrushType m_brushType = BrushType::Circle; // Default brush type
    bool folded = false;                       // Whether the side panel is folded or not

    // Callbacks
    std::function<void()> m_onClearCallback;
    std::function<void()> m_onSaveCallback;
    std::function<void(bool)> m_onGridToggleCallback;
    std::function<void(int)> m_onBrushSizeChanged;
};
