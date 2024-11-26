#include "spdlog/spdlog.h"

#include "src/engine.h"

int main() {
    Engine engine;

    engine.Init();
    engine.Run();
    engine.Draw();

    spdlog::info("Hello, World");
}
