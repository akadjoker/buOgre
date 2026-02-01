#include "bindings.hpp"
#include <OgreSkeleton.h>
#include <OgreBone.h>
#include <OgreEntity.h>

// ============== OGRE SKELETON/BONE BINDINGS ==============

namespace OgreSkeletonBindings
{
    // getSkeleton(entity) - global function
    int getSkeleton(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            Error("getSkeleton: requires entity");
            return 0;
        }

        NativeClassInstance *entityInstance = args[0].asNativeClassInstance();
        Ogre::Entity *entity = static_cast<Ogre::Entity *>(entityInstance->userData);

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

    // getBone(skeleton, boneName) - global function
    int getBone(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 2)
        {
            Error("getBone: requires skeleton and bone name");
            vm->pushNil();
            return 1;
        }

        NativeClassInstance *skelInstance = args[0].asNativeClassInstance();
        Ogre::SkeletonInstance *skeleton = static_cast<Ogre::SkeletonInstance *>(skelInstance->userData);

        if (!skeleton)
        {
            vm->pushNil();
            return 1;
        }

        const char *boneName = args[1].asStringChars();

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

    // ========== SKELETON METHODS ==========

    // getNumBones() -> int
    int skeleton_getNumBones(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::SkeletonInstance *skeleton = static_cast<Ogre::SkeletonInstance *>(data);
        vm->pushInt(skeleton ? skeleton->getNumBones() : 0);
        return 1;
    }

    // reset()
    int skeleton_reset(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::SkeletonInstance *skeleton = static_cast<Ogre::SkeletonInstance *>(data);
        if (skeleton) skeleton->reset();
        return 0;
    }

    // setBindingPose()
    int skeleton_setBindingPose(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::SkeletonInstance *skeleton = static_cast<Ogre::SkeletonInstance *>(data);
        if (skeleton) skeleton->setBindingPose();
        return 0;
    }

    // ========== BONE METHODS ==========

    // setPosition(x, y, z)
    int bone_setPosition(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3) return 0;

        Ogre::Bone *bone = static_cast<Ogre::Bone *>(data);
        if (bone)
        {
            bone->setPosition(
                (float)args[0].asNumber(),
                (float)args[1].asNumber(),
                (float)args[2].asNumber()
            );
        }

        return 0;
    }

    // setOrientation(w, x, y, z)
    int bone_setOrientation(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 4) return 0;

        Ogre::Bone *bone = static_cast<Ogre::Bone *>(data);
        if (bone)
        {
            bone->setOrientation(Ogre::Quaternion(
                (float)args[0].asNumber(),
                (float)args[1].asNumber(),
                (float)args[2].asNumber(),
                (float)args[3].asNumber()
            ));
        }

        return 0;
    }

    // setScale(x, y, z)
    int bone_setScale(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3) return 0;

        Ogre::Bone *bone = static_cast<Ogre::Bone *>(data);
        if (bone)
        {
            bone->setScale(
                (float)args[0].asNumber(),
                (float)args[1].asNumber(),
                (float)args[2].asNumber()
            );
        }

        return 0;
    }

    // getPosition() -> x, y, z
    int bone_getPosition(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::Bone *bone = static_cast<Ogre::Bone *>(data);
        if (bone)
        {
            Ogre::Vector3 pos = bone->getPosition();
            vm->pushFloat(pos.x);
            vm->pushFloat(pos.y);
            vm->pushFloat(pos.z);
            return 3;
        }

        vm->pushFloat(0.0f);
        vm->pushFloat(0.0f);
        vm->pushFloat(0.0f);
        return 3;
    }

    // yaw(angle)
    int bone_yaw(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::Bone *bone = static_cast<Ogre::Bone *>(data);
        if (bone)
        {
            bone->yaw(Ogre::Degree((float)args[0].asNumber()));
        }

        return 0;
    }

    // pitch(angle)
    int bone_pitch(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::Bone *bone = static_cast<Ogre::Bone *>(data);
        if (bone)
        {
            bone->pitch(Ogre::Degree((float)args[0].asNumber()));
        }

        return 0;
    }

    // roll(angle)
    int bone_roll(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::Bone *bone = static_cast<Ogre::Bone *>(data);
        if (bone)
        {
            bone->roll(Ogre::Degree((float)args[0].asNumber()));
        }

        return 0;
    }

    // setManuallyControlled(bool)
    int bone_setManuallyControlled(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::Bone *bone = static_cast<Ogre::Bone *>(data);
        if (bone)
        {
            bone->setManuallyControlled(args[0].asBool());
        }

        return 0;
    }

    // resetToInitialState()
    int bone_resetToInitialState(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::Bone *bone = static_cast<Ogre::Bone *>(data);
        if (bone) bone->resetToInitialState();
        return 0;
    }

    // ========== ENTITY BONE ATTACHMENT ==========

    // attachObjectToBone(entity, boneName, object)
    int attachObjectToBone(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 3)
        {
            Error("attachObjectToBone: requires entity, boneName, and object");
            return 0;
        }

        NativeClassInstance *entityInstance = args[0].asNativeClassInstance();
        Ogre::Entity *entity = static_cast<Ogre::Entity *>(entityInstance->userData);

        if (!entity)
        {
            Error("attachObjectToBone: invalid entity");
            return 0;
        }

        const char *boneName = args[1].asStringChars();

        NativeClassInstance *objectInstance = args[2].asNativeClassInstance();
        Ogre::MovableObject *object = static_cast<Ogre::MovableObject *>(objectInstance->userData);

        if (!object)
        {
            Error("attachObjectToBone: invalid object");
            return 0;
        }

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
    int detachObjectFromBone(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 2)
        {
            Error("detachObjectFromBone: requires entity and object");
            return 0;
        }

        NativeClassInstance *entityInstance = args[0].asNativeClassInstance();
        Ogre::Entity *entity = static_cast<Ogre::Entity *>(entityInstance->userData);

        NativeClassInstance *objectInstance = args[1].asNativeClassInstance();
        Ogre::MovableObject *object = static_cast<Ogre::MovableObject *>(objectInstance->userData);

        if (entity && object)
        {
            entity->detachObjectFromBone(object);
        }

        return 0;
    }

    void registerAll(Interpreter &vm)
    {
        // Register Skeleton class
        NativeClassDef *skeleton = vm.registerNativeClass(
            "Skeleton",
            nullptr,
            nullptr,
            0,
            false
        );

        vm.addNativeMethod(skeleton, "getNumBones", skeleton_getNumBones);
        vm.addNativeMethod(skeleton, "reset", skeleton_reset);
        vm.addNativeMethod(skeleton, "setBindingPose", skeleton_setBindingPose);

        // Register Bone class
        NativeClassDef *bone = vm.registerNativeClass(
            "Bone",
            nullptr,
            nullptr,
            0,
            false
        );

        vm.addNativeMethod(bone, "setPosition", bone_setPosition);
        vm.addNativeMethod(bone, "setOrientation", bone_setOrientation);
        vm.addNativeMethod(bone, "setScale", bone_setScale);
        vm.addNativeMethod(bone, "getPosition", bone_getPosition);
        vm.addNativeMethod(bone, "yaw", bone_yaw);
        vm.addNativeMethod(bone, "pitch", bone_pitch);
        vm.addNativeMethod(bone, "roll", bone_roll);
        vm.addNativeMethod(bone, "setManuallyControlled", bone_setManuallyControlled);
        vm.addNativeMethod(bone, "resetToInitialState", bone_resetToInitialState);

        // Global functions
        vm.registerNative("getSkeleton", getSkeleton, 1);
        vm.registerNative("getBone", getBone, 2);
        vm.registerNative("attachObjectToBone", attachObjectToBone, 3);
        vm.registerNative("detachObjectFromBone", detachObjectFromBone, 2);

        Info("Skeleton/Bone bindings registered");
    }

} // namespace OgreSkeletonBindings
