#include "bindings.hpp"
#include <chrono>

namespace TimerBindings
{
    // Timer state
    static auto gStartTime = std::chrono::high_resolution_clock::now();
    static auto gLastFrameTime = std::chrono::high_resolution_clock::now();
    static double gDeltaTime = 0.0;
    static double gElapsedTime = 0.0;
    static int gFrameCount = 0;
    static double gFPS = 0.0;
    static double gFPSUpdateTime = 0.0;

    // Call this once per frame (from main loop)
    void updateTimer()
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> deltaSeconds = currentTime - gLastFrameTime;
        gDeltaTime = deltaSeconds.count();
        
        std::chrono::duration<double> elapsedSeconds = currentTime - gStartTime;
        gElapsedTime = elapsedSeconds.count();
        
        gLastFrameTime = currentTime;
        gFrameCount++;
        gFPSUpdateTime += gDeltaTime;
        
        // Update FPS every 0.5 seconds
        if (gFPSUpdateTime >= 0.5)
        {
            gFPS = (double)gFrameCount / gFPSUpdateTime;
            gFrameCount = 0;
            gFPSUpdateTime = 0.0;
        }
    }

    // getDeltaTime() -> float (tempo desde o último frame em segundos)
    int timer_getDeltaTime(Interpreter *vm, int argCount, Value *args)
    {
        vm->pushDouble(gDeltaTime);
        return 1;
    }

    // getElapsedTime() -> float (tempo total decorrido em segundos)
    int timer_getElapsedTime(Interpreter *vm, int argCount, Value *args)
    {
        vm->pushDouble  (gElapsedTime);
        return 1;
    }

    // getFPS() -> float
    int timer_getFPS(Interpreter *vm, int argCount, Value *args)
    {
        vm->pushDouble(gFPS);
        return 1;
    }

    // getFrameCount() -> int (número total de frames desde o inicio)
    int timer_getFrameCount(Interpreter *vm, int argCount, Value *args)
    {
        vm->pushInt(gFrameCount);
        return 1;
    }

    // reset() - reinicia o timer
    int timer_reset(Interpreter *vm, int argCount, Value *args)
    {
        gStartTime = std::chrono::high_resolution_clock::now();
        gLastFrameTime = gStartTime;
        gDeltaTime = 0.0;
        gElapsedTime = 0.0;
        gFrameCount = 0;
        gFPS = 0.0;
        gFPSUpdateTime = 0.0;
        return 0;
    }

    void registerAll(Interpreter &vm)
    {
        vm.registerNative("getDeltaTime", timer_getDeltaTime, 0);
        vm.registerNative("getElapsedTime", timer_getElapsedTime, 0);
        vm.registerNative("getFPS", timer_getFPS, 0);
        vm.registerNative("getFrameCount", timer_getFrameCount, 0);
        vm.registerNative("resetTimer", timer_reset, 0);
        
        Info("Timer bindings registered");
    }

} // namespace TimerBindings
