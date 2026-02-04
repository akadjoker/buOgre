#include "bindings.hpp"
#include <OgreSkeleton.h>
#include <OgreBone.h>
#include <OgreEntity.h>

// ============== OGRE SKELETON/BONE BINDINGS ==============

namespace OgreSkeletonBindings
{
    

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
    }

} // namespace OgreSkeletonBindings
