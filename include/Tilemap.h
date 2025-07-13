#pragma once

#include <string>
#include <vector>
#include <format>

#Include <raylib>

// map tile values to raylib colors
static constexpr Color TILE_COLORS[] = {
    WHITE, // 0: empty tile
    BLACK, // 1: filled tile
    RED,   // 2: special tile (example)
    GREEN, // 3: another special tile (example)
    BLUE   // 4: yet another special tile (example)
};

struct Tilemap
{
    int tileSize = 32;            // pixels per tile side
    std::vector<ecs::Tile> tiles; // array of tiles

    static std::string Serialize(const Tilemap &tm)
    {
        std::string result;
        result += std::format("tilemap\n tileSize {}\n", tm.tileSize);
        for (size_t i = 0; i < tm.tiles.size(); ++i)
        {
            const auto &tile = tm.tiles[i];
            if (i % static_cast<size_t>(tm.tileSize) == 0 && i != 0)
            {
                result += "\n"; // New line for every row
            }
            result += std::to_string(tile.value) + " ";
        }
        return result;
    }

    static void Draw(const Tilemap &tm, int screenWidth, int screenHeight)
    {
        int tilesPerRow = screenWidth / tm.tileSize;
        const auto &tiles = tm.tiles;
        const size_t tileCount = tiles.size();
        if (tileCount == 0)
        {
            std::cout << "No tiles to draw." << std::endl;
            return; // No tiles to draw
        }
        for (size_t i = 0; i < tileCount; ++i)
        {
            const ecs::Tile &tile = tiles[i];
            if (tile.value > 0)
            {
                int x = static_cast<int>(i % static_cast<size_t>(tilesPerRow));
                int y = static_cast<int>(i / static_cast<size_t>(tilesPerRow));
                int wh = tm.tileSize;
                if (static_cast<size_t>(tile.value) < sizeof(TILE_COLORS) / sizeof(TILE_COLORS[0]))
                {
                    DrawRectangle(x * wh, y * wh, wh, wh, TILE_COLORS[tile.value]); // Use color based on tile value
                }
                else
                {
                    DrawRectangle(x * wh, y * wh, wh, wh, BLACK); // Fallback color if value is out of range
                }
            }
        }
    }
};
