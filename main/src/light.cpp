#include "bindings.hpp"

namespace OgreLightBindings
{
    // Constructor: Light(scene, name, type?)
    // type: 0 = point (default), 1 = directional, 2 = spotlight
    void *light_ctor(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 2)
        {
            Error("Light constructor requires scene and name arguments");
            return nullptr;
        }

        NativeClassInstance *sceneInstance = args[0].asNativeClassInstance();
        Ogre::SceneManager *scene = static_cast<Ogre::SceneManager *>(sceneInstance->userData);
        if (!scene)
        {
            Error("Light: invalid scene manager");
            return nullptr;
        }

        const char *name = args[1].asStringChars();
        
        // Create light
        Ogre::Light *light = scene->createLight(name);
       
        // Set type if provided (0=point, 1=directional, 2=spotlight)
        if (argCount >= 3)
        {
            int t = (int)args[2].asNumber();
            if (t == 1) light->setType(Ogre::Light::LT_DIRECTIONAL);
            else if (t == 2) light->setType(Ogre::Light::LT_SPOTLIGHT);
            else light->setType(Ogre::Light::LT_POINT);
        }

        Info("Light '%s' created (type: %d)", name, light->getType());
        return light;
    }

    void light_dtor(Interpreter *vm, void *data)
    {
        // Ogre owns lights, we don't delete
    }

    // Properties
    Value light_getVisible(Interpreter *vm, void *data)
    {
        return vm->makeBool(static_cast<Ogre::Light *>(data)->isVisible());
    }

    void light_setVisible(Interpreter *vm, void *data, Value value)
    {
        static_cast<Ogre::Light *>(data)->setVisible(value.asBool());
    }

  
    int light_setDiffuse(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3) return 0;
        Ogre::Light *light = static_cast<Ogre::Light *>(data);
        light->setDiffuseColour(Ogre::ColourValue(
            (float)args[0].asNumber(),
            (float)args[1].asNumber(),
            (float)args[2].asNumber()));
        return 0;
    }

    int light_setSpecular(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3) return 0;
        Ogre::Light *light = static_cast<Ogre::Light *>(data);
        light->setSpecularColour(Ogre::ColourValue(
            (float)args[0].asNumber(),
            (float)args[1].asNumber(),
            (float)args[2].asNumber()));
        return 0;
    }
 

    // setAttenuation(range, constant, linear, quadratic)
    int light_setAttenuation(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 4) return 0;
        Ogre::Light *light = static_cast<Ogre::Light *>(data);
        light->setAttenuation(
            (float)args[0].asNumber(),
            (float)args[1].asNumber(),
            (float)args[2].asNumber(),
            (float)args[3].asNumber());
        return 0;
    }

    // setPowerScale(power)
    int light_setPowerScale(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        Ogre::Light *light = static_cast<Ogre::Light *>(data);
        light->setPowerScale((float)args[0].asNumber());
        return 0;
    }

    // setType(type) - 0=point, 1=directional, 2=spotlight
    int light_setType(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        Ogre::Light *light = static_cast<Ogre::Light *>(data);
        int type = (int)args[0].asNumber();
        if (type == 1) light->setType(Ogre::Light::LT_DIRECTIONAL);
        else if (type == 2) light->setType(Ogre::Light::LT_SPOTLIGHT);
        else light->setType(Ogre::Light::LT_POINT);
        return 0;
    }

    // setSpotlightRange(innerAngle, outerAngle, falloff)
    // Angles in degrees
    int light_setSpotlightRange(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 2) return 0;
        Ogre::Light *light = static_cast<Ogre::Light *>(data);
        float inner = (float)args[0].asNumber();
        float outer = (float)args[1].asNumber();
        float falloff = argCount >= 3 ? (float)args[2].asNumber() : 2.0f;
        
        light->setSpotlightRange(
            Ogre::Radian(Ogre::Degree(inner)),
            Ogre::Radian(Ogre::Degree(outer)),
            falloff);
        return 0;
    }

    void registerAll(Interpreter &vm)
    {
        NativeClassDef *lt = vm.registerNativeClass(
            "Light",
            light_ctor,
            light_dtor,
            3,      // properties: visible
            false
        );

        vm.addNativeProperty(lt, "visible", light_getVisible, light_setVisible);

        // Methods
 
        vm.addNativeMethod(lt, "setDiffuse", light_setDiffuse);
        vm.addNativeMethod(lt, "setSpecular", light_setSpecular);
        vm.addNativeMethod(lt, "setAttenuation", light_setAttenuation);
        vm.addNativeMethod(lt, "setPowerScale", light_setPowerScale);
        vm.addNativeMethod(lt, "setType", light_setType);
        vm.addNativeMethod(lt, "setSpotlightRange", light_setSpotlightRange);

        Info("Light bindings registered");
    }

} // namespace OgreLightBindings


 