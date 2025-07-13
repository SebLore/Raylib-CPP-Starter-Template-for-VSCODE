/**
 * @file Sandbox.h
 * @author Sebastian Lorensson (you@domain.com)
 * @brief just a sandbox to test various parts of the engine
 * @version 0.1
 * @date 2025-07-12
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include <iostream>
#include <format>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <fstream>

#include <raylib.h>
#include <raymath.h>
#include <entt/entt.hpp>

#include "Simulation.h"
#include "Components.h"
#include "Systems.h"
#include "Maths.h"

#include "SidePanel.h"

#include "Tilemap.h"

class PhysicsSystem : public ISystem
{
};

class Sandbox : public ISimulation
{
public:
    Sandbox(int screenWidth = 800, int screenHeight = 600, const char *title = "Sandbox",
            unsigned int flags = FLAG_WINDOW_RESIZABLE, int fps = 60)
        : ISimulation(screenWidth, screenHeight, title, flags, fps)
    {
        Init();
    }

    void Init() override
    {
        // calculate how many tiles it would take to fit the defined area
        // Reserve space for the side panel
        int drawingAreaWidth = m_screenWidth - m_sidePanelWidth;

        m_tilemap.tileSize = 20;
        m_tilemap.tiles.resize((drawingAreaWidth / m_tilemap.tileSize) * (m_screenHeight / m_tilemap.tileSize), ecs::Tile{0}); // Initialize tilemap with empty tiles

        // Initialize the side panel
        m_sidePanel = std::make_unique<SidePanelGUI>(m_screenWidth - m_sidePanelWidth, 0, m_sidePanelWidth, m_screenHeight, LIGHTGRAY);

        
        m_sidePanel->SetOnClearCallback([this]()
                                        { ClearTilemap(); });
        m_sidePanel->SetOnSaveCallback([this]()
                                       { SaveTilemap(); });
        m_sidePanel->SetOnGridToggleCallback([this](bool enabled)
                                             { m_drawGrid = enabled; });
        m_sidePanel->SetOnBrushSizeChanged([this](int size)
                                           { 
            m_brushSize = size; 
            std::cout << "Brush size changed to: " << size << std::endl; });
        m_sidePanel->Init();

        // load serialized tilemap from a file or string
        std::string serializedTilemap = "tilemap\n tileSize 20\n 0 0 0 0 0 0 0 0 0 0 \n";
        // Initialize entities and components here
        std::cout << "Sandbox initialized." << std::endl;
    }

    void HandleInput() override
    {
        // Handle user input here
        if (IsKeyPressed(KEY_ESCAPE))
        {
            CloseWindow(); // Close the window on ESC key press
        }

        if (IsKeyPressed(KEY_G))
        {
            m_drawGrid = !m_drawGrid; // Toggle grid visibility
        }

        // Handle side panel input with proper coordinate transformation
        Vector2 mousePos = GetMousePosition();
        Vector2 panelOffset = {(float)(m_screenWidth - m_sidePanelWidth), 0.0f};

        // Check if mouse is over the side panel area
        if (mousePos.x >= panelOffset.x && mousePos.x < m_screenWidth &&
            mousePos.y >= 0 && mousePos.y < m_screenHeight)
        {
            // Handle side panel interactions manually
            HandleSidePanelInput(mousePos, panelOffset);
        }
        else
        {
            // Only handle tile drawing if mouse is not over the side panel
            int drawingAreaWidth = m_screenWidth - m_sidePanelWidth;

            if (mousePos.x < drawingAreaWidth) // Only draw tiles in the drawing area
            {
                // Draw tiles based on mouse position but don't add them permanently
                int tileIndex = (static_cast<int>(mousePos.y) / m_tilemap.tileSize) * (drawingAreaWidth / m_tilemap.tileSize) +
                                (static_cast<int>(mousePos.x) / m_tilemap.tileSize);
                if (tileIndex < 0 || tileIndex >= static_cast<int>(m_tilemap.tiles.size()))
                {
                    std::cout << "Mouse position out of bounds for tilemap." << std::endl;
                    return; // Prevent out-of-bounds access
                }

                // Use brush size for tile drawing
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                {
                    DrawBrushTiles(mousePos, 1, drawingAreaWidth); // Draw tiles with brush
                }

                // Right click to erase tiles while dragging
                if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
                {
                    DrawBrushTiles(mousePos, 0, drawingAreaWidth); // Erase tiles with brush
                }
            }
        }

        static bool full = false;
        if (IsKeyPressed(KEY_T) || IsKeyDown(KEY_T))
        {
            if (!full)
            {
                // add tile
                bool added = false;
                for (size_t i = 0; i < m_tilemap.tiles.size() && !added; i++)
                {
                    if (m_tilemap.tiles[i].value == 0)
                    {
                        m_tilemap.tiles[i].value = 1;
                        added = true;
                    }
                }
                if (!added)
                    full = true;
            }
            else
            {
                std::cout << "full on tiles!\n";
            }
        }

        // print tilemap
        if (IsKeyPressed(KEY_P))
        {
            std::cout << Tilemap::Serialize(m_tilemap) << std::endl;
        }
    }

    void Update(float deltaTime) override
    {
        // Update simulation state here
        static float last = 0;
        accumulated_time += deltaTime;
        if ((accumulated_time - last) > 1.0f)
        {
            last = accumulated_time;
            std::cout << "Updating sandbox with delta time: " << deltaTime << std::endl;
        }
    }

    void Render() override
    {
        BeginDrawing();
        ClearBackground(RAYWHITE); // Clear the background with white color

        // Draw tiles in the main drawing area
        DrawTiles();

        // Draw grid if enabled
        if (m_drawGrid)
        {
            DrawGrid(m_screenWidth - m_sidePanelWidth, m_screenHeight, m_tilemap.tileSize);
        }

        // Render side panel using the modular GUI system
        if (m_sidePanel)
        {
            // Calculate panel offset and draw the side panel manually at the correct position
            Vector2 panelOffset = {(float)(m_screenWidth - m_sidePanelWidth), 0.0f};

            // Draw the side panel background at the correct position
            DrawRectangle(panelOffset.x, panelOffset.y, m_sidePanelWidth, m_screenHeight, LIGHTGRAY);
            DrawRectangleLinesEx({panelOffset.x, panelOffset.y, (float)m_sidePanelWidth, (float)m_screenHeight}, 2, BLACK);

            // Render the side panel content with offset
            RenderSidePanelWithOffset(panelOffset);
        }

        EndDrawing();
    }

    void DrawTiles()
    {
        int drawingAreaWidth = m_screenWidth - m_sidePanelWidth;
        Tilemap::Draw(m_tilemap, drawingAreaWidth, m_screenHeight); // Draw the tilemap
    }

    void Cleanup() override
    {
        // Cleanup resources here
        if (m_sidePanel)
        {
            m_sidePanel->Cleanup();
        }
        std::cout << "Cleaning up sandbox." << std::endl;
    }

    void Run() override
    {
        Init(); // Initialize the simulation
        while (!WindowShouldClose())
        {
            HandleInput();          // Handle user input
            Update(GetFrameTime()); // Update the simulation state
            Render();               // Render the simulation
        }
        Cleanup(); // Cleanup resources
    }

    void DrawGrid(int screenWidth, int screenHeight, int cellCount)
    {
        // Draw a grid for debugging purposes
        for (int i = 0; i < screenWidth; i += cellCount)
        {
            DrawLine(i, 0, i, screenHeight, LIGHTGRAY);
        }
        for (int j = 0; j < screenHeight; j += cellCount)
        {
            DrawLine(0, j, screenWidth, j, LIGHTGRAY);
        }
    }

    void RenderSidePanelWithOffset(Vector2 offset)
    {
        if (!m_sidePanel)
            return;

        // Render side panel components with manual positioning
        // This is a simplified approach - we'll manually draw the main components

        int yOffset = 20;

        // Draw title
        DrawText("Tile Editor", offset.x + 10, offset.y + yOffset, 24, BLACK);
        yOffset += 40;

        // Draw brush size slider (simplified version)
        Rectangle sliderRect = {offset.x + 10, offset.y + (float)yOffset, (float)(m_sidePanelWidth - 20), 20};
        DrawRectangleRec(sliderRect, WHITE);
        DrawRectangleLinesEx(sliderRect, 2, BLACK);

        // Draw slider handle based on brush size
        float sliderValue = (float)(m_brushSize - 1) / 9.0f; // Normalize to 0-1 range
        float handleX = sliderRect.x + sliderValue * (sliderRect.width - 10);
        DrawRectangle(handleX, sliderRect.y + 2, 10, 16, BLUE);

        // Draw brush size label
        DrawText(TextFormat("Brush Size: %d", m_brushSize), offset.x + 10, offset.y + yOffset - 25, 18, BLACK);
        yOffset += 50;

        // Draw clear button
        Rectangle clearButton = {offset.x + 10, offset.y + (float)yOffset, (float)(m_sidePanelWidth - 20), 30};
        DrawRectangleRec(clearButton, RED);
        DrawRectangleLinesEx(clearButton, 2, BLACK);
        DrawText("Clear All", clearButton.x + 15, clearButton.y + 5, 20, BLACK);
        yOffset += 40;

        // Draw save button
        Rectangle saveButton = {offset.x + 10, offset.y + (float)yOffset, (float)(m_sidePanelWidth - 20), 30};
        DrawRectangleRec(saveButton, GREEN);
        DrawRectangleLinesEx(saveButton, 2, BLACK);
        DrawText("Save", saveButton.x + 25, saveButton.y + 5, 20, BLACK);
        yOffset += 40;

        // Draw grid checkbox
        Rectangle checkboxRect = {offset.x + 10, offset.y + (float)yOffset, 20, 20};
        DrawRectangleRec(checkboxRect, WHITE);
        DrawRectangleLinesEx(checkboxRect, 2, BLACK);
        if (m_drawGrid)
        {
            DrawRectangle(checkboxRect.x + 4, checkboxRect.y + 4, 12, 12, GREEN);
        }
        DrawText("Show Grid", offset.x + 40, offset.y + yOffset + 2, 18, BLACK);
        yOffset += 35;

        // Draw image browser placeholder
        Rectangle imageBrowser = {offset.x + 10, offset.y + (float)yOffset, (float)(m_sidePanelWidth - 20), 150};
        DrawRectangleRec(imageBrowser, LIGHTGRAY);
        DrawRectangleLinesEx(imageBrowser, 2, BLACK);
        DrawText("Image Browser", imageBrowser.x + 20, imageBrowser.y + 70, 16, DARKGRAY);
    }

    void HandleSidePanelInput(Vector2 mousePos, Vector2 panelOffset)
    {
        int yOffset = 20;

        // Skip title
        yOffset += 40;

        // Handle brush size slider
        Rectangle sliderRect = {panelOffset.x + 10, panelOffset.y + (float)yOffset, (float)(m_sidePanelWidth - 20), 20};
        if (CheckCollisionPointRec(mousePos, sliderRect) && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            float relativeX = mousePos.x - sliderRect.x;
            float sliderValue = relativeX / sliderRect.width;
            sliderValue = Clamp(sliderValue, 0.0f, 1.0f);
            m_brushSize = 1 + (int)(sliderValue * 9); // Map to 1-10 range
            std::cout << "Brush size changed to: " << m_brushSize << std::endl;
        }
        yOffset += 50;

        // Handle clear button
        Rectangle clearButton = {panelOffset.x + 10, panelOffset.y + (float)yOffset, (float)(m_sidePanelWidth - 20), 30};
        if (CheckCollisionPointRec(mousePos, clearButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            ClearTilemap();
        }
        yOffset += 40;

        // Handle save button
        Rectangle saveButton = {panelOffset.x + 10, panelOffset.y + (float)yOffset, (float)(m_sidePanelWidth - 20), 30};
        if (CheckCollisionPointRec(mousePos, saveButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            SaveTilemap();
        }
        yOffset += 40;

        // Handle grid checkbox
        Rectangle checkboxRect = {panelOffset.x + 10, panelOffset.y + (float)yOffset, 20, 20};
        if (CheckCollisionPointRec(mousePos, checkboxRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            m_drawGrid = !m_drawGrid;
        }
        yOffset += 35;

        // Image browser interactions could be added here
    }

private:
    bool m_drawGrid = true;                            // Flag to toggle grid drawing
    int m_gridSize = 32;                               // Size of each grid cell
    int tileSize = gcd(m_screenWidth, m_screenHeight); // Calculate tile size based on screen dimensions
    Tilemap m_tilemap;                                 // Tilemap for the sandbox
    int m_brushSize = 1;                               // Current brush size

    // GUI components
    static constexpr int m_sidePanelWidth = 200; // Width of the side panel
    std::unique_ptr<SidePanelGUI> m_sidePanel;   // Side panel GUI

    float accumulated_time = 0.0f;                   // Accumulator for delta time
    std::vector<std::unique_ptr<ISystem>> m_systems; // List of systems in the simulation

    template <typename T, typename... Args>
    void CreateSystem(Args &&...args)
    {
        // Create a system and add it to the list of systems
        m_systems.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
    }

    // Helper functions for GUI callbacks
    void ClearTilemap()
    {
        for (auto &tile : m_tilemap.tiles)
        {
            tile.value = 0;
        }
        std::cout << "Tilemap cleared!" << std::endl;
    }

    void SaveTilemap()
    {
        std::string serialized = Tilemap::Serialize(m_tilemap);
        std::cout << "Saving tilemap:\n"
                  << serialized << std::endl;

        // Here you could write to a file
        // std::ofstream file("tilemap.txt");
        // file << serialized;
        // file.close();
    }

    void DrawBrushTiles(Vector2 mousePos, int tileValue, int drawingAreaWidth)
    {
        int centerTileX = static_cast<int>(mousePos.x / m_tilemap.tileSize);
        int centerTileY = static_cast<int>(mousePos.y / m_tilemap.tileSize);
        int tilesPerRow = drawingAreaWidth / m_tilemap.tileSize;
        int brushRadius = (m_brushSize - 1) / 2;

        // Draw tiles in a square brush pattern
        for (int dy = -brushRadius; dy <= brushRadius; dy++)
        {
            for (int dx = -brushRadius; dx <= brushRadius; dx++)
            {
                int tileX = centerTileX + dx;
                int tileY = centerTileY + dy;
                int index = tileY * tilesPerRow + tileX;

                // Check bounds to ensure we don't go out of screen or tilemap bounds
                if (tileX >= 0 && tileX < tilesPerRow &&
                    tileY >= 0 && tileY < (m_screenHeight / m_tilemap.tileSize) &&
                    index >= 0 && index < static_cast<int>(m_tilemap.tiles.size()))
                {
                    m_tilemap.tiles[index].value = tileValue;
                }
            }
        }
    }
};
