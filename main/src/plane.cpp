#include "bindings.hpp"
#include <OgreVector3.h>

namespace OgrePlaneBindings
{
    // Plane(normal: Vector3, distance: float)
    // void *plane_ctor(Interpreter *vm, int argCount, Value *args)
    // {
    //     if (argCount < 1)
    //     {
    //         Error("Plane constructor requires normal (Vector3) argument");
    //         return nullptr;
    //     }

    //     // Get normal vector
    //     Ogre::Vector3 normal(0, 1, 0); // default up
    //     float distance = 0.0f;

    //     // If first arg is a Vector3 (represented as a native object with x, y, z properties)
    //     if (args[0].type == ValueType::NATIVE)
    //     {
    //         // For now, use default - will improve later
    //     }

    //     // If second arg is distance (float)
    //     if (argCount >= 2)
    //     {
    //         distance = args[1].asFloat();
    //     }

    //     // Allocate Plane object (Ogre::Plane)
    //     Ogre::Plane *plane = new Ogre::Plane(normal, distance);
        
    //     Info("Plane created with normal (%.2f, %.2f, %.2f), distance: %.2f", 
    //          plane->normal.x, plane->normal.y, plane->normal.z, plane->d);
        
    //     return plane;
    // }

    // void plane_dtor(Interpreter *vm, void *data)
    // {
    //     delete static_cast<Ogre::Plane *>(data);
    //     Info("Plane deleted");
    // }

    // // Property: normal (Vector3)
    // Value plane_getNormal(Interpreter *vm, void *data)
    // {
    //     Ogre::Plane *plane = static_cast<Ogre::Plane *>(data);
    //     // Return as a new Vector3 object
    //     // For now, return a string representation
       
    // }

    // void plane_setNormal(Interpreter *vm, void *data, Value value)
    // {
    //     // TODO: parse Vector3 from value
    // }

    // // Property: distance (float)
    // Value plane_getDistance(Interpreter *vm, void *data)
    // {
    //     Ogre::Plane *plane = static_cast<Ogre::Plane *>(data);
    //     return vm->makeNumber(plane->d);
    // }

    // void plane_setDistance(Interpreter *vm, void *data, Value value)
    // {
    //     Ogre::Plane *plane = static_cast<Ogre::Plane *>(data);
    //     plane->d = value.asFloat();
    // }

    void registerAll(Interpreter &vm)
    {
        // NativeClassDef *planeClass = vm.registerNativeClass(
        //     "Plane",
        //     plane_ctor,
        //     plane_dtor,
        //     2,      // properties: normal, distance
        //     false
        // );

        // vm.addNativeProperty(planeClass, "normal", plane_getNormal, plane_setNormal);
        // vm.addNativeProperty(planeClass, "distance", plane_getDistance, plane_setDistance);

        Info("Plane bindings registered");
    }

} // namespace OgrePlaneBindings
