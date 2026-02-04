#include "bindings.hpp"

namespace OgreEntityBindings
{
    void *entity_ctor(Interpreter *vm, int argCount, Value *args)
    {

        NativeClassInstance *sceneInstance = args[0].asNativeClassInstance();
        Ogre::SceneManager *scene = static_cast<Ogre::SceneManager *>(sceneInstance->userData);
        if (!scene)
        {
            Error("Entity constructor: no scene manager available");
            return nullptr;
        }
        if (argCount < 2)
        {
            Error("Entity constructor: requires name argument");
            return nullptr;
        }

        if (argCount < 1)
        {
            Error("Entity constructor requires a mesh name argument");
            return nullptr;
        }

        const char *meshName = args[1].asStringChars();
        Ogre::Entity *entity = scene->createEntity(meshName);
        Info("Entity constructed with mesh: %s", meshName);
        return entity;
    }

    // // No destructor - Ogre owns Entities
    void entity_dtor(Interpreter *vm, void *data)
    {
        // Info("Entity ref released");
    }

    // Property: castShadows
    Value entity_getCastShadows(Interpreter *vm, void *data)
    {
        return vm->makeBool(static_cast<Ogre::Entity *>(data)->getCastShadows());
    }

    void entity_setCastShadows(Interpreter *vm, void *data, Value value)
    {
        static_cast<Ogre::Entity *>(data)->setCastShadows(value.asBool());
    }

    // Property: visible
    Value entity_getVisible(Interpreter *vm, void *data)
    {
        return vm->makeBool(static_cast<Ogre::Entity *>(data)->getVisible());
    }

    void entity_setVisible(Interpreter *vm, void *data, Value value)
    {
        static_cast<Ogre::Entity *>(data)->setVisible(value.asBool());
    }

    // Method: setMaterialName(name)
    int entity_setMaterial(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
            return 0;
        Ogre::Entity *ent = static_cast<Ogre::Entity *>(data);
        const char *matName = args[0].asStringChars();
        ent->setMaterialName(matName);
        return 0;
    }

    int entity_setCastShadows(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
            return 0;

        Ogre::Entity *ent = static_cast<Ogre::Entity *>(data);

        if (ent)
            ent->setCastShadows(args[0].asBool());
        return 0;
    }

    int entity_getAnimationState(Interpreter *vm, void *data, int argCount, Value *args)
    {

        if (argCount < 1)
        {
            Error("getAnimationState: requires animation name argument"); 
            vm->pushNil();
            return 1;
        }
        Ogre::Entity *entity = static_cast<Ogre::Entity *>(data);
        const char *animName = args[0].asStringChars();

        // Check if animation exists
        if (!entity->hasAnimationState(animName))
        {
            Error("getAnimationState: animation '%s' not found", animName);
            vm->pushNil();
            return 1;
        }

        Ogre::AnimationState *animState = entity->getAnimationState(animName);

        // Get the AnimationState NativeClassDef
        NativeClassDef *animStateClass = nullptr;
        if (!vm->tryGetNativeClassDef("AnimationState", &animStateClass))
        {
            Error("AnimationState class not found in VM");
            vm->pushNil();
            return 1;
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
    int entity_hasAnimation(Interpreter *vm, void *data, int argCount, Value *args)
    {

        Ogre::Entity *entity = static_cast<Ogre::Entity *>(data);
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

      // ========== ENTITY BONE ATTACHMENT ==========

    // attachObjectToBone(entity, boneName, object)
    int entity_attachObjectToBone(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::Entity *entity = static_cast<Ogre::Entity *>(data);
        if (!entity)
        {
            return 0;
        }

        if (argCount < 2)
        {
            return 0;
        }

        if (!entity->hasSkeleton())
        {
            Error("attachObjectToBone: Entity has no skeleton");
            return 0;
        }
        if (!args[0].isString())
        {
            Error("attachObjectToBone: First argument must be a bone name string");
            return 0;
        }   
        if ( !args[1].isNativeClassInstance() )
        {
            Error("attachObjectToBone: Argument is not a NativeClassInstance");
            return 0;
        }
        const char *boneName = args[0].asStringChars();

        NativeClassInstance *objectInstance = args[1].asNativeClassInstance();
        Ogre::MovableObject *object = static_cast<Ogre::MovableObject *>(objectInstance->userData);


 

        try
        {
            entity->attachObjectToBone(boneName, object);
            Info("Object attached to bone '%s'", boneName);
        }
        catch (Ogre::Exception &e)
        {
            Error("attachObjectToBone failed: %s", e.what());
        }

        return 0;
    }

    // detachObjectFromBone(entity, object)
    int entity_detachObjectFromBone(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::Entity *entity = static_cast<Ogre::Entity *>(data);
        if (!entity)
        {
            return 0;
        }
        if (argCount < 1)
        {
            return 0;
        }
        if (!entity->hasSkeleton())
        {
            Error("detachObjectFromBone: Entity has no skeleton");
            return 0;
        }
        if ( !args[0].isNativeClassInstance() )
        {
            Error("detachObjectFromBone: Argument is not a NativeClassInstance");
            return 0;
        }
        NativeClassInstance *objectInstance = args[0].asNativeClassInstance();
        Ogre::MovableObject *object = static_cast<Ogre::MovableObject *>(objectInstance->userData); 
        entity->detachObjectFromBone(object);


        return 0;
    }


    // getSkeleton(entity) - global function
    int entity_getSkeleton(Interpreter *vm,void *data, int argCount, Value *args)
    {
        Ogre::Entity *entity = static_cast<Ogre::Entity *>(data);

        if (!entity || !entity->hasSkeleton())
        {
            Error("getSkeleton: entity has no skeleton");
            vm->pushNil();
            return 1;
        }

        Ogre::SkeletonInstance *skeleton = entity->getSkeleton();
        NativeClassDef *skelClass = nullptr;
        if (!vm->tryGetNativeClassDef("Skeleton", &skelClass))
        {
            Error("Skeleton class not found in VM");
            vm->pushNil();
            return 1;
        }

        Value skelValue = vm->makeNativeClassInstance(false);
        NativeClassInstance *instance = skelValue.asNativeClassInstance();
        instance->klass = skelClass;
        instance->userData = (void *)skeleton;

        vm->push(skelValue);
        return 1;
    }

    // getBone(skeleton, boneName)  
    int entity_getBone(Interpreter *vm,void *data, int argCount, Value *args)
    { 

        
        Ogre::SkeletonInstance *skeleton = static_cast<Ogre::SkeletonInstance *>(data);

        if (!skeleton)
        {
            Error("getBone: invalid skeleton");
            vm->pushNil();
            return 1;
        }
        if (argCount < 1)
        {
            Error("getBone: requires bone name argument");
            vm->pushNil();
            return 1;
        }

        if (!args[1].isString())
        {
            Error("getBone: bone name must be a string");
            vm->pushNil();
            return 1;
        }

        const char *boneName = args[0].asStringChars();

        try
        {
            Ogre::Bone *bone = skeleton->getBone(boneName);

            NativeClassDef *boneClass = nullptr;
            if (!vm->tryGetNativeClassDef("Bone", &boneClass))
            {
                Error("Bone class not found in VM");
                vm->pushNil();
                return 1;
            }

            Value boneValue = vm->makeNativeClassInstance(false);
            NativeClassInstance *instance = boneValue.asNativeClassInstance();
            instance->klass = boneClass;
            instance->userData = (void *)bone;

            vm->push(boneValue);
            return 1;
        }
        catch (Ogre::Exception &e)
        {
            Error("getBone failed: %s", e.what());
            vm->pushNil();
            return 1;
        }
    }

    void registerAll(Interpreter &vm)
    {
        NativeClassDef *ent = vm.registerNativeClass(
            "Entity",
            entity_ctor,
            entity_dtor,
            2, // properties: castShadows, visible
            false);

        vm.addNativeProperty(ent, "castShadows", entity_getCastShadows, entity_setCastShadows);
        vm.addNativeProperty(ent, "visible", entity_getVisible, entity_setVisible);
        vm.addNativeMethod(ent, "setMaterial", entity_setMaterial);
        vm.addNativeMethod(ent, "setCastShadows", entity_setCastShadows);

        vm.addNativeMethod(ent, "getAnimationState", entity_getAnimationState);
        vm.addNativeMethod(ent, "hasAnimation", entity_hasAnimation);
        vm.addNativeMethod(ent, "attachObjectToBone", entity_attachObjectToBone);
        vm.addNativeMethod(ent, "detachObjectFromBone", entity_detachObjectFromBone);
        vm.addNativeMethod(ent, "getSkeleton", entity_getSkeleton);
        vm.addNativeMethod(ent, "getBone", entity_getBone);

        // Info("Entity bindings registered");
    }

} // namespace OgreEntityBindings
