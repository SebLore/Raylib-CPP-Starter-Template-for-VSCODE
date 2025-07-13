#pragma once

#include "Simulation.h"
#include "Components.h"
#include "Systems.h"


struct SimulationConfig
{
    bool pauseOnStart = true;       // Pause the simulation on start
    bool pauseOnCollision = true;   // Pause the simulation on collision
    bool showDebugInfo = true;      // Show debug information
    bool showMenu = true;           // Show the menu
    bool showGrid = true;           // Show the grid
    bool showFPS = true;            // Show FPS counter
    bool showCollisionBoxes = true; // Show collision boxes
};

// Game specific components
namespace ecs
{
    struct Droppable
    {
        bool dropped = false;
    };
}

class CollisionSystem : public ISystem
{
public:
    bool OnUpdate(entt::registry &registry, float) override
    {
        // lazy collision detection system
        // just checking droppable entities against collidable rectangles
        auto droppables = registry.view<ecs::Droppable, ::Rectangle, ecs::RigidBody, ecs::Collidable>();
        auto collidables = registry.view<ecs::Collidable, ::Rectangle, ecs::RigidBody>(entt::exclude<ecs::Droppable>);

        droppables.each([&](entt::entity droppableEntity, ecs::Droppable &droppable, Rectangle &droppableRect, ecs::RigidBody &droppableBody, ecs::Collidable &droppableCollidable)
                        {

            collidables.each([&](ecs::Collidable &collidable, Rectangle &collidableRect, ecs::RigidBody &collidableBody)
            {
                if (CheckCollisionRecs(droppableRect, collidableRect))
                {
                    collidable.isColliding = true;
                    droppableBody.velocity.y = 0.0f; // Reset vertical velocity on collision
                    droppableBody.velocity.x = collidableBody.velocity.x; // Match horizontal velocity of the collidable
                }
            });

            if(droppableCollidable.isColliding)
            {
                registry.emplace_or_replace<ecs::Grounded>(droppableEntity, ecs::Grounded{}); // Mark as grounded
            }
            else
            {
                registry.remove<ecs::Grounded>(droppableEntity); // Remove grounded status if not colliding
            } });

        return true;
    }
};

class PhysicsSystem : public ISystem
{
public:
    bool OnUpdate(entt::registry &registry, float deltaTime) override
    {
        const auto &gravity = registry.ctx().get<ecs::Gravity>(); // Get the gravity value from the registry
        float gravityValue = gravity.value;                       // Access the gravity value

        auto view = registry.view<ecs::RigidBody, Rectangle>(entt::exclude<ecs::Grounded>);
        view.each([&](ecs::RigidBody &body, Rectangle &rec)
                  {
                      body.velocity.y += gravityValue * deltaTime; // Apply gravity to the vertical velocity
                  });                                              // Apply gravity to all rigid bodies

        return true; // Indicate that the system has updated
    }
};

/// @brief Registers components to an entity in the registry.
/// @param e The entity to register components to.
/// @param registry The entity registry to register components in.
/// @param Components The component types to register
/// @tparam Components The component types to register.
template <typename... Components>
inline void RegisterComponents(entt::entity e, entt::registry &registry, Components &&...components)
{
    (registry.emplace<Components>(e, std::forward<Components>(components)), ...);
    // This uses fold expression to apply emplace for each component type
}

class GravityGame final : public ISimulation
{
public:
    GravityGame(int screenWidth = 800, int screenHeight = 600, const char *title = "Simulation",
                unsigned int flags = FLAG_WINDOW_RESIZABLE, int fps = 60)
        : ISimulation(screenWidth, screenHeight, title, flags, fps), m_isPaused(true), m_boxDropped(false)
    {
    }
    ~GravityGame() = default;

    /// @brief initializes entities with components
    /// TODO: use a factory method to create entities with components instead of manually registering components
    void Init() override
    {
        using namespace ecs;

        // setup the simulation context
        m_registry.ctx().emplace<Gravity>(Gravity{9.81f}); // Initialize gravity context

        // create a box entity to drop
        entt::entity box = m_registry.create();
        RegisterComponents<Rectangle, Droppable, RigidBody, Collidable, Grounded, MouseInteractible>(
            box, m_registry,
            Rectangle{(float)m_horizontalOffset, (float)m_initialAltitude, (float)m_boxWidth, (float)m_boxHeight},
            Droppable{}, RigidBody{}, Collidable{}, Grounded{}, MouseInteractible{});

        // create platform for box to land on
        entt::entity platform = m_registry.create();
        RegisterComponents<Rectangle, RigidBody, Collidable, MouseInteractible>(
            platform, m_registry,
            Rectangle{0, (float)(m_screenHeight - m_boxHeight), (float)m_platformWidth, (float)m_boxHeight},
            RigidBody{}, Collidable{}, MouseInteractible{});

        entt::entity text = m_registry.create();
        m_registry.emplace<ecs::Text>(text, ecs::Text{"Press SPACE to drop the box", Vector2{10, 10}, 20, BLACK});

        // create text drawing system
        CreateSystem<PhysicsSystem>();
        CreateSystem<CollisionSystem>();
        CreateSystem<TextInterface>();
    }

    /// @brief handles user input
    /// @details Pressing SPACE drops the box, and pressing G toggles the grid visibility.
    void HandleInput() override
    {
        if (IsKeyPressed(KEY_SPACE))
        {
            auto boxView = m_registry.view<ecs::Droppable, ecs::RigidBody, ecs::Grounded>();
            bool wasDropped = false;
            std::vector<entt::entity> removeEntities; // To store entities to be removed

            // check if a the a droppable box with grounded component exists
            boxView.each([&](entt::entity e, ecs::Droppable &droppable, ecs::RigidBody &body)
                         {
                             if (!droppable.dropped) // Only drop if not already dropped
                             {
                                 droppable.dropped = true; // Mark as dropped
                                 body.velocity.y = 9.0f; // Apply an initial upward velocity
                                 wasDropped = true; // Set dropped flag to true
                                 removeEntities.push_back(e); // Add entity to removal list
                             }
                             else
                                wasDropped = true; });

            // Remove grounded component of dropped entit(y|ies)
            for (const auto &e : removeEntities)
            {
                m_registry.remove<ecs::Grounded>(e); // Remove grounded component from the entity
            }

            if (wasDropped)
            {
                auto platformView = m_registry.view<ecs::RigidBody>(entt::exclude<ecs::Droppable>);
                platformView.each([&](ecs::RigidBody &body)
                                  {
                                      body.velocity.x = 2.0f; // Reset horizontal velocity of the platform
                                  });                         // Reset horizontal velocity of the platform
                // move platforms
            }
        }

        if (IsKeyPressed(KEY_G))
        {
            m_drawGrid = !m_drawGrid; // Toggle grid visibility
        }

        if (IsKeyPressed(KEY_RIGHT))
        {
            m_gridSize += 5;
            if (m_gridSize >= m_screenWidth * 0.25f)
            {
                m_gridSize = m_screenHeight * 0.25f; // cap grid size to 1/4 of screen width
            }
        }
        else if (IsKeyPressed(KEY_LEFT))
        {
            m_gridSize -= 5;
            if (m_gridSize <= 5)
            {
                m_gridSize = 5; // minimum grid size
            }
        }

        // // Handle mouse interactions with
        // auto rects = m_registry.view<::Rectangle, ecs::MouseInteractible, ecs::Drawable>();
        // rects.each([&](entt::entity e, ::Rectangle &rec, ecs::MouseInteractible &mouseI, ecs::Drawable &drawable)
        //            {
        //             ::Vector2 mousePos = GetMousePosition();
        //             auto mouseClicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

        //             if (CheckCollisionPointRec(mousePos, rec))
        //             {
        //                 mouseClicked ? mouseI.wasClicked = true : mouseI.wasClicked = false; // Set wasClicked flag based on mouse button state
        //                 mouseI.hovered = true; // Set hovered flag since mouse is over the rectangle

        //                 if (mouseI.wasClicked)
        //                 {
        //                     mouseI.selected = !mouseI.selected; // Toggle selection on click
        //                     drawable.tint = mouseI.selected ? YELLOW : WHITE; // Change color based on selection
        //                 }
        //                 else
        //                 {
        //                     drawable.tint = drawable.defaultTint; // Maintain color if not clicked
        //                 }
        //             }
        //             else
        //             {
        //                 mouseI.hovered = false; // Reset hovered flag if not colliding
        //             } });
    }

    void Update(float deltaTime) override
    {
        // // Update positions of all Rectangle entities based on RigidBody velocity
        // auto bodies = m_registry.view<Rectangle, ecs::RigidBody>(); // Exclude grounded entities to prevent them from moving
        // bodies.each([&](Rectangle &rec, const ecs::RigidBody &body)
        //             {
        //                 rec.y += body.velocity.y * m_pixelsPerMeter * deltaTime; // Update vertical position based on velocity
        //                 rec.x += body.velocity.x * m_pixelsPerMeter * deltaTime; // Update horizontal position based on velocity
        //             });

        for (auto &system : m_systems)
        {
            system->OnUpdate(m_registry, deltaTime); // Update each system in the scene
        }
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

    void Render() override
    {
        BeginDrawing();
        ClearBackground(SKYBLUE);

        Update(GetFrameTime()); // Update the simulation state

        if (m_drawGrid)
        {
            DrawGrid(m_screenWidth, m_screenHeight, m_gridSize); // Draw grid if enabled
        }

        // Draw all drawable rectangle entities

        auto rectangles = m_registry.view<::Rectangle, ecs::Drawable, ecs::MouseInteractible>();
        rectangles.each([](const ::Rectangle &rec, const ecs::Drawable &drawable, const ecs::MouseInteractible &selected)
                        { 
                        DrawRectangle((int)rec.x, (int)rec.y, (int)rec.width, (int)rec.height, drawable.tint); 

                        // if selected, draw a border
                        if (selected.selected)
                        {
                            ::DrawRectangleLines((int)rec.x, (int)rec.y, (int)rec.width, (int)rec.height, BLACK);
                        } });

        auto text = m_registry.view<ecs::Text, ecs::Drawable>();
        text.each([](const ecs::Text &text, const ecs::Drawable &drawable)
                  { DrawText(text.content.c_str(), (int)text.position.x, (int)text.position.y, text.fontSize, drawable.tint); });

        EndDrawing();
    }

    void Cleanup() override
    {
        // Cleanup resources if needed
        m_registry.clear(); // Clear the registry to remove all entities and components
    }

    void DrawGrid(int screenWidth, int screenHeight, int cellCount)
    {
        for (int x = 0; x < screenWidth / cellCount; x += m_gridSize)
        {
            DrawLine(x, 0, x, screenHeight, LIGHTGRAY);
        }
        for (int y = 0; y < screenHeight / cellCount; y += m_gridSize)
        {
            DrawLine(0, y, screenWidth, y, BLACK);
        }
    }

    template <typename T, typename... Args>
    inline void CreateSystem(Args... args)
    {
        std::unique_ptr<T> system = std::make_unique<T>(args...);
        if (!system)
        {
            std::cerr << "Failed to create system of type: " << typeid(T).name() << std::endl;
            return;
        }
        m_systems.emplace_back(std::move(system)); // Store the system in the simulation
        std::cout << "Created system: " << typeid(T).name() << std::endl;
    }

private:
    // state tracking
    bool m_isPaused;
    bool m_boxDropped;
    bool m_drawGrid = true; // Flag to control grid visibility
    int m_gridSize = 20;    // how many cells per row/column
    // TODO: load from config
    int m_initialAltitude = 0;                       // Initial altitude for the box
    int m_horizontalOffset = 600;                    // Horizontal offset for the box
    int m_boxWidth = 20;                             // Width of the box
    int m_boxHeight = 20;                            // Height of the box
    int m_platformWidth = 100;                       // Width of the
    float m_pixelsPerMeter = 40.0f;                  // Pixels per meter for scaling
    std::vector<std::unique_ptr<ISystem>> m_systems; // List of systems in the scene, looped over in the Update function
};