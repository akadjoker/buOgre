#include "bindings.hpp"
#include <SDL2/SDL.h>
#include <queue>

namespace InputBindings
{
    // Keyboard state - usando scancodes (números)
    bool gCurrentKeyState[512] = {false};
    bool gPreviousKeyState[512] = {false};
    std::queue<int> gKeyPressedQueue;
    std::queue<int> gCharPressedQueue;

    // Mouse state
    bool gCurrentMouseButtonState[3] = {false, false, false};  // Left, Right, Middle
    bool gPreviousMouseButtonState[3] = {false, false, false};
    int gMouseX = 0, gMouseY = 0;
    int gPrevMouseX = 0, gPrevMouseY = 0;
    int gMouseWheelMove = 0;

    // Update input state - deve ser chamado a cada evento SDL
    void updateInput(SDL_Event &event)
    {
        // Guarda posição anterior
        gPrevMouseX = gMouseX;
        gPrevMouseY = gMouseY;
        gMouseWheelMove = 0;

        if (event.type == SDL_KEYDOWN && !event.key.repeat)
        {
            int scancode = event.key.keysym.scancode;
            if (scancode < 512) gCurrentKeyState[scancode] = true;
            gKeyPressedQueue.push(scancode);

            Info("Key pressed: scancode=%d", scancode);
            
            // Char pressionado
            if (event.key.keysym.sym >= 32 && event.key.keysym.sym <= 126)
            {
                gCharPressedQueue.push(event.key.keysym.sym);
            }
        }
        else if (event.type == SDL_KEYUP)
        {
            int scancode = event.key.keysym.scancode;
            if (scancode < 512) gCurrentKeyState[scancode] = false;
        }
        else if (event.type == SDL_MOUSEMOTION)
        {
            gMouseX = event.motion.x;
            gMouseY = event.motion.y;
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            if (event.button.button == SDL_BUTTON_LEFT)
                gCurrentMouseButtonState[0] = true;
            else if (event.button.button == SDL_BUTTON_RIGHT)
                gCurrentMouseButtonState[1] = true;
            else if (event.button.button == SDL_BUTTON_MIDDLE)
                gCurrentMouseButtonState[2] = true;
        }
        else if (event.type == SDL_MOUSEBUTTONUP)
        {
            if (event.button.button == SDL_BUTTON_LEFT)
                gCurrentMouseButtonState[0] = false;
            else if (event.button.button == SDL_BUTTON_RIGHT)
                gCurrentMouseButtonState[1] = false;
            else if (event.button.button == SDL_BUTTON_MIDDLE)
                gCurrentMouseButtonState[2] = false;
        }
        else if (event.type == SDL_MOUSEWHEEL)
        {
            gMouseWheelMove = event.wheel.y;
        }
    }

 
    void updateInputState()
    {
        // Atualiza estado anterior
        for (int i = 0; i < 512; i++)
        {
            gPreviousKeyState[i] = gCurrentKeyState[i];
        }
        for (int i = 0; i < 3; i++)
        {
            gPreviousMouseButtonState[i] = gCurrentMouseButtonState[i];
        }
    }

    // isKeyPressed(scancode: int) -> bool (pressionado uma vez este frame)
    int input_isKeyPressed(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            vm->pushBool(false);
            return 1;
        }

        int scancode = (int)args[0].asNumber();
        if (scancode < 0 || scancode >= 512) {
            vm->pushBool(false);
            return 1;
        }
        
        bool pressed = gCurrentKeyState[scancode] && !gPreviousKeyState[scancode];
        vm->pushBool(pressed);
        return 1;
    }

    // isKeyDown(scancode: int) -> bool (está pressionado neste momento)
    int input_isKeyDown(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            vm->pushBool(false);
            return 1;
        }

        int scancode = (int)args[0].asNumber();
        if (scancode < 0 || scancode >= 512) {
            vm->pushBool(false);
            return 1;
        }
        
        bool down = gCurrentKeyState[scancode];
        vm->pushBool(down);
        return 1;
    }

    // isKeyReleased(scancode: int) -> bool (solto uma vez este frame)
    int input_isKeyReleased(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            vm->pushBool(false);
            return 1;
        }

        int scancode = (int)args[0].asNumber();
        if (scancode < 0 || scancode >= 512) {
            vm->pushBool(false);
            return 1;
        }
        
        bool released = !gCurrentKeyState[scancode] && gPreviousKeyState[scancode];
        vm->pushBool(released);
        return 1;
    }

    // isKeyUp(scancode: int) -> bool (não está pressionado)
    int input_isKeyUp(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            vm->pushBool(true);
            return 1;
        }

        int scancode = (int)args[0].asNumber();
        if (scancode < 0 || scancode >= 512) {
            vm->pushBool(true);
            return 1;
        }
        
        bool up = !gCurrentKeyState[scancode];
        vm->pushBool(up);
        return 1;
    }

    // getKeyPressed() -> int (última tecla da fila)
    int input_getKeyPressed(Interpreter *vm, int argCount, Value *args)
    {
        int key = 0;
        if (!gKeyPressedQueue.empty())
        {
            key = gKeyPressedQueue.front();
            gKeyPressedQueue.pop();
        }
        vm->pushInt(key);
        return 1;
    }

    // getCharPressed() -> int (último char da fila)
    int input_getCharPressed(Interpreter *vm, int argCount, Value *args)
    {
        int ch = 0;
        if (!gCharPressedQueue.empty())
        {
            ch = gCharPressedQueue.front();
            gCharPressedQueue.pop();
        }
        vm->pushInt(ch);
        return 1;
    }

    // ===== MOUSE FUNCTIONS =====

    // isMouseButtonPressed(button: 0=left, 1=right, 2=middle) -> bool
    int input_isMouseButtonPressed(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            vm->pushBool(false);
            return 1;
        }
        int button = (int)args[0].asNumber();
        if (button < 0 || button >= 3) button = 0;
        
        bool pressed = gCurrentMouseButtonState[button] && !gPreviousMouseButtonState[button];
        vm->pushBool(pressed);
        return 1;
    }

    // isMouseButtonDown(button: 0=left, 1=right, 2=middle) -> bool
    int input_isMouseButtonDown(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            vm->pushBool(false);
            return 1;
        }
        int button = (int)args[0].asNumber();
        if (button < 0 || button >= 3) button = 0;
        
        bool down = gCurrentMouseButtonState[button];
        vm->pushBool(down);
        return 1;
    }

    // isMouseButtonReleased(button: 0=left, 1=right, 2=middle) -> bool
    int input_isMouseButtonReleased(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            vm->pushBool(false);
            return 1;
        }
        int button = (int)args[0].asNumber();
        if (button < 0 || button >= 3) button = 0;
        
        bool released = !gCurrentMouseButtonState[button] && gPreviousMouseButtonState[button];
        vm->pushBool(released);
        return 1;
    }

    // isMouseButtonUp(button: 0=left, 1=right, 2=middle) -> bool
    int input_isMouseButtonUp(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            vm->pushBool(true);
            return 1;
        }
        int button = (int)args[0].asNumber();
        if (button < 0 || button >= 3) button = 0;
        
        bool up = !gCurrentMouseButtonState[button];
        vm->pushBool(up);
        return 1;
    }

    // getMouseX() -> int
    int input_getMouseX(Interpreter *vm, int argCount, Value *args)
    {
        vm->pushInt(gMouseX);
        return 1;
    }

    // getMouseY() -> int
    int input_getMouseY(Interpreter *vm, int argCount, Value *args)
    {
        vm->pushInt(gMouseY);
        return 1;
    }

    // getMousePosition() -> string "Vector2(x, y)"
    int input_getMousePosition(Interpreter *vm, int argCount, Value *args)
    {
        vm->pushInt(gMouseX);
        vm->pushInt(gMouseY);
        return 2;
    }

    // setMousePosition(x, y)
    int input_setMousePosition(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 2) return 0;
        gMouseX = (int)args[0].asNumber();
        gMouseY = (int)args[1].asNumber();
        return 0;
    }

    // getMouseWheelMove() -> int
    int input_getMouseWheelMove(Interpreter *vm, int argCount, Value *args)
    {
        vm->pushInt(gMouseWheelMove);
        return 1;
    }

    // setMouseRelativeMode(enabled: bool)
    int input_setMouseRelativeMode(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        bool enabled = args[0].asBool();
        SDL_SetRelativeMouseMode(enabled ? SDL_TRUE : SDL_FALSE);
        return 0;
    }

    void registerAll(Interpreter &vm)
    {
        // Keyboard functions
        vm.registerNative("isKeyPressed", input_isKeyPressed, 1);
        vm.registerNative("isKeyDown", input_isKeyDown, 1);
        vm.registerNative("isKeyReleased", input_isKeyReleased, 1);
        vm.registerNative("isKeyUp", input_isKeyUp, 1);
        vm.registerNative("getKeyPressed", input_getKeyPressed, 0);
        vm.registerNative("getCharPressed", input_getCharPressed, 0);

        // Mouse functions
        vm.registerNative("isMouseButtonPressed", input_isMouseButtonPressed, 1);
        vm.registerNative("isMouseButtonDown", input_isMouseButtonDown, 1);
        vm.registerNative("isMouseButtonReleased", input_isMouseButtonReleased, 1);
        vm.registerNative("isMouseButtonUp", input_isMouseButtonUp, 1);
        vm.registerNative("getMouseX", input_getMouseX, 0);
        vm.registerNative("getMouseY", input_getMouseY, 0);
        vm.registerNative("getMousePosition", input_getMousePosition, 0);
        vm.registerNative("setMousePosition", input_setMousePosition, 2);
        vm.registerNative("getMouseWheelMove", input_getMouseWheelMove, 0);
        vm.registerNative("setMouseRelativeMode", input_setMouseRelativeMode, 1);

        Info("Input bindings registered");
    }

} // namespace InputBindings
