#include "bindings.hpp"

 

// ============== OGRE VECTOR3 BINDINGS ==============

namespace OgreVector3Bindings
{
    // Constructor: Vector3(x, y, z)
    void *vector3_ctor(Interpreter *vm, int argCount, Value *args)
    {
        Ogre::Vector3 *vec = new Ogre::Vector3();

        if (argCount >= 3)
        {
            vec->x = (float)args[0].asNumber();
            vec->y = (float)args[1].asNumber();
            vec->z = (float)args[2].asNumber();
        }
        else if (argCount == 1)
        {
            float val = (float)args[0].asNumber();
            vec->x = val;
            vec->y = val;
            vec->z = val;
        }
        else
        {
            vec->x = 0.0f;
            vec->y = 0.0f;
            vec->z = 0.0f;
        }

        Info("Vector3 constructed: (%f, %f, %f)", vec->x, vec->y, vec->z);
        return vec;
    }

    // Destructor
    void vector3_dtor(Interpreter *vm, void *data)
    {
       // Info("Vector3 destructed");
        delete static_cast<Ogre::Vector3 *>(data);
    }

    // Property getters
    Value vector3_getX(Interpreter *vm, void *data)
    {
        return vm->makeFloat(static_cast<Ogre::Vector3 *>(data)->x);
    }

    Value vector3_getY(Interpreter *vm, void *data)
    {
        return vm->makeFloat(static_cast<Ogre::Vector3 *>(data)->y);
    }

    Value vector3_getZ(Interpreter *vm, void *data)
    {
        return vm->makeFloat(static_cast<Ogre::Vector3 *>(data)->z);
    }

    // Property setters
    void vector3_setX(Interpreter *vm, void *data, Value value)
    {
        static_cast<Ogre::Vector3 *>(data)->x = (float)value.asNumber();
    }

    void vector3_setY(Interpreter *vm, void *data, Value value)
    {
        static_cast<Ogre::Vector3 *>(data)->y = (float)value.asNumber();
    }

    void vector3_setZ(Interpreter *vm, void *data, Value value)
    {
        static_cast<Ogre::Vector3 *>(data)->z = (float)value.asNumber();
    }

    // Methods
    int vector3_length(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::Vector3 *vec = static_cast<Ogre::Vector3 *>(data);
        float len = vec->length();
        vm->pushFloat(len);
        return 1;
    }

    int vector3_normalise(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::Vector3 *vec = static_cast<Ogre::Vector3 *>(data);
        vec->normalise();
        return 0;
    }

    int vector3_dot(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
            return 0;

        Ogre::Vector3 *self = static_cast<Ogre::Vector3 *>(data);
        Ogre::Vector3 *other = static_cast<Ogre::Vector3 *>(args[0].asPointer());

        if (other == nullptr)
            return 0;

        float result = self->dotProduct(*other);
        vm->pushFloat(result);
        return 1;
    }

    int vector3_cross(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
            return 0;

        Ogre::Vector3 *self = static_cast<Ogre::Vector3 *>(data);
        Ogre::Vector3 *other = static_cast<Ogre::Vector3 *>(args[0].asPointer());

        if (other == nullptr)
            return 0;

        Ogre::Vector3 result = self->crossProduct(*other);
        Ogre::Vector3 *newVec = new Ogre::Vector3(result);
        vm->pushPointer((void *)newVec);
        return 1;
    }

    int vector3_add(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
            return 0;

        Ogre::Vector3 *self = static_cast<Ogre::Vector3 *>(data);
        Ogre::Vector3 *other = static_cast<Ogre::Vector3 *>(args[0].asPointer());

        if (other == nullptr)
            return 0;

        *self = *self + *other;
        return 0;
    }

    int vector3_scale(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
            return 0;

        Ogre::Vector3 *self = static_cast<Ogre::Vector3 *>(data);
        float scale = (float)args[0].asNumber();

        *self = *self * scale;
        return 0;
    }

    void registerAll(Interpreter &vm)
    {
        NativeClassDef *vec3 = vm.registerNativeClass(
            "Vector3",
            vector3_ctor,
            vector3_dtor,
            3,      // number of properties
            false   // GC collects
        );

        // Add properties
        vm.addNativeProperty(vec3, "x", vector3_getX, vector3_setX);
        vm.addNativeProperty(vec3, "y", vector3_getY, vector3_setY);
        vm.addNativeProperty(vec3, "z", vector3_getZ, vector3_setZ);

        // Add methods
        vm.addNativeMethod(vec3, "length", vector3_length);
        vm.addNativeMethod(vec3, "normalise", vector3_normalise);
        vm.addNativeMethod(vec3, "dot", vector3_dot);
        vm.addNativeMethod(vec3, "cross", vector3_cross);
        vm.addNativeMethod(vec3, "add", vector3_add);
        vm.addNativeMethod(vec3, "scale", vector3_scale);

        Info("Vector3 bindings registered");
    }

} // namespace OgreVector3Bindings

// ============== OGRE QUATERNION BINDINGS ==============

namespace OgreQuaternionBindings
{
    // Constructor: Quaternion(w, x, y, z)
    void *quat_ctor(Interpreter *vm, int argCount, Value *args)
    {
        Ogre::Quaternion *quat = new Ogre::Quaternion();

        if (argCount >= 4)
        {
            quat->w = (float)args[0].asNumber();
            quat->x = (float)args[1].asNumber();
            quat->y = (float)args[2].asNumber();
            quat->z = (float)args[3].asNumber();
        }
        else if (argCount == 0)
        {
            *quat = Ogre::Quaternion::IDENTITY;
        }

        Info("Quaternion constructed: (%f, %f, %f, %f)", quat->w, quat->x, quat->y, quat->z);
        return quat;
    }

    // Destructor
    void quat_dtor(Interpreter *vm, void *data)
    {
        Info("Quaternion destructed");
        delete static_cast<Ogre::Quaternion *>(data);
    }

    // Property getters
    Value quat_getW(Interpreter *vm, void *data)
    {
        return vm->makeFloat(static_cast<Ogre::Quaternion *>(data)->w);
    }

    Value quat_getX(Interpreter *vm, void *data)
    {
        return vm->makeFloat(static_cast<Ogre::Quaternion *>(data)->x);
    }

    Value quat_getY(Interpreter *vm, void *data)
    {
        return vm->makeFloat(static_cast<Ogre::Quaternion *>(data)->y);
    }

    Value quat_getZ(Interpreter *vm, void *data)
    {
        return vm->makeFloat(static_cast<Ogre::Quaternion *>(data)->z);
    }

    // Property setters
    void quat_setW(Interpreter *vm, void *data, Value value)
    {
        static_cast<Ogre::Quaternion *>(data)->w = (float)value.asNumber();
    }

    void quat_setX(Interpreter *vm, void *data, Value value)
    {
        static_cast<Ogre::Quaternion *>(data)->x = (float)value.asNumber();
    }

    void quat_setY(Interpreter *vm, void *data, Value value)
    {
        static_cast<Ogre::Quaternion *>(data)->y = (float)value.asNumber();
    }

    void quat_setZ(Interpreter *vm, void *data, Value value)
    {
        static_cast<Ogre::Quaternion *>(data)->z = (float)value.asNumber();
    }

    // Methods
    int quat_normalise(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::Quaternion *quat = static_cast<Ogre::Quaternion *>(data);
        quat->normalise();
        return 0;
    }

    int quat_fromAxisAngle(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 2)
            return 0;

        Ogre::Quaternion *quat = static_cast<Ogre::Quaternion *>(data);
        Ogre::Vector3 *axis = static_cast<Ogre::Vector3 *>(args[0].asPointer());
        float angle = (float)args[1].asNumber();

        if (axis == nullptr)
            return 0;

        *quat = Ogre::Quaternion(Ogre::Radian(angle), *axis);
        return 0;
    }

    int quat_multiply(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
            return 0;

        Ogre::Quaternion *self = static_cast<Ogre::Quaternion *>(data);
        Ogre::Quaternion *other = static_cast<Ogre::Quaternion *>(args[0].asPointer());

        if (other == nullptr)
            return 0;

        *self = *self * *other;
        return 0;
    }

    int quat_inverse(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::Quaternion *quat = static_cast<Ogre::Quaternion *>(data);
        Ogre::Quaternion invQuat = quat->Inverse();
        Ogre::Quaternion *result = new Ogre::Quaternion(invQuat);
        vm->pushPointer((void *)result);
        return 1;
    }

    void registerAll(Interpreter &vm)
    {
        NativeClassDef *quat = vm.registerNativeClass(
            "Quaternion",
            quat_ctor,
            quat_dtor,
            4,      // number of properties (w, x, y, z)
            false   // GC collects
        );

        // Add properties
        vm.addNativeProperty(quat, "w", quat_getW, quat_setW);
        vm.addNativeProperty(quat, "x", quat_getX, quat_setX);
        vm.addNativeProperty(quat, "y", quat_getY, quat_setY);
        vm.addNativeProperty(quat, "z", quat_getZ, quat_setZ);

        // Add methods
        vm.addNativeMethod(quat, "normalise", quat_normalise);
        vm.addNativeMethod(quat, "fromAxisAngle", quat_fromAxisAngle);
        vm.addNativeMethod(quat, "multiply", quat_multiply);
        vm.addNativeMethod(quat, "inverse", quat_inverse);

        Info("Quaternion bindings registered");
    }

} // namespace OgreQuaternionBindings


// ============== OGRE3D BINDINGS ==============

namespace OgreBindings
{

    void registerAll(Interpreter &vm)
    {
        OgreVector3Bindings::registerAll(vm);
        OgreQuaternionBindings::registerAll(vm);
        OgreSceneNodeBindings::registerAll(vm);
        OgreEntityBindings::registerAll(vm);
        OgrePlaneBindings::registerAll(vm);
        OgreSceneBindings::registerAll(vm);
        OgreLightBindings::registerAll(vm);
        OgreCameraBindings::registerAll(vm);
        OgreCoreBindings::registerAll(vm);
        OgreMeshBindings::registerAll(vm);
        InputBindings::registerAll(vm);
        TimerBindings::registerAll(vm);
        CameraControllerBindings::registerAll(vm);
        OgreAnimationStateBindings::registerAll(vm);
        OgreParticleSystemBindings::registerAll(vm);
        OgreRibbonTrailBindings::registerAll(vm);
        OgreTerrainBindings::registerAll(vm);
      
        OgreManualObjectBindings::registerAll(vm);
        OgreResourceManagerBindings::registerAll(vm);
        OgreMaterialBindings::registerAll(vm);
        
        OgreCompositorBindings::registerAll(vm);
        OgreOverlayBindings::registerAll(vm);
       
        OgreBillboardBindings::registerAll(vm);
 
        OgreSkeletonBindings::registerAll(vm);

        Info("Ogre3D bindings registered");
    }

} // namespace OgreBindings

