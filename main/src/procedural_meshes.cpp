#include "bindings.hpp"
#include <OgreMeshManager.h>
#include <OgreSubMesh.h>

// ============== PROCEDURAL MESH GENERATION ==============

namespace ProceduralMeshBindings
{
    // Helper function to create a manual mesh
    Ogre::ManualObject* createManualObject(Ogre::SceneManager *scene, const char *name)
    {
        return scene->createManualObject(name);
    }

    // ========== CREATE CUBE ==========

    int createCube(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 2)
        {
            Error("createCube: requires scene and name");
            return 0;
        }

        NativeClassInstance *sceneInstance = args[0].asNativeClassInstance();
        Ogre::SceneManager *scene = static_cast<Ogre::SceneManager *>(sceneInstance->userData);

        if (!scene)
        {
            Error("createCube: invalid scene");
            return 0;
        }

        const char *name = args[1].asStringChars();
        float size = 1.0f;
        if (argCount >= 3) size = (float)args[2].asNumber();

        // Create manual object
        Ogre::ManualObject *manual = scene->createManualObject(Ogre::String(name) + "_manual");
        manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);

        float s = size / 2.0f;

        // Define 8 vertices of cube
        Ogre::Vector3 vertices[8] = {
            Ogre::Vector3(-s, -s, -s), // 0
            Ogre::Vector3( s, -s, -s), // 1
            Ogre::Vector3( s,  s, -s), // 2
            Ogre::Vector3(-s,  s, -s), // 3
            Ogre::Vector3(-s, -s,  s), // 4
            Ogre::Vector3( s, -s,  s), // 5
            Ogre::Vector3( s,  s,  s), // 6
            Ogre::Vector3(-s,  s,  s)  // 7
        };

        // Front face
        manual->position(vertices[0]); manual->normal(0, 0, -1); manual->textureCoord(0, 1);
        manual->position(vertices[1]); manual->normal(0, 0, -1); manual->textureCoord(1, 1);
        manual->position(vertices[2]); manual->normal(0, 0, -1); manual->textureCoord(1, 0);
        manual->position(vertices[3]); manual->normal(0, 0, -1); manual->textureCoord(0, 0);

        // Back face
        manual->position(vertices[5]); manual->normal(0, 0, 1); manual->textureCoord(0, 1);
        manual->position(vertices[4]); manual->normal(0, 0, 1); manual->textureCoord(1, 1);
        manual->position(vertices[7]); manual->normal(0, 0, 1); manual->textureCoord(1, 0);
        manual->position(vertices[6]); manual->normal(0, 0, 1); manual->textureCoord(0, 0);

        // Top face
        manual->position(vertices[3]); manual->normal(0, 1, 0); manual->textureCoord(0, 1);
        manual->position(vertices[2]); manual->normal(0, 1, 0); manual->textureCoord(1, 1);
        manual->position(vertices[6]); manual->normal(0, 1, 0); manual->textureCoord(1, 0);
        manual->position(vertices[7]); manual->normal(0, 1, 0); manual->textureCoord(0, 0);

        // Bottom face
        manual->position(vertices[4]); manual->normal(0, -1, 0); manual->textureCoord(0, 1);
        manual->position(vertices[5]); manual->normal(0, -1, 0); manual->textureCoord(1, 1);
        manual->position(vertices[1]); manual->normal(0, -1, 0); manual->textureCoord(1, 0);
        manual->position(vertices[0]); manual->normal(0, -1, 0); manual->textureCoord(0, 0);

        // Right face
        manual->position(vertices[1]); manual->normal(1, 0, 0); manual->textureCoord(0, 1);
        manual->position(vertices[5]); manual->normal(1, 0, 0); manual->textureCoord(1, 1);
        manual->position(vertices[6]); manual->normal(1, 0, 0); manual->textureCoord(1, 0);
        manual->position(vertices[2]); manual->normal(1, 0, 0); manual->textureCoord(0, 0);

        // Left face
        manual->position(vertices[4]); manual->normal(-1, 0, 0); manual->textureCoord(0, 1);
        manual->position(vertices[0]); manual->normal(-1, 0, 0); manual->textureCoord(1, 1);
        manual->position(vertices[3]); manual->normal(-1, 0, 0); manual->textureCoord(1, 0);
        manual->position(vertices[7]); manual->normal(-1, 0, 0); manual->textureCoord(0, 0);

        // Define indices (2 triangles per face)
        for (int i = 0; i < 6; ++i)
        {
            int base = i * 4;
            manual->triangle(base, base + 1, base + 2);
            manual->triangle(base, base + 2, base + 3);
        }

        manual->end();

        // Convert to mesh
        Ogre::String meshName = Ogre::String(name) + ".mesh";
        manual->convertToMesh(meshName);

        Info("Cube mesh '%s' created", name);
        vm->pushString(meshName.c_str());
        return 1;
    }

    // ========== CREATE SPHERE ==========

    int createSphere(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 2)
        {
            Error("createSphere: requires scene and name");
            return 0;
        }

        NativeClassInstance *sceneInstance = args[0].asNativeClassInstance();
        Ogre::SceneManager *scene = static_cast<Ogre::SceneManager *>(sceneInstance->userData);

        if (!scene)
        {
            Error("createSphere: invalid scene");
            return 0;
        }

        const char *name = args[1].asStringChars();
        float radius = 1.0f;
        int rings = 16;
        int segments = 16;

        if (argCount >= 3) radius = (float)args[2].asNumber();
        if (argCount >= 4) rings = (int)args[3].asNumber();
        if (argCount >= 5) segments = (int)args[4].asNumber();

        Ogre::ManualObject *manual = scene->createManualObject(Ogre::String(name) + "_manual");
        manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);

        // Generate sphere vertices
        for (int ring = 0; ring <= rings; ++ring)
        {
            float phi = Ogre::Math::PI * ring / rings;
            for (int seg = 0; seg <= segments; ++seg)
            {
                float theta = 2.0f * Ogre::Math::PI * seg / segments;

                float x = radius * Ogre::Math::Sin(phi) * Ogre::Math::Cos(theta);
                float y = radius * Ogre::Math::Cos(phi);
                float z = radius * Ogre::Math::Sin(phi) * Ogre::Math::Sin(theta);

                Ogre::Vector3 pos(x, y, z);
                Ogre::Vector3 normal = pos.normalisedCopy();

                manual->position(pos);
                manual->normal(normal);
                manual->textureCoord((float)seg / segments, (float)ring / rings);
            }
        }

        // Generate indices
        for (int ring = 0; ring < rings; ++ring)
        {
            for (int seg = 0; seg < segments; ++seg)
            {
                int current = ring * (segments + 1) + seg;
                int next = current + segments + 1;

                manual->triangle(current, next, current + 1);
                manual->triangle(current + 1, next, next + 1);
            }
        }

        manual->end();

        Ogre::String meshName = Ogre::String(name) + ".mesh";
        manual->convertToMesh(meshName);

        Info("Sphere mesh '%s' created (r=%.2f, rings=%d, segments=%d)", name, radius, rings, segments);
        vm->pushString(meshName.c_str());
        return 1;
    }

    // ========== CREATE CYLINDER ==========

    int createCylinder(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 2)
        {
            Error("createCylinder: requires scene and name");
            return 0;
        }

        NativeClassInstance *sceneInstance = args[0].asNativeClassInstance();
        Ogre::SceneManager *scene = static_cast<Ogre::SceneManager *>(sceneInstance->userData);

        if (!scene)
        {
            Error("createCylinder: invalid scene");
            return 0;
        }

        const char *name = args[1].asStringChars();
        float radius = 0.5f;
        float height = 2.0f;
        int segments = 16;

        if (argCount >= 3) radius = (float)args[2].asNumber();
        if (argCount >= 4) height = (float)args[3].asNumber();
        if (argCount >= 5) segments = (int)args[4].asNumber();

        Ogre::ManualObject *manual = scene->createManualObject(Ogre::String(name) + "_manual");
        manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);

        float halfHeight = height / 2.0f;

        // Generate cylinder body
        for (int i = 0; i <= segments; ++i)
        {
            float theta = 2.0f * Ogre::Math::PI * i / segments;
            float x = radius * Ogre::Math::Cos(theta);
            float z = radius * Ogre::Math::Sin(theta);

            Ogre::Vector3 normal(x, 0, z);
            normal.normalise();

            // Top vertex
            manual->position(x, halfHeight, z);
            manual->normal(normal);
            manual->textureCoord((float)i / segments, 0);

            // Bottom vertex
            manual->position(x, -halfHeight, z);
            manual->normal(normal);
            manual->textureCoord((float)i / segments, 1);
        }

        // Generate body indices
        for (int i = 0; i < segments; ++i)
        {
            int current = i * 2;
            int next = current + 2;

            manual->triangle(current, next, current + 1);
            manual->triangle(current + 1, next, next + 1);
        }

        manual->end();

        Ogre::String meshName = Ogre::String(name) + ".mesh";
        manual->convertToMesh(meshName);

        Info("Cylinder mesh '%s' created (r=%.2f, h=%.2f, segments=%d)", name, radius, height, segments);
        vm->pushString(meshName.c_str());
        return 1;
    }

    // ========== CREATE CONE ==========

    int createCone(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 2)
        {
            Error("createCone: requires scene and name");
            return 0;
        }

        NativeClassInstance *sceneInstance = args[0].asNativeClassInstance();
        Ogre::SceneManager *scene = static_cast<Ogre::SceneManager *>(sceneInstance->userData);

        if (!scene)
        {
            Error("createCone: invalid scene");
            return 0;
        }

        const char *name = args[1].asStringChars();
        float radius = 0.5f;
        float height = 2.0f;
        int segments = 16;

        if (argCount >= 3) radius = (float)args[2].asNumber();
        if (argCount >= 4) height = (float)args[3].asNumber();
        if (argCount >= 5) segments = (int)args[4].asNumber();

        Ogre::ManualObject *manual = scene->createManualObject(Ogre::String(name) + "_manual");
        manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);

        // Tip of cone
        Ogre::Vector3 tip(0, height / 2.0f, 0);

        // Generate cone body
        for (int i = 0; i <= segments; ++i)
        {
            float theta = 2.0f * Ogre::Math::PI * i / segments;
            float x = radius * Ogre::Math::Cos(theta);
            float z = radius * Ogre::Math::Sin(theta);

            // Base vertex
            manual->position(x, -height / 2.0f, z);
            manual->normal(Ogre::Vector3(x, radius, z).normalisedCopy());
            manual->textureCoord((float)i / segments, 1);

            // Tip vertex
            manual->position(tip);
            manual->normal(Ogre::Vector3(x, radius, z).normalisedCopy());
            manual->textureCoord((float)i / segments, 0);
        }

        // Generate indices
        for (int i = 0; i < segments; ++i)
        {
            int current = i * 2;
            int next = current + 2;

            manual->triangle(current, next, current + 1);
        }

        manual->end();

        Ogre::String meshName = Ogre::String(name) + ".mesh";
        manual->convertToMesh(meshName);

        Info("Cone mesh '%s' created (r=%.2f, h=%.2f, segments=%d)", name, radius, height, segments);
        vm->pushString(meshName.c_str());
        return 1;
    }

    // ========== CREATE TORUS ==========

    int createTorus(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 2)
        {
            Error("createTorus: requires scene and name");
            return 0;
        }

        NativeClassInstance *sceneInstance = args[0].asNativeClassInstance();
        Ogre::SceneManager *scene = static_cast<Ogre::SceneManager *>(sceneInstance->userData);

        if (!scene)
        {
            Error("createTorus: invalid scene");
            return 0;
        }

        const char *name = args[1].asStringChars();
        float majorRadius = 1.0f;
        float minorRadius = 0.3f;
        int majorSegments = 16;
        int minorSegments = 8;

        if (argCount >= 3) majorRadius = (float)args[2].asNumber();
        if (argCount >= 4) minorRadius = (float)args[3].asNumber();
        if (argCount >= 5) majorSegments = (int)args[4].asNumber();
        if (argCount >= 6) minorSegments = (int)args[5].asNumber();

        Ogre::ManualObject *manual = scene->createManualObject(Ogre::String(name) + "_manual");
        manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);

        // Generate torus vertices
        for (int i = 0; i <= majorSegments; ++i)
        {
            float theta = 2.0f * Ogre::Math::PI * i / majorSegments;
            float cosTheta = Ogre::Math::Cos(theta);
            float sinTheta = Ogre::Math::Sin(theta);

            for (int j = 0; j <= minorSegments; ++j)
            {
                float phi = 2.0f * Ogre::Math::PI * j / minorSegments;
                float cosPhi = Ogre::Math::Cos(phi);
                float sinPhi = Ogre::Math::Sin(phi);

                float x = (majorRadius + minorRadius * cosPhi) * cosTheta;
                float y = minorRadius * sinPhi;
                float z = (majorRadius + minorRadius * cosPhi) * sinTheta;

                Ogre::Vector3 center(majorRadius * cosTheta, 0, majorRadius * sinTheta);
                Ogre::Vector3 pos(x, y, z);
                Ogre::Vector3 normal = (pos - center).normalisedCopy();

                manual->position(pos);
                manual->normal(normal);
                manual->textureCoord((float)i / majorSegments, (float)j / minorSegments);
            }
        }

        // Generate indices
        for (int i = 0; i < majorSegments; ++i)
        {
            for (int j = 0; j < minorSegments; ++j)
            {
                int current = i * (minorSegments + 1) + j;
                int next = current + minorSegments + 1;

                manual->triangle(current, next, current + 1);
                manual->triangle(current + 1, next, next + 1);
            }
        }

        manual->end();

        Ogre::String meshName = Ogre::String(name) + ".mesh";
        manual->convertToMesh(meshName);

        Info("Torus mesh '%s' created", name);
        vm->pushString(meshName.c_str());
        return 1;
    }

    

    void registerAll(Interpreter &vm)
    {
        // Register global functions
        vm.registerNative("createCube", createCube, 3);
        vm.registerNative("createSphere", createSphere, 5);
        vm.registerNative("createCylinder", createCylinder, 5);
        vm.registerNative("createCone", createCone, 5);
        vm.registerNative("createTorus", createTorus, 6);

        Info("Procedural Mesh bindings registered");
    }

} // namespace ProceduralMeshBindings
