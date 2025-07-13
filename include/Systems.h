/**
 * @file Systems.h
 * @author Sebastian Lorensson
 * @brief Contains ISystem interface as well as basic universal system implementations.
 * @version 0.1
 * @date 2025-06-27
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

// include raylib for graphics, using extern "C" to avoid C++ name mangling if C++ is used
#ifdef __cplusplus
extern "C"
{
#endif
#include <raylib.h>
#ifdef __cplusplus
}
#endif

#include <entt/entt.hpp>

#include "Components.h"

class ISystem
{
public:
    virtual ~ISystem() = default;

    virtual bool OnUpdate(entt::registry &registry, float deltaTime) = 0;
};

// basic system to test the interface. Just writes deltaTime to the screen
class TextInterface : public ISystem
{
public:
    bool OnUpdate(entt::registry &registry, float deltaTime) override
    {
        if (!m_enabled)
            return false;

        auto view = registry.view<ecs::Text>(entt::exclude<ecs::Drawable>);
        bool updated = false;

        view.each([&](entt::entity e, const ecs::Text &textComponent)
                  {
            registry.emplace<ecs::Drawable>(e, ecs::Drawable{textComponent.color});
            if(!updated) 
                updated = true; });

        return updated;
    }

    void Toggle()
    {
        m_enabled = !m_enabled;
    }

private:
    bool m_enabled = true;
};