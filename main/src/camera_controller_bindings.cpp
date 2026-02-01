#include "bindings.hpp"
#include "camera_controller.hpp"

using namespace CameraControllers;

// ============== FPS CONTROLLER BINDINGS ==============

namespace FPSControllerBindings
{
    // Constructor: FPSController(cameraNode)
    void *fps_ctor(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            Error("FPSController: requires camera node argument");
            return nullptr;
        }

        NativeClassInstance *nodeInstance = args[0].asNativeClassInstance();
        Ogre::SceneNode *cameraNode = static_cast<Ogre::SceneNode *>(nodeInstance->userData);

        if (!cameraNode)
        {
            Error("FPSController: invalid camera node");
            return nullptr;
        }

        FPSController *controller = new FPSController(cameraNode);
        Info("FPSController created");
        return controller;
    }

    void fps_dtor(Interpreter *vm, void *data)
    {
        delete static_cast<FPSController *>(data);
        Info("FPSController destroyed");
    }

    // update(deltaTime, mouseDeltaX, mouseDeltaY)
    int fps_update(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3) return 0;
        FPSController *controller = static_cast<FPSController *>(data);

        float deltaTime = (float)args[0].asNumber();
        int mouseDeltaX = (int)args[1].asNumber();
        int mouseDeltaY = (int)args[2].asNumber();

        controller->update(deltaTime, mouseDeltaX, mouseDeltaY);
        return 0;
    }

    // setMoveForward(bool)
    int fps_setMoveForward(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        static_cast<FPSController *>(data)->setMoveForward(args[0].asBool());
        return 0;
    }

    int fps_setMoveBackward(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        static_cast<FPSController *>(data)->setMoveBackward(args[0].asBool());
        return 0;
    }

    int fps_setMoveLeft(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        static_cast<FPSController *>(data)->setMoveLeft(args[0].asBool());
        return 0;
    }

    int fps_setMoveRight(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        static_cast<FPSController *>(data)->setMoveRight(args[0].asBool());
        return 0;
    }

    int fps_setMoveUp(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        static_cast<FPSController *>(data)->setMoveUp(args[0].asBool());
        return 0;
    }

    int fps_setMoveDown(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        static_cast<FPSController *>(data)->setMoveDown(args[0].asBool());
        return 0;
    }

    // Properties
    Value fps_getMoveSpeed(Interpreter *vm, void *data)
    {
        return vm->makeFloat(static_cast<FPSController *>(data)->getMoveSpeed());
    }

    void fps_setMoveSpeed(Interpreter *vm, void *data, Value value)
    {
        static_cast<FPSController *>(data)->setMoveSpeed((float)value.asNumber());
    }

    Value fps_getMouseSensitivity(Interpreter *vm, void *data)
    {
        return vm->makeFloat(static_cast<FPSController *>(data)->getMouseSensitivity());
    }

    void fps_setMouseSensitivity(Interpreter *vm, void *data, Value value)
    {
        static_cast<FPSController *>(data)->setMouseSensitivity((float)value.asNumber());
    }

    Value fps_getEnabled(Interpreter *vm, void *data)
    {
        return vm->makeBool(static_cast<FPSController *>(data)->isEnabled());
    }

    void fps_setEnabled(Interpreter *vm, void *data, Value value)
    {
        static_cast<FPSController *>(data)->setEnabled(value.asBool());
    }

    void registerAll(Interpreter &vm)
    {
        NativeClassDef *fps = vm.registerNativeClass(
            "FPSController",
            fps_ctor,
            fps_dtor,
            1,  // properties: moveSpeed, mouseSensitivity, enabled
            false
        );

        // Properties
        vm.addNativeProperty(fps, "moveSpeed", fps_getMoveSpeed, fps_setMoveSpeed);
        vm.addNativeProperty(fps, "mouseSensitivity", fps_getMouseSensitivity, fps_setMouseSensitivity);
        vm.addNativeProperty(fps, "enabled", fps_getEnabled, fps_setEnabled);

        // Methods
        vm.addNativeMethod(fps, "update", fps_update);
        vm.addNativeMethod(fps, "setMoveForward", fps_setMoveForward);
        vm.addNativeMethod(fps, "setMoveBackward", fps_setMoveBackward);
        vm.addNativeMethod(fps, "setMoveLeft", fps_setMoveLeft);
        vm.addNativeMethod(fps, "setMoveRight", fps_setMoveRight);
        vm.addNativeMethod(fps, "setMoveUp", fps_setMoveUp);
        vm.addNativeMethod(fps, "setMoveDown", fps_setMoveDown);

        Info("FPSController bindings registered");
    }
}

// ============== ORBIT CONTROLLER BINDINGS ==============

namespace OrbitControllerBindings
{
    void *orbit_ctor(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            Error("OrbitController: requires camera node argument");
            return nullptr;
        }

        NativeClassInstance *nodeInstance = args[0].asNativeClassInstance();
        Ogre::SceneNode *cameraNode = static_cast<Ogre::SceneNode *>(nodeInstance->userData);

        if (!cameraNode)
        {
            Error("OrbitController: invalid camera node");
            return nullptr;
        }

        Ogre::Vector3 target = Ogre::Vector3::ZERO;
        if (argCount >= 4)
        {
            target.x = (float)args[1].asNumber();
            target.y = (float)args[2].asNumber();
            target.z = (float)args[3].asNumber();
        }

        OrbitController *controller = new OrbitController(cameraNode, target);
        Info("OrbitController created");
        return controller;
    }

    void orbit_dtor(Interpreter *vm, void *data)
    {
        delete static_cast<OrbitController *>(data);
        Info("OrbitController destroyed");
    }

    int orbit_update(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3) return 0;
        OrbitController *controller = static_cast<OrbitController *>(data);

        float deltaTime = (float)args[0].asNumber();
        int mouseDeltaX = (int)args[1].asNumber();
        int mouseDeltaY = (int)args[2].asNumber();

        controller->update(deltaTime, mouseDeltaX, mouseDeltaY);
        return 0;
    }

    int orbit_setTarget(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3) return 0;
        OrbitController *controller = static_cast<OrbitController *>(data);

        Ogre::Vector3 target(
            (float)args[0].asNumber(),
            (float)args[1].asNumber(),
            (float)args[2].asNumber()
        );

        controller->setTarget(target);
        return 0;
    }

    int orbit_zoom(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        static_cast<OrbitController *>(data)->zoom((float)args[0].asNumber());
        return 0;
    }

    Value orbit_getMoveSpeed(Interpreter *vm, void *data)
    {
        return vm->makeFloat(static_cast<OrbitController *>(data)->getMoveSpeed());
    }

    void orbit_setMoveSpeed(Interpreter *vm, void *data, Value value)
    {
        static_cast<OrbitController *>(data)->setMoveSpeed((float)value.asNumber());
    }

    Value orbit_getMouseSensitivity(Interpreter *vm, void *data)
    {
        return vm->makeFloat(static_cast<OrbitController *>(data)->getMouseSensitivity());
    }

    void orbit_setMouseSensitivity(Interpreter *vm, void *data, Value value)
    {
        static_cast<OrbitController *>(data)->setMouseSensitivity((float)value.asNumber());
    }

    Value orbit_getDistance(Interpreter *vm, void *data)
    {
        return vm->makeFloat(static_cast<OrbitController *>(data)->getDistance());
    }

    void orbit_setDistance(Interpreter *vm, void *data, Value value)
    {
        static_cast<OrbitController *>(data)->setDistance((float)value.asNumber());
    }

    Value orbit_getEnabled(Interpreter *vm, void *data)
    {
        return vm->makeBool(static_cast<OrbitController *>(data)->isEnabled());
    }

    void orbit_setEnabled(Interpreter *vm, void *data, Value value)
    {
        static_cast<OrbitController *>(data)->setEnabled(value.asBool());
    }

    void registerAll(Interpreter &vm)
    {
        NativeClassDef *orbit = vm.registerNativeClass(
            "OrbitController",
            orbit_ctor,
            orbit_dtor,
            4,  // properties
            false
        );

        vm.addNativeProperty(orbit, "moveSpeed", orbit_getMoveSpeed, orbit_setMoveSpeed);
        vm.addNativeProperty(orbit, "mouseSensitivity", orbit_getMouseSensitivity, orbit_setMouseSensitivity);
        vm.addNativeProperty(orbit, "distance", orbit_getDistance, orbit_setDistance);
        vm.addNativeProperty(orbit, "enabled", orbit_getEnabled, orbit_setEnabled);

        vm.addNativeMethod(orbit, "update", orbit_update);
        vm.addNativeMethod(orbit, "setTarget", orbit_setTarget);
        vm.addNativeMethod(orbit, "zoom", orbit_zoom);

        Info("OrbitController bindings registered");
    }
}

// ============== FREE CAMERA CONTROLLER BINDINGS ==============

namespace FreeCameraControllerBindings
{
    void *free_ctor(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            Error("FreeCameraController: requires camera node argument");
            return nullptr;
        }

        NativeClassInstance *nodeInstance = args[0].asNativeClassInstance();
        Ogre::SceneNode *cameraNode = static_cast<Ogre::SceneNode *>(nodeInstance->userData);

        if (!cameraNode)
        {
            Error("FreeCameraController: invalid camera node");
            return nullptr;
        }

        FreeCameraController *controller = new FreeCameraController(cameraNode);
        Info("FreeCameraController created");
        return controller;
    }

    void free_dtor(Interpreter *vm, void *data)
    {
        delete static_cast<FreeCameraController *>(data);
        Info("FreeCameraController destroyed");
    }

    int free_update(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3) return 0;
        FreeCameraController *controller = static_cast<FreeCameraController *>(data);

        float deltaTime = (float)args[0].asNumber();
        int mouseDeltaX = (int)args[1].asNumber();
        int mouseDeltaY = (int)args[2].asNumber();

        controller->update(deltaTime, mouseDeltaX, mouseDeltaY);
        return 0;
    }

    int free_setMoveForward(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        static_cast<FreeCameraController *>(data)->setMoveForward(args[0].asBool());
        return 0;
    }

    int free_setMoveBackward(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        static_cast<FreeCameraController *>(data)->setMoveBackward(args[0].asBool());
        return 0;
    }

    int free_setMoveLeft(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        static_cast<FreeCameraController *>(data)->setMoveLeft(args[0].asBool());
        return 0;
    }

    int free_setMoveRight(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        static_cast<FreeCameraController *>(data)->setMoveRight(args[0].asBool());
        return 0;
    }

    int free_setMoveUp(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        static_cast<FreeCameraController *>(data)->setMoveUp(args[0].asBool());
        return 0;
    }

    int free_setMoveDown(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        static_cast<FreeCameraController *>(data)->setMoveDown(args[0].asBool());
        return 0;
    }

    Value free_getMoveSpeed(Interpreter *vm, void *data)
    {
        return vm->makeFloat(static_cast<FreeCameraController *>(data)->getMoveSpeed());
    }

    void free_setMoveSpeed(Interpreter *vm, void *data, Value value)
    {
        static_cast<FreeCameraController *>(data)->setMoveSpeed((float)value.asNumber());
    }

    Value free_getMouseSensitivity(Interpreter *vm, void *data)
    {
        return vm->makeFloat(static_cast<FreeCameraController *>(data)->getMouseSensitivity());
    }

    void free_setMouseSensitivity(Interpreter *vm, void *data, Value value)
    {
        static_cast<FreeCameraController *>(data)->setMouseSensitivity((float)value.asNumber());
    }

    Value free_getEnabled(Interpreter *vm, void *data)
    {
        return vm->makeBool(static_cast<FreeCameraController *>(data)->isEnabled());
    }

    void free_setEnabled(Interpreter *vm, void *data, Value value)
    {
        static_cast<FreeCameraController *>(data)->setEnabled(value.asBool());
    }

    void registerAll(Interpreter &vm)
    {
        NativeClassDef *freeCam = vm.registerNativeClass(
            "FreeCameraController",
            free_ctor,
            free_dtor,
            1,
            false
        );

        vm.addNativeProperty(freeCam, "moveSpeed", free_getMoveSpeed, free_setMoveSpeed);
        vm.addNativeProperty(freeCam, "mouseSensitivity", free_getMouseSensitivity, free_setMouseSensitivity);
        vm.addNativeProperty(freeCam, "enabled", free_getEnabled, free_setEnabled);

        vm.addNativeMethod(freeCam, "update", free_update);
        vm.addNativeMethod(freeCam, "setMoveForward", free_setMoveForward);
        vm.addNativeMethod(freeCam, "setMoveBackward", free_setMoveBackward);
        vm.addNativeMethod(freeCam, "setMoveLeft", free_setMoveLeft);
        vm.addNativeMethod(freeCam, "setMoveRight", free_setMoveRight);
        vm.addNativeMethod(freeCam, "setMoveUp", free_setMoveUp);
        vm.addNativeMethod(freeCam, "setMoveDown", free_setMoveDown);

        Info("FreeCameraController bindings registered");
    }
}

// ============== THIRD PERSON CONTROLLER BINDINGS ==============
namespace ThirdPersonControllerBindings
{
    void *thirdperson_ctor(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 2)
        {
            Error("ThirdPersonController: requires cameraNode and targetNode arguments");
            return nullptr;
        }

        NativeClassInstance *cameraNodeInstance = args[0].asNativeClassInstance();
        Ogre::SceneNode *cameraNode = static_cast<Ogre::SceneNode *>(cameraNodeInstance->userData);

        NativeClassInstance *targetNodeInstance = args[1].asNativeClassInstance();
        Ogre::SceneNode *targetNode = static_cast<Ogre::SceneNode *>(targetNodeInstance->userData);

        if (!cameraNode || !targetNode)
        {
            Error("ThirdPersonController: invalid camera or target node");
            return nullptr;
        }

        ThirdPersonController *controller = new ThirdPersonController(cameraNode, targetNode);
        Info("ThirdPersonController created");
        return controller;
    }

    void thirdperson_dtor(Interpreter *vm, void *data)
    {
        delete static_cast<ThirdPersonController *>(data);
        Info("ThirdPersonController destroyed");
    }

    int thirdperson_update(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3) return 0;
        ThirdPersonController *controller = static_cast<ThirdPersonController *>(data);

        float deltaTime = (float)args[0].asNumber();
        int mouseDeltaX = (int)args[1].asNumber();
        int mouseDeltaY = (int)args[2].asNumber();

        controller->update(deltaTime, mouseDeltaX, mouseDeltaY);
        return 0;
    }

   

    int thirdperson_setTarget(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        NativeClassInstance *targetNodeInstance = args[0].asNativeClassInstance();
        Ogre::SceneNode *targetNode = static_cast<Ogre::SceneNode *>(targetNodeInstance->userData);

        if (targetNode)
        {
            static_cast<ThirdPersonController *>(data)->setTarget(targetNode);
        }
        return 0;
    }

    int thirdperson_setOffset(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3) return 0;
        ThirdPersonController *controller = static_cast<ThirdPersonController *>(data);

        float x = (float)args[0].asNumber();
        float y = (float)args[1].asNumber();
        float z = (float)args[2].asNumber();

        controller->setOffset(x, y, z);
        return 0;
    }

    int thirdperson_setOrientationOffset(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3) return 0;
        ThirdPersonController *controller = static_cast<ThirdPersonController *>(data);

        float x = (float)args[0].asNumber();
        float y = (float)args[1].asNumber();
        float z = (float)args[2].asNumber();

        controller->setOrientationOffset(x, y, z);
        return 0;
    }

    

    Value thirdperson_getEnabled(Interpreter *vm, void *data)
    {
        return vm->makeBool(static_cast<ThirdPersonController *>(data)->isEnabled());
    }

    void thirdperson_setEnabled(Interpreter *vm, void *data, Value value)
    {
        static_cast<ThirdPersonController *>(data)->setEnabled(value.asBool());
    }

    Value thirdperson_getSpringDamping(Interpreter *vm, void *data)
    {
        return vm->makeFloat(static_cast<ThirdPersonController *>(data)->getSpringDamping());
    }
    void thirdperson_setSpringDamping(Interpreter *vm, void *data, Value value)
    {
        static_cast<ThirdPersonController *>(data)->setSpringDamping((float)value.asNumber());
    }

    Value thirdperson_getSpringStiffness(Interpreter *vm, void *data)
    {
        return vm->makeFloat(static_cast<ThirdPersonController *>(data)->getSpringStiffness());
    }
    void thirdperson_setSpringStiffness(Interpreter *vm, void *data, Value value)
    {
        static_cast<ThirdPersonController *>(data)->setSpringStiffness((float)value.asNumber());
    }

     

    void registerAll(Interpreter &vm)
    {
        NativeClassDef *thirdPerson = vm.registerNativeClass(
            "ThirdPersonController",
            thirdperson_ctor,
            thirdperson_dtor,
            2,  // properties count
            false
        );

        // Properties
        vm.addNativeProperty(thirdPerson, "enabled", thirdperson_getEnabled, thirdperson_setEnabled);
    

        // Methods
        vm.addNativeMethod(thirdPerson, "update", thirdperson_update);
 
        vm.addNativeMethod(thirdPerson, "setTarget", thirdperson_setTarget);
        vm.addNativeMethod(thirdPerson, "setOffset", thirdperson_setOffset);
        vm.addNativeMethod(thirdPerson, "setOrientationOffset", thirdperson_setOrientationOffset);
        vm.addNativeProperty(thirdPerson, "springDamping", thirdperson_getSpringDamping, thirdperson_setSpringDamping);
        vm.addNativeProperty(thirdPerson, "springStiffness", thirdperson_getSpringStiffness, thirdperson_setSpringStiffness);
    }
}
// ============== REGISTER ALL CAMERA CONTROLLERS ==============

namespace CameraControllerBindings
{
    void registerAll(Interpreter &vm)
    {
        FPSControllerBindings::registerAll(vm);
        OrbitControllerBindings::registerAll(vm);
        FreeCameraControllerBindings::registerAll(vm);
        ThirdPersonControllerBindings::registerAll(vm);

        Info("Camera Controllers registered");
    }
}
