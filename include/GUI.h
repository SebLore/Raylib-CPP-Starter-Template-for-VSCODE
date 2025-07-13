/**
 * @file GUI.h
 * @brief GUI base interfaces and core functionality
 * @date 2025-07-12
 * @details This file defines the base interfaces for GUI systems and components
 */

#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <raylib.h>

/**
 * @brief Base interface for GUI components
 * @description This interface defines the core functionality for GUI components, including rendering and input handling.
 */
class IGUIComponent
{
public:
    virtual ~IGUIComponent() = default;

    virtual void Render(Vector2 offset) = 0;
    virtual void HandleInput(Vector2 offset) = 0;

protected:
    Rectangle m_bounds = {0, 0, 0, 0}; // Position and size of the component
    bool m_visible = true;             // Whether the component is visible
    bool m_enabled = true;             // Whether the component is enabled
};

/**
 * @brief Base interface for GUI systems
 * @description This interface defines the core functionality for GUI systems, including initialization, input handling, rendering, and cleanup.
 */
class IGUI
{
public:
    virtual ~IGUI() = default;

    virtual void Init() = 0;        // Initialize GUI components
    virtual void HandleInput() = 0; // Handle user input for GUI
    virtual void Render() = 0;      // Render GUI components
    virtual void Cleanup() = 0;     // Cleanup GUI resources
    virtual void AddComponent(std::unique_ptr<IGUIComponent> component) = 0;
    virtual void RemoveComponent(IGUIComponent *component) = 0;
};
