#include "bindings.hpp"

 
namespace OgreCameraBindings
{
     

    // Constructor: Camera("name") - creates camera + node
    void *camera_ctor(Interpreter *vm, int argCount, Value *args)
    {
        NativeClassInstance *sceneInstance = args[0].asNativeClassInstance();
        Ogre::SceneManager *scene = static_cast<Ogre::SceneManager *>(sceneInstance->userData);
        if (!scene)
        {
            Error("Camera constructor: no scene manager available");
            return nullptr;
        }
        if (argCount < 2)
        {
            Error("Camera constructor: requires name argument");
            return nullptr;
        }
        

        const char *name = args[1].asStringChars();
        Ogre::Camera *cam = scene->createCamera(name);
        cam->setNearClipDistance(1.0f);
        cam->setAutoAspectRatio(true);

        

        Info("Camera '%s' created", name);
        return cam;
    }

    // void camera_dtor(Interpreter *vm, void *data) {}

    int camera_setAutoAspectRatio(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        Ogre::Camera *cam = static_cast<Ogre::Camera *>(data);
        if (cam) cam->setAutoAspectRatio(args[0].asBool());
        return 0;
    }
 

    // setFOV(degrees)
    int camera_setFOV(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::Camera *cam = static_cast<Ogre::Camera *>(data);
        if (cam)
        {
            float fovDegrees = (float)args[0].asNumber();
            cam->setFOVy(Ogre::Degree(fovDegrees));
        }
        
        return 0;
    }

    // setNearClip(distance)
    int camera_setNearClip(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        Ogre::Camera *cam = static_cast<Ogre::Camera *>(data);
        if (cam) cam->setNearClipDistance((float)args[0].asNumber());
        return 0;
    }

    // setFarClip(distance)
    int camera_setFarClip(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        Ogre::Camera *cam = static_cast<Ogre::Camera *>(data);
        if (cam) cam->setFarClipDistance((float)args[0].asNumber());
        return 0;
    }

   

    // getParentNode() - retorna o SceneNode que contém a câmara
    int camera_getParentNode(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::Camera *cam = static_cast<Ogre::Camera *>(data);
        if (!cam || !cam->getParentNode())
        {
            Error("Camera has no parent node");
            return 0;
        }

        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(cam->getParentNode());
        
        // Get the SceneNode NativeClassDef
        NativeClassDef *sceneNodeClass = nullptr;
        if (!vm->tryGetNativeClassDef("SceneNode", &sceneNodeClass))
        {
            Error("SceneNode class not found in VM");
            return 0;
        }
        
        // Create a NativeClassInstance of SceneNode
        Value nodeValue = vm->makeNativeClassInstance(false);
        NativeClassInstance *instance = nodeValue.asNativeClassInstance();
        instance->klass = sceneNodeClass;
        instance->userData = (void *)node;
        
        vm->push(nodeValue);
        return 1;
    }

    void registerAll(Interpreter &vm)
    {
        NativeClassDef *cam = vm.registerNativeClass(
            "Camera",
            camera_ctor,
            nullptr,  // destructor
            2,      // properties: posX, posY, posZ
            false
        );

 
         vm.addNativeMethod(cam, "setFOV", camera_setFOV);
         vm.addNativeMethod(cam, "setNearClip", camera_setNearClip);
         vm.addNativeMethod(cam, "setFarClip", camera_setFarClip);
        vm.addNativeMethod(cam, "setAutoAspectRatio", camera_setAutoAspectRatio);
 
        vm.addNativeMethod(cam, "getParentNode", camera_getParentNode);

        

        Info("Camera bindings registered");
    }

} // namespace OgreCameraBindings
