#include "bindings.hpp"

// ============== CAMERA HELPER BINDINGS ==============

namespace CameraHelperBindings
{
    // worldToScreen(camera, worldX, worldY, worldZ, viewportWidth, viewportHeight) -> screenX, screenY, isVisible
    int worldToScreen(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 6)
        {
            Error("worldToScreen: requires camera, world position(x,y,z), viewport size(w,h)");
            vm->pushFloat(0.0f);
            vm->pushFloat(0.0f);
            vm->pushBool(false);
            return 3;
        }

        NativeClassInstance *cameraInstance = args[0].asNativeClassInstance();
        Ogre::Camera *camera = static_cast<Ogre::Camera *>(cameraInstance->userData);

        if (!camera)
        {
            vm->pushFloat(0.0f);
            vm->pushFloat(0.0f);
            vm->pushBool(false);
            return 3;
        }

        Ogre::Vector3 worldPos(
            (float)args[1].asNumber(),
            (float)args[2].asNumber(),
            (float)args[3].asNumber()
        );

        float viewportWidth = (float)args[4].asNumber();
        float viewportHeight = (float)args[5].asNumber();

        // Project world position to screen
        Ogre::Vector3 screenPos = camera->getProjectionMatrix() * (camera->getViewMatrix() * worldPos);

        // Check if behind camera
        if (screenPos.z < 0)
        {
            vm->pushFloat(0.0f);
            vm->pushFloat(0.0f);
            vm->pushBool(false);
            return 3;
        }

        // Convert from [-1,1] to screen coordinates
        float screenX = (screenPos.x + 1.0f) * 0.5f * viewportWidth;
        float screenY = (1.0f - screenPos.y) * 0.5f * viewportHeight;

        // Check if on screen
        bool isVisible = (screenX >= 0 && screenX <= viewportWidth &&
                         screenY >= 0 && screenY <= viewportHeight);

        vm->pushFloat(screenX);
        vm->pushFloat(screenY);
        vm->pushBool(isVisible);
        return 3;
    }

    // isPositionVisible(camera, worldX, worldY, worldZ) -> bool
    int isPositionVisible(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 4)
        {
            Error("isPositionVisible: requires camera and world position(x,y,z)");
            vm->pushBool(false);
            return 1;
        }

        NativeClassInstance *cameraInstance = args[0].asNativeClassInstance();
        Ogre::Camera *camera = static_cast<Ogre::Camera *>(cameraInstance->userData);

        if (!camera)
        {
            vm->pushBool(false);
            return 1;
        }

        Ogre::Vector3 worldPos(
            (float)args[1].asNumber(),
            (float)args[2].asNumber(),
            (float)args[3].asNumber()
        );

        // Check if position is in frustum
        bool visible = camera->isVisible(worldPos);

        vm->pushBool(visible);
        return 1;
    }

    // getDistanceToCamera(camera, worldX, worldY, worldZ) -> float
    int getDistanceToCamera(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 4)
        {
            Error("getDistanceToCamera: requires camera and world position(x,y,z)");
            vm->pushFloat(0.0f);
            return 1;
        }

        NativeClassInstance *cameraInstance = args[0].asNativeClassInstance();
        Ogre::Camera *camera = static_cast<Ogre::Camera *>(cameraInstance->userData);

        if (!camera)
        {
            vm->pushFloat(0.0f);
            return 1;
        }

        Ogre::Vector3 worldPos(
            (float)args[1].asNumber(),
            (float)args[2].asNumber(),
            (float)args[3].asNumber()
        );

        Ogre::Vector3 cameraPos = camera->getDerivedPosition();
        float distance = cameraPos.distance(worldPos);

        vm->pushFloat(distance);
        return 1;
    }

    // getCameraForwardVector(camera) -> x, y, z
    int getCameraForwardVector(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            Error("getCameraForwardVector: requires camera");
            vm->pushFloat(0.0f);
            vm->pushFloat(0.0f);
            vm->pushFloat(1.0f);
            return 3;
        }

        NativeClassInstance *cameraInstance = args[0].asNativeClassInstance();
        Ogre::Camera *camera = static_cast<Ogre::Camera *>(cameraInstance->userData);

        if (!camera)
        {
            vm->pushFloat(0.0f);
            vm->pushFloat(0.0f);
            vm->pushFloat(1.0f);
            return 3;
        }

        Ogre::Vector3 forward = camera->getDerivedDirection();

        vm->pushFloat(forward.x);
        vm->pushFloat(forward.y);
        vm->pushFloat(forward.z);
        return 3;
    }

    // getCameraRightVector(camera) -> x, y, z
    int getCameraRightVector(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            vm->pushFloat(1.0f);
            vm->pushFloat(0.0f);
            vm->pushFloat(0.0f);
            return 3;
        }

        NativeClassInstance *cameraInstance = args[0].asNativeClassInstance();
        Ogre::Camera *camera = static_cast<Ogre::Camera *>(cameraInstance->userData);

        if (!camera)
        {
            vm->pushFloat(1.0f);
            vm->pushFloat(0.0f);
            vm->pushFloat(0.0f);
            return 3;
        }

        Ogre::Vector3 right = camera->getDerivedRight();

        vm->pushFloat(right.x);
        vm->pushFloat(right.y);
        vm->pushFloat(right.z);
        return 3;
    }

    // getCameraUpVector(camera) -> x, y, z
    int getCameraUpVector(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            vm->pushFloat(0.0f);
            vm->pushFloat(1.0f);
            vm->pushFloat(0.0f);
            return 3;
        }

        NativeClassInstance *cameraInstance = args[0].asNativeClassInstance();
        Ogre::Camera *camera = static_cast<Ogre::Camera *>(cameraInstance->userData);

        if (!camera)
        {
            vm->pushFloat(0.0f);
            vm->pushFloat(1.0f);
            vm->pushFloat(0.0f);
            return 3;
        }

        Ogre::Vector3 up = camera->getDerivedUp();

        vm->pushFloat(up.x);
        vm->pushFloat(up.y);
        vm->pushFloat(up.z);
        return 3;
    }

    // setNearClipDistance(camera, distance)
    int setNearClipDistance(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 2) return 0;

        NativeClassInstance *cameraInstance = args[0].asNativeClassInstance();
        Ogre::Camera *camera = static_cast<Ogre::Camera *>(cameraInstance->userData);

        if (camera)
        {
            camera->setNearClipDistance((float)args[1].asNumber());
        }

        return 0;
    }

    // setFarClipDistance(camera, distance)
    int setFarClipDistance(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 2) return 0;

        NativeClassInstance *cameraInstance = args[0].asNativeClassInstance();
        Ogre::Camera *camera = static_cast<Ogre::Camera *>(cameraInstance->userData);

        if (camera)
        {
            camera->setFarClipDistance((float)args[1].asNumber());
        }

        return 0;
    }

   

    void registerAll(Interpreter &vm)
    {
        // Global camera helper functions
        // vm.registerNative("worldToScreen", worldToScreen, 6);
        // vm.registerNative("isPositionVisible", isPositionVisible, 4);
        // vm.registerNative("getDistanceToCamera", getDistanceToCamera, 4);
        // vm.registerNative("getCameraForwardVector", getCameraForwardVector, 1);
        // vm.registerNative("getCameraRightVector", getCameraRightVector, 1);
        // vm.registerNative("getCameraUpVector", getCameraUpVector, 1);
        // vm.registerNative("setNearClipDistance", setNearClipDistance, 2);
        // vm.registerNative("setFarClipDistance", setFarClipDistance, 2);
        // vm.registerNative("setFOV", setFOV, 2);
        // vm.registerNative("setAspectRatio", setAspectRatio, 2);

        // Info("Camera helper bindings registered");
    }

} // namespace CameraHelperBindings
