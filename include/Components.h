/**
 * @file Components.h
 * @brief Various component structures used in the ECS for different physics simulations.
 * @date 2025-06-26
 * @details This file defines the structures for 2D vectors, transforms, rigid bodies, gravity, velocity, position, dimensions, and collidable components.
 * It is made to be type agnostic, meaning any platform specific components will have to be defined in a separate file.
 */
#pragma once

extern "C"
{
    #include <raylib.h> // Include raylib for graphics
}

namespace ecs
{
    struct Vec2D
    {
        float x = 0.0f;
        float y = 0.0f;
    };

    struct Vec3D
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
    };

    struct Transform2D
    {
        Vec2D position;
        Vec2D scale = {1.0f, 1.0f};
        float rotation = 0.0f; // in degrees
    };

    struct RigidBody
    {
        // -- linear motion properties --
        Vec2D velocity;
        Vec2D acceleration;
        float mass = 1.0f; 
        float inverseMass = 1.0f; // 1/mass for faster calcs, / is slow

        // -- angular motion properties --
        float angularVelocity = 0.0f; // in radians per second
        float angularAcceleration = 0.0f; // in radians per second squared 
        float momentOfInertia = 1.0f; // for rotation, depends on shape
        float inverseMomentOfInertia = 1.0f; // 1/moment, same as above

        void setMass(float m)
        {
            mass = m;
            inverseMass = (mass == 0.0f) ? 0.0f : 1.0f / mass; // prevent division by zero
        }
    };

    struct Draggable{
        bool isDragged = false;
    };

    struct Tile
    {
        int value = 0;
    };

    struct Position
    {
        float x = 0.0f;
        float y = 0.0f;
    };

    struct Dimensions
    {
        float width;
        float height;
    };

    struct Collidable
    {
        bool isColliding = false;
    };

    struct Gravity
    {
        float value = 9.81f; // Default gravity value in m/s^2
    };

    struct Grounded
    {
    };

    struct MouseInteractible
    {
        bool hovered = false; // Flag to indicate if the entity is hovered
        bool wasClicked = false; // Flag to indicate if the entity was clicked
        bool selected = false; // Flag to indicate if the entity is selected
    };

    #ifdef RAYLIB_H
    struct TextureComponent
    {
        Texture2D texture;    // The raylib texture
        Rectangle sourceRect; // Source rectangle for partial rendering

        TextureComponent(const char *texturePath,
                         Rectangle srcRect = {0, 0, 0, 0})
            : texture(LoadTexture(texturePath)), sourceRect(srcRect)
        {
            if (texture.id == 0) // Check if texture loaded successfully
            {
                fprintf(stderr, "Failed to load texture: %s\n", texturePath);
            }
        }
        ~TextureComponent()
        {
            UnloadTexture(texture); // Unload texture when the component is destroyed
        }
    };

    struct Animation
    {
        TextureComponent texture; // The texture for the animation
        int currentFrame;         // Current frame index
        int frameCount;           // Total number of frames in the animation
        float frameTime;          // Time per frame in seconds
        float elapsedTime;        // Time elapsed since the last frame change

        Animation(const char *texturePath, int frameCount, float frameTime, Rectangle srcRect = {0, 0, 0, 0})
            : texture(texturePath, srcRect), currentFrame(0), frameCount(frameCount), frameTime(frameTime), elapsedTime(0.0f)
        {
            if (frameCount <= 0)
            {
                fprintf(stderr, "Invalid frame count: %d\n", frameCount);
                exit(EXIT_FAILURE);
            }
        }
    };

    struct Drawable
    {
        Color defaultTint = WHITE; // Default color for drawing
        Color tint = WHITE;
    };

    struct Text
    {
        std::string content; // The text content
        Vector2 position;    // Position to draw the text
        int fontSize;        // Font size for the text
        Color color = BLACK; // Color of the text

        Text(const std::string &text, Vector2 pos, int size, Color col = BLACK)
            : content(text), position(pos), fontSize(size), color(col) {}
    };
    struct TextComponent
    {
        Text text; // The text component

        TextComponent(const std::string &textContent, Vector2 pos, int size, Color col = BLACK)
            : text(textContent, pos, size, col) {}
    };
    #endif // RAYLIB_H
};