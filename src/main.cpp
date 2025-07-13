#include <iostream> // for printouts

#include "Components.h"
#include "Simulation.h"
#include "Maths.h"
#include "Sandbox.h"

static constexpr int SCREEN_WIDTH = 800;  // Default screen width
static constexpr int SCREEN_HEIGHT = 600; // Default screen height
static constexpr const char *TITLE = "Gravity Game"; // Default window title
static constexpr unsigned FLAGS = FLAG_WINDOW_HIGHDPI;

int main()
{
    Sandbox sandbox(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE, FLAGS);
    sandbox.Run(); 
}


#ifdef RUN_GRAVITY_GAME
#include "GravityGame.h"
void RunGravityGame()
{
    GravityGame game(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE);
    game.Run();
}
#endif