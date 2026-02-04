#pragma once

#include "interpreter.hpp"

#include <Ogre.h>
#include <OgreMeshManager.h>
#include <OgreSubMesh.h>
#include <SDL2/SDL.h> 

inline Ogre::Node::TransformSpace IntToSpace(int value)
{
    switch (value)
    {
    case 0:
        return Ogre::Node::TS_LOCAL;
    case 1:
        return Ogre::Node::TS_PARENT;
    case 2:
        return Ogre::Node::TS_WORLD;
    default:
        return Ogre::Node::TS_LOCAL;
    }
}

namespace OgreVector3Bindings
{
    void registerAll(Interpreter &vm);
}

namespace OgreQuaternionBindings
{
    void registerAll(Interpreter &vm);
}

namespace OgreSceneNodeBindings
{
    void registerAll(Interpreter &vm);
}

namespace OgreBindings
{
    void registerAll(Interpreter &vm);
}

namespace OgreEntityBindings
{
    void registerAll(Interpreter &vm);
}

namespace OgrePlaneBindings
{
    void registerAll(Interpreter &vm);
}

namespace OgreSceneBindings
{
    void registerAll(Interpreter &vm);
}

namespace OgreLightBindings
{
    void registerAll(Interpreter &vm);
}

namespace OgreCameraBindings
{
    void registerAll(Interpreter &vm);
}

namespace OgreCoreBindings
{
    void registerAll(Interpreter &vm);
}

namespace InputBindings
{
    void registerAll(Interpreter &vm);
    void updateInput(SDL_Event &event);
    void updateInputState();
}

namespace OgreMeshBindings
{
    void registerAll(Interpreter &vm);
}

namespace TimerBindings
{
    void registerAll(Interpreter &vm);
    void updateTimer();
}

namespace CameraControllerBindings
{
    void registerAll(Interpreter &vm);
}

namespace OgreAnimationStateBindings
{
    void registerAll(Interpreter &vm);
}

namespace OgreParticleSystemBindings
{
    void registerAll(Interpreter &vm);
}

namespace OgreRibbonTrailBindings
{
    void registerAll(Interpreter &vm);
}

namespace OgreTerrainBindings
{
    void registerAll(Interpreter &vm);
}

namespace ProceduralMesh
{
    int createCube(Ogre::ManualObject *manual, const char *material, float size, const char *group);
    int createSphere(Ogre::ManualObject *manual, const char *material, float radius, int rings, int segments, const char *group);
    int createCylinder(Ogre::ManualObject *manual, const char *material, float radius, float height, int segments, const char *group);
    int createCone(Ogre::ManualObject *manual, const char *material, float radius, float height, int segments, const char *group);
    int createTorus(Ogre::ManualObject *manual, const char *material, float majorRadius, float minorRadius, int majorSegments, int minorSegments, const char *group);
    int createQuad(Ogre::ManualObject *manual, const char *material, float width, float height, const char *group);
    int createPlane(Ogre::ManualObject *manual, const char *material, float width, float depth, int widthSegments, int depthSegments, const char *group);
    int createCapsule(Ogre::ManualObject *manual, const char* material, float radius, float height, int segments, int rings, const char* group);
   
}

namespace OgreManualObjectBindings
{
    void registerAll(Interpreter &vm);
}

namespace OgreResourceManagerBindings
{
    void registerAll(Interpreter &vm);
}

namespace OgreMaterialBindings
{
    void registerAll(Interpreter &vm);
}

namespace OgreCompositorBindings
{
    void registerAll(Interpreter &vm);
}

namespace OgreOverlayBindings
{
    void registerAll(Interpreter &vm);
}

namespace OgreBillboardBindings
{
    void registerAll(Interpreter &vm);
}

namespace OgreSkeletonBindings
{
    void registerAll(Interpreter &vm);
}