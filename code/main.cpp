#include "game.h"
#include <engine/engine.h>

int common_main()
{
    Engine &engine = Engine::GetInstance();
    engine.SetApplication(new Game);

    if (!engine.Init(1600, 900)) {
        ERROR("Failed to initialize engine. Shutting down...");
        return 1;
    }
    else {
        engine.Run();
    }
    engine.Shutdown();

    return 0;
}

#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN
#define _VCEXTRALEAN

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    return common_main();
}

#else

int main(int, char*[])
{
    return common_main();
}

#endif
