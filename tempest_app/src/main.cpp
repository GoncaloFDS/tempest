#include "spdlog/spdlog.h"

#include "src/engine.h"

int main() {
    Engine engine;

    engine.Init();
    engine.Run();

    spdlog::info("Hello, World");
}
