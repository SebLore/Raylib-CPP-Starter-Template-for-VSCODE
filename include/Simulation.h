#pragma once

extern "C"
{
#include <raylib.h>
}
#include <entt/entt.hpp>

class ISimulation
{
public:
    ISimulation(int screenWidth = 800, int screenHeight = 600, const char *title = "Simulation",
                unsigned int flags = FLAG_WINDOW_RESIZABLE, int targetFPS = 60)
        : m_screenWidth(screenWidth), m_screenHeight(screenHeight)
    {
        SetConfigFlags(flags);                            // Set window configuration flags
        InitWindow(m_screenWidth, m_screenHeight, title); // Initialize the window with the
        SetTargetFPS(targetFPS);                          // Set the target frames per second
        if (!IsWindowReady())
        {
            fprintf(stderr, "Failed to create window: %s\n", title);
            exit(EXIT_FAILURE); // Exit if window creation fails
        }
    }
    virtual ~ISimulation()
    {
        CloseWindow(); // Close the window when the simulation is destroyed
    }

    virtual void Init() = 0;
    virtual void HandleInput() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Render() = 0;
    virtual void Cleanup() = 0;
    virtual void Run()
    {

        Init();
        while (!WindowShouldClose())
        {
            HandleInput();
            Update(GetFrameTime());
            Render();
        }
        Cleanup();
    }

protected:
    entt::registry m_registry; // Entity registry for the simulation
    int m_screenWidth = 800;   // Default screen width
    int m_screenHeight = 600;  // Default screen height
};
