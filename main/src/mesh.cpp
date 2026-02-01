#include "bindings.hpp"

extern Ogre::Root *mRoot;

namespace OgreMeshBindings
{
    // createPlane(name, width, height, xSegments, zSegments, normals?, numTexCoordSets?, uTile?, vTile?, upVector?) -> returns mesh name
    int mesh_createPlane(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 5) 
        {
            Error("CreatePlane requires 5 arguments: name, width, height, xSegments, zSegments");
            return 0;
        }

        const char *name = args[0].asStringChars();
        float width = (float)args[1].asNumber();
        float height = (float)args[2].asNumber();
        int xSegments = (int)args[3].asNumber();
        int zSegments = (int)args[4].asNumber();

        // Optional parameters with defaults
        bool normals = argCount > 5 ? args[5].asBool() : true;
        int numTexCoordSets = argCount > 6 ? (int)args[6].asNumber() : 1;
        float uTile = argCount > 7 ? (float)args[7].asNumber() : 5.0f;
        float vTile = argCount > 8 ? (float)args[8].asNumber() : 5.0f;
        
        Ogre::Vector3 upVector = Ogre::Vector3::UNIT_Z;
        // TODO: parse Vector3 from args[9] if provided

        try
        {
            Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
            Ogre::MeshManager::getSingleton().createPlane(
                name, 
                Ogre::RGN_DEFAULT,
                plane,
                width, height, 
                xSegments, zSegments,
                normals, 
                numTexCoordSets, uTile, vTile,
                upVector);
            
            Info("CreatePlane '%s' created: %fx%f, segments=%dx%d, tiles=%.1fx%.1f", 
                 name, width, height, xSegments, zSegments, uTile, vTile);
            
            return 0;
        }
        catch (const std::exception &e)
        {
            Error("Failed to create plane mesh: %s", e.what());
            return 0;
        }
    }
 
    void registerAll(Interpreter &vm)
    {
      
        vm.registerNative("CreatePlane", mesh_createPlane, -1);
     //   vm.registerNative("CreateCube", mesh_createCube,2);
     //   vm.registerNative("CreateSphere", mesh_createSphere,4);
     
    }
 

} // namespace OgreMeshBindings
