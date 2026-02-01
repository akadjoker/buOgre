#include "bindings.hpp"

// ============== OGRE ANIMATION STATE BINDINGS ==============

namespace OgreAnimationStateBindings
{
    // getAnimationState(entity, "animationName") - função global
    int getAnimationState(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 2)
        {
            Error("getAnimationState: requires entity and animation name");
            return 0;
        }

        NativeClassInstance *entityInstance = args[0].asNativeClassInstance();
        Ogre::Entity *entity = static_cast<Ogre::Entity *>(entityInstance->userData);

        if (!entity)
        {
            Error("getAnimationState: invalid entity");
            return 0;
        }

        const char *animName = args[1].asStringChars();

        // Check if animation exists
        if (!entity->hasAnimationState(animName))
        {
            Error("getAnimationState: animation '%s' not found", animName);
            return 0;
        }

        Ogre::AnimationState *animState = entity->getAnimationState(animName);

        // Get the AnimationState NativeClassDef
        NativeClassDef *animStateClass = nullptr;
        if (!vm->tryGetNativeClassDef("AnimationState", &animStateClass))
        {
            Error("AnimationState class not found in VM");
            return 0;
        }

        // Create a NativeClassInstance
        Value animValue = vm->makeNativeClassInstance(false);
        NativeClassInstance *instance = animValue.asNativeClassInstance();
        instance->klass = animStateClass;
        instance->userData = (void *)animState;

        vm->push(animValue);
        return 1;
    }

    // hasAnimation(entity, "animationName") - verifica se existe
    int hasAnimation(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 2)
        {
            vm->pushBool(false);
            return 1;
        }

        NativeClassInstance *entityInstance = args[0].asNativeClassInstance();
        Ogre::Entity *entity = static_cast<Ogre::Entity *>(entityInstance->userData);

        if (!entity)
        {
            vm->pushBool(false);
            return 1;
        }

        const char *animName = args[1].asStringChars();
        bool hasAnim = entity->hasAnimationState(animName);

        vm->pushBool(hasAnim);
        return 1;
    }

    // ========== ANIMATION STATE METHODS ==========

    // setEnabled(bool)
    int animState_setEnabled(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        Ogre::AnimationState *anim = static_cast<Ogre::AnimationState *>(data);
        if (anim) anim->setEnabled(args[0].asBool());
        return 0;
    }

    // setLoop(bool)
    int animState_setLoop(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        Ogre::AnimationState *anim = static_cast<Ogre::AnimationState *>(data);
        if (anim) anim->setLoop(args[0].asBool());
        return 0;
    }

    // addTime(deltaTime)
    int animState_addTime(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        Ogre::AnimationState *anim = static_cast<Ogre::AnimationState *>(data);
        if (anim) anim->addTime((float)args[0].asNumber());
        return 0;
    }

    // setTimePosition(time)
    int animState_setTimePosition(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        Ogre::AnimationState *anim = static_cast<Ogre::AnimationState *>(data);
        if (anim) anim->setTimePosition((float)args[0].asNumber());
        return 0;
    }

    // getTimePosition() -> float
    int animState_getTimePosition(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::AnimationState *anim = static_cast<Ogre::AnimationState *>(data);
        if (anim)
        {
            vm->pushFloat(anim->getTimePosition());
            return 1;
        }
        vm->pushFloat(0.0f);
        return 1;
    }

    // getLength() -> float
    int animState_getLength(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::AnimationState *anim = static_cast<Ogre::AnimationState *>(data);
        if (anim)
        {
            vm->pushFloat(anim->getLength());
            return 1;
        }
        vm->pushFloat(0.0f);
        return 1;
    }

    // hasEnded() -> bool
    int animState_hasEnded(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::AnimationState *anim = static_cast<Ogre::AnimationState *>(data);
        if (anim)
        {
            vm->pushBool(anim->hasEnded());
            return 1;
        }
        vm->pushBool(false);
        return 1;
    }

    // ========== PROPERTIES ==========

    Value animState_getEnabled(Interpreter *vm, void *data)
    {
        Ogre::AnimationState *anim = static_cast<Ogre::AnimationState *>(data);
        return vm->makeBool(anim ? anim->getEnabled() : false);
    }

    void animState_setEnabledProp(Interpreter *vm, void *data, Value value)
    {
        Ogre::AnimationState *anim = static_cast<Ogre::AnimationState *>(data);
        if (anim) anim->setEnabled(value.asBool());
    }

    Value animState_getLoop(Interpreter *vm, void *data)
    {
        Ogre::AnimationState *anim = static_cast<Ogre::AnimationState *>(data);
        return vm->makeBool(anim ? anim->getLoop() : false);
    }

    void animState_setLoopProp(Interpreter *vm, void *data, Value value)
    {
        Ogre::AnimationState *anim = static_cast<Ogre::AnimationState *>(data);
        if (anim) anim->setLoop(value.asBool());
    }

    Value animState_getWeight(Interpreter *vm, void *data)
    {
        Ogre::AnimationState *anim = static_cast<Ogre::AnimationState *>(data);
        return vm->makeFloat(anim ? anim->getWeight() : 0.0f);
    }

    void animState_setWeight(Interpreter *vm, void *data, Value value)
    {
        Ogre::AnimationState *anim = static_cast<Ogre::AnimationState *>(data);
        if (anim) anim->setWeight((float)value.asNumber());
    }

    void registerAll(Interpreter &vm)
    {
        // Register AnimationState class
        NativeClassDef *animState = vm.registerNativeClass(
            "AnimationState",
            nullptr,  // No constructor - obtained via getAnimationState()
            nullptr,  // No destructor - Ogre owns it
            3,        // properties: enabled, loop, weight
            false
        );

        // Properties
        vm.addNativeProperty(animState, "enabled", animState_getEnabled, animState_setEnabledProp);
        vm.addNativeProperty(animState, "loop", animState_getLoop, animState_setLoopProp);
        vm.addNativeProperty(animState, "weight", animState_getWeight, animState_setWeight);

        // Methods
        vm.addNativeMethod(animState, "setEnabled", animState_setEnabled);
        vm.addNativeMethod(animState, "setLoop", animState_setLoop);
        vm.addNativeMethod(animState, "addTime", animState_addTime);
        vm.addNativeMethod(animState, "setTimePosition", animState_setTimePosition);
        vm.addNativeMethod(animState, "getTimePosition", animState_getTimePosition);
        vm.addNativeMethod(animState, "getLength", animState_getLength);
        vm.addNativeMethod(animState, "hasEnded", animState_hasEnded);

        // Global functions
        vm.registerNative("getAnimationState", getAnimationState, 2);
        vm.registerNative("hasAnimation", hasAnimation, 2);

        Info("AnimationState bindings registered");
    }

} // namespace OgreAnimationStateBindings
