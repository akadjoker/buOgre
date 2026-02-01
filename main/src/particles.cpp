#include "bindings.hpp"

// ============== OGRE PARTICLE SYSTEM BINDINGS ==============

namespace OgreParticleSystemBindings
{
    // createParticleSystem(scene, "name", "templateName") - função global
    int createParticleSystem(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 3)
        {
            Error("createParticleSystem: requires scene, name and template");
            return 0;
        }

        NativeClassInstance *sceneInstance = args[0].asNativeClassInstance();
        Ogre::SceneManager *scene = static_cast<Ogre::SceneManager *>(sceneInstance->userData);

        if (!scene)
        {
            Error("createParticleSystem: invalid scene");
            return 0;
        }

        const char *name = args[1].asStringChars();
        const char *templateName = args[2].asStringChars();

        Ogre::ParticleSystem *particleSystem = scene->createParticleSystem(name, templateName);

        if (!particleSystem)
        {
            Error("createParticleSystem: failed to create particle system '%s' with template '%s'", name, templateName);
            return 0;
        }

        // Get the ParticleSystem NativeClassDef
        NativeClassDef *particleClass = nullptr;
        if (!vm->tryGetNativeClassDef("ParticleSystem", &particleClass))
        {
            Error("ParticleSystem class not found in VM");
            return 0;
        }

        // Create NativeClassInstance
        Value particleValue = vm->makeNativeClassInstance(false);
        NativeClassInstance *instance = particleValue.asNativeClassInstance();
        instance->klass = particleClass;
        instance->userData = (void *)particleSystem;

        vm->push(particleValue);
        return 1;
    }

    // ========== PARTICLE SYSTEM METHODS ==========

    // setEmitting(bool)
    int particle_setEmitting(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        Ogre::ParticleSystem *ps = static_cast<Ogre::ParticleSystem *>(data);
        if (ps) ps->setEmitting(args[0].asBool());
        return 0;
    }

    // clear()
    int particle_clear(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::ParticleSystem *ps = static_cast<Ogre::ParticleSystem *>(data);
        if (ps) ps->clear();
        return 0;
    }

    // setSpeedFactor(factor)
    int particle_setSpeedFactor(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        Ogre::ParticleSystem *ps = static_cast<Ogre::ParticleSystem *>(data);
        if (ps) ps->setSpeedFactor((float)args[0].asNumber());
        return 0;
    }

    // fastForward(time)
    int particle_fastForward(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        Ogre::ParticleSystem *ps = static_cast<Ogre::ParticleSystem *>(data);
        if (ps) ps->fastForward((float)args[0].asNumber());
        return 0;
    }

    // getNumParticles()
    int particle_getNumParticles(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::ParticleSystem *ps = static_cast<Ogre::ParticleSystem *>(data);
        if (ps)
        {
            vm->pushInt(ps->getNumParticles());
            return 1;
        }
        vm->pushInt(0);
        return 1;
    }

    // ========== PROPERTIES ==========

    Value particle_getEmitting(Interpreter *vm, void *data)
    {
        Ogre::ParticleSystem *ps = static_cast<Ogre::ParticleSystem *>(data);
        return vm->makeBool(ps ? ps->getNumEmitters() > 0 : false);
    }

    void particle_setEmittingProp(Interpreter *vm, void *data, Value value)
    {
        Ogre::ParticleSystem *ps = static_cast<Ogre::ParticleSystem *>(data);
        if (ps) ps->setEmitting(value.asBool());
    }

    Value particle_getSpeedFactor(Interpreter *vm, void *data)
    {
        Ogre::ParticleSystem *ps = static_cast<Ogre::ParticleSystem *>(data);
        return vm->makeFloat(ps ? ps->getSpeedFactor() : 1.0f);
    }

    void particle_setSpeedFactorProp(Interpreter *vm, void *data, Value value)
    {
        Ogre::ParticleSystem *ps = static_cast<Ogre::ParticleSystem *>(data);
        if (ps) ps->setSpeedFactor((float)value.asNumber());
    }

    void registerAll(Interpreter &vm)
    {
        // Register ParticleSystem class
        NativeClassDef *particle = vm.registerNativeClass(
            "ParticleSystem",
            nullptr,  // No constructor - use createParticleSystem()
            nullptr,  // No destructor - Ogre owns it
            2,        // properties: emitting, speedFactor
            false
        );

        // Properties
        vm.addNativeProperty(particle, "emitting", particle_getEmitting, particle_setEmittingProp);
        vm.addNativeProperty(particle, "speedFactor", particle_getSpeedFactor, particle_setSpeedFactorProp);

        // Methods
        vm.addNativeMethod(particle, "setEmitting", particle_setEmitting);
        vm.addNativeMethod(particle, "clear", particle_clear);
        vm.addNativeMethod(particle, "setSpeedFactor", particle_setSpeedFactor);
        vm.addNativeMethod(particle, "fastForward", particle_fastForward);
        vm.addNativeMethod(particle, "getNumParticles", particle_getNumParticles);

        // Global functions
        vm.registerNative("createParticleSystem", createParticleSystem, 3);

        Info("ParticleSystem bindings registered");
    }

} // namespace OgreParticleSystemBindings

// ============== OGRE RIBBON TRAIL BINDINGS ==============

namespace OgreRibbonTrailBindings
{
    // createRibbonTrail(scene, "name") - função global
    int createRibbonTrail(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 2)
        {
            Error("createRibbonTrail: requires scene and name");
            return 0;
        }

        NativeClassInstance *sceneInstance = args[0].asNativeClassInstance();
        Ogre::SceneManager *scene = static_cast<Ogre::SceneManager *>(sceneInstance->userData);

        if (!scene)
        {
            Error("createRibbonTrail: invalid scene");
            return 0;
        }

        const char *name = args[1].asStringChars();

        Ogre::RibbonTrail *ribbonTrail = scene->createRibbonTrail(name);

        if (!ribbonTrail)
        {
            Error("createRibbonTrail: failed to create ribbon trail '%s'", name);
            return 0;
        }

        // Get the RibbonTrail NativeClassDef
        NativeClassDef *ribbonClass = nullptr;
        if (!vm->tryGetNativeClassDef("RibbonTrail", &ribbonClass))
        {
            Error("RibbonTrail class not found in VM");
            return 0;
        }

        // Create NativeClassInstance
        Value ribbonValue = vm->makeNativeClassInstance(false);
        NativeClassInstance *instance = ribbonValue.asNativeClassInstance();
        instance->klass = ribbonClass;
        instance->userData = (void *)ribbonTrail;

        vm->push(ribbonValue);
        return 1;
    }

    // ========== RIBBON TRAIL METHODS ==========

    // addNode(sceneNode)
    int ribbon_addNode(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::RibbonTrail *ribbon = static_cast<Ogre::RibbonTrail *>(data);
        NativeClassInstance *nodeInstance = args[0].asNativeClassInstance();
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(nodeInstance->userData);

        if (ribbon && node)
        {
            ribbon->addNode(node);
        }
        return 0;
    }

    // removeNode(sceneNode)
    int ribbon_removeNode(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::RibbonTrail *ribbon = static_cast<Ogre::RibbonTrail *>(data);
        NativeClassInstance *nodeInstance = args[0].asNativeClassInstance();
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(nodeInstance->userData);

        if (ribbon && node)
        {
            ribbon->removeNode(node);
        }
        return 0;
    }

    // clearChain(index)
    int ribbon_clearChain(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::RibbonTrail *ribbon = static_cast<Ogre::RibbonTrail *>(data);
        if (ribbon)
        {
            ribbon->clearChain((size_t)args[0].asNumber());
        }
        return 0;
    }

    // clearAllChains()
    int ribbon_clearAllChains(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::RibbonTrail *ribbon = static_cast<Ogre::RibbonTrail *>(data);
        if (ribbon)
        {
            ribbon->clearAllChains();
        }
        return 0;
    }

    // setMaterialName(materialName)
    int ribbon_setMaterialName(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::RibbonTrail *ribbon = static_cast<Ogre::RibbonTrail *>(data);
        if (ribbon)
        {
            ribbon->setMaterialName(args[0].asStringChars());
        }
        return 0;
    }

    // setTrailLength(length)
    int ribbon_setTrailLength(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::RibbonTrail *ribbon = static_cast<Ogre::RibbonTrail *>(data);
        if (ribbon)
        {
            ribbon->setTrailLength((float)args[0].asNumber());
        }
        return 0;
    }

    // setMaxChainElements(count)
    int ribbon_setMaxChainElements(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::RibbonTrail *ribbon = static_cast<Ogre::RibbonTrail *>(data);
        if (ribbon)
        {
            ribbon->setMaxChainElements((size_t)args[0].asNumber());
        }
        return 0;
    }

    // setInitialColour(index, r, g, b, a)
    int ribbon_setInitialColour(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 5) return 0;

        Ogre::RibbonTrail *ribbon = static_cast<Ogre::RibbonTrail *>(data);
        if (ribbon)
        {
            size_t index = (size_t)args[0].asNumber();
            Ogre::ColourValue color(
                (float)args[1].asNumber(),
                (float)args[2].asNumber(),
                (float)args[3].asNumber(),
                (float)args[4].asNumber()
            );
            ribbon->setInitialColour(index, color);
        }
        return 0;
    }

    // setColourChange(index, r, g, b, a)
    int ribbon_setColourChange(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 5) return 0;

        Ogre::RibbonTrail *ribbon = static_cast<Ogre::RibbonTrail *>(data);
        if (ribbon)
        {
            size_t index = (size_t)args[0].asNumber();
            Ogre::ColourValue colorChange(
                (float)args[1].asNumber(),
                (float)args[2].asNumber(),
                (float)args[3].asNumber(),
                (float)args[4].asNumber()
            );
            ribbon->setColourChange(index, colorChange);
        }
        return 0;
    }

    // setInitialWidth(index, width)
    int ribbon_setInitialWidth(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 2) return 0;

        Ogre::RibbonTrail *ribbon = static_cast<Ogre::RibbonTrail *>(data);
        if (ribbon)
        {
            size_t index = (size_t)args[0].asNumber();
            float width = (float)args[1].asNumber();
            ribbon->setInitialWidth(index, width);
        }
        return 0;
    }

    // ========== PROPERTIES ==========

    Value ribbon_getTrailLength(Interpreter *vm, void *data)
    {
        Ogre::RibbonTrail *ribbon = static_cast<Ogre::RibbonTrail *>(data);
        return vm->makeFloat(ribbon ? ribbon->getTrailLength() : 100.0f);
    }

    void ribbon_setTrailLengthProp(Interpreter *vm, void *data, Value value)
    {
        Ogre::RibbonTrail *ribbon = static_cast<Ogre::RibbonTrail *>(data);
        if (ribbon) ribbon->setTrailLength((float)value.asNumber());
    }

    void registerAll(Interpreter &vm)
    {
        // Register RibbonTrail class
        NativeClassDef *ribbon = vm.registerNativeClass(
            "RibbonTrail",
            nullptr,  // No constructor - use createRibbonTrail()
            nullptr,  // No destructor - Ogre owns it
            1,        // properties: trailLength
            false
        );

        // Properties
        vm.addNativeProperty(ribbon, "trailLength", ribbon_getTrailLength, ribbon_setTrailLengthProp);

        // Methods
        vm.addNativeMethod(ribbon, "addNode", ribbon_addNode);
        vm.addNativeMethod(ribbon, "removeNode", ribbon_removeNode);
        vm.addNativeMethod(ribbon, "clearChain", ribbon_clearChain);
        vm.addNativeMethod(ribbon, "clearAllChains", ribbon_clearAllChains);
        vm.addNativeMethod(ribbon, "setMaterialName", ribbon_setMaterialName);
        vm.addNativeMethod(ribbon, "setTrailLength", ribbon_setTrailLength);
        vm.addNativeMethod(ribbon, "setMaxChainElements", ribbon_setMaxChainElements);
        vm.addNativeMethod(ribbon, "setInitialColour", ribbon_setInitialColour);
        vm.addNativeMethod(ribbon, "setColourChange", ribbon_setColourChange);
        vm.addNativeMethod(ribbon, "setInitialWidth", ribbon_setInitialWidth);

        // Global functions
        vm.registerNative("createRibbonTrail", createRibbonTrail, 2);

        Info("RibbonTrail bindings registered");
    }

} // namespace OgreRibbonTrailBindings
