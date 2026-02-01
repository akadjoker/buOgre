#include "bindings.hpp"
#include <OgreManualObject.h>

// ============== OGRE MANUAL OBJECT BINDINGS ==============
// Permite criar geometria procedural dinâmica via script

namespace OgreManualObjectBindings
{
    // createManualObject(scene, "name") - função global
    int createManualObject(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 2)
        {
            Error("createManualObject: requires scene and name");
            return 0;
        }

        NativeClassInstance *sceneInstance = args[0].asNativeClassInstance();
        Ogre::SceneManager *scene = static_cast<Ogre::SceneManager *>(sceneInstance->userData);

        if (!scene)
        {
            Error("createManualObject: invalid scene");
            return 0;
        }

        const char *name = args[1].asStringChars();

        Ogre::ManualObject *manual = scene->createManualObject(name);

        if (!manual)
        {
            Error("createManualObject: failed to create manual object '%s'", name);
            return 0;
        }

        // Get the ManualObject NativeClassDef
        NativeClassDef *manualClass = nullptr;
        if (!vm->tryGetNativeClassDef("ManualObject", &manualClass))
        {
            Error("ManualObject class not found in VM");
            return 0;
        }

        // Create NativeClassInstance
        Value manualValue = vm->makeNativeClassInstance(false);
        NativeClassInstance *instance = manualValue.asNativeClassInstance();
        instance->klass = manualClass;
        instance->userData = (void *)manual;

        vm->push(manualValue);
        return 1;
    }

    // ========== MANUAL OBJECT METHODS ==========

    // begin(materialName, operationType)
    // operationType: 1=POINT_LIST, 2=LINE_LIST, 3=LINE_STRIP, 4=TRIANGLE_LIST, 5=TRIANGLE_STRIP, 6=TRIANGLE_FAN
    int manual_begin(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            Error("begin: requires materialName");
            return 0;
        }

        Ogre::ManualObject *manual = static_cast<Ogre::ManualObject *>(data);
        if (!manual) return 0;

        const char *materialName = args[0].asStringChars();
        Ogre::RenderOperation::OperationType opType = Ogre::RenderOperation::OT_TRIANGLE_LIST;

        if (argCount >= 2)
        {
            int type = (int)args[1].asNumber();
            switch (type)
            {
                case 1: opType = Ogre::RenderOperation::OT_POINT_LIST; break;
                case 2: opType = Ogre::RenderOperation::OT_LINE_LIST; break;
                case 3: opType = Ogre::RenderOperation::OT_LINE_STRIP; break;
                case 4: opType = Ogre::RenderOperation::OT_TRIANGLE_LIST; break;
                case 5: opType = Ogre::RenderOperation::OT_TRIANGLE_STRIP; break;
                case 6: opType = Ogre::RenderOperation::OT_TRIANGLE_FAN; break;
                default: opType = Ogre::RenderOperation::OT_TRIANGLE_LIST; break;
            }
        }

        manual->begin(materialName, opType);
        return 0;
    }

    // position(x, y, z)
    int manual_position(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3) return 0;

        Ogre::ManualObject *manual = static_cast<Ogre::ManualObject *>(data);
        if (!manual) return 0;

        manual->position(
            (float)args[0].asNumber(),
            (float)args[1].asNumber(),
            (float)args[2].asNumber()
        );
        return 0;
    }

    // normal(x, y, z)
    int manual_normal(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3) return 0;

        Ogre::ManualObject *manual = static_cast<Ogre::ManualObject *>(data);
        if (!manual) return 0;

        manual->normal(
            (float)args[0].asNumber(),
            (float)args[1].asNumber(),
            (float)args[2].asNumber()
        );
        return 0;
    }

    // textureCoord(u, v)
    int manual_textureCoord(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 2) return 0;

        Ogre::ManualObject *manual = static_cast<Ogre::ManualObject *>(data);
        if (!manual) return 0;

        manual->textureCoord(
            (float)args[0].asNumber(),
            (float)args[1].asNumber()
        );
        return 0;
    }

    // colour(r, g, b, a)
    int manual_colour(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 4) return 0;

        Ogre::ManualObject *manual = static_cast<Ogre::ManualObject *>(data);
        if (!manual) return 0;

        manual->colour(
            (float)args[0].asNumber(),
            (float)args[1].asNumber(),
            (float)args[2].asNumber(),
            (float)args[3].asNumber()
        );
        return 0;
    }

    // triangle(i1, i2, i3)
    int manual_triangle(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3) return 0;

        Ogre::ManualObject *manual = static_cast<Ogre::ManualObject *>(data);
        if (!manual) return 0;

        manual->triangle(
            (uint32_t)args[0].asNumber(),
            (uint32_t)args[1].asNumber(),
            (uint32_t)args[2].asNumber()
        );
        return 0;
    }

    // quad(i1, i2, i3, i4)
    int manual_quad(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 4) return 0;

        Ogre::ManualObject *manual = static_cast<Ogre::ManualObject *>(data);
        if (!manual) return 0;

        manual->quad(
            (uint32_t)args[0].asNumber(),
            (uint32_t)args[1].asNumber(),
            (uint32_t)args[2].asNumber(),
            (uint32_t)args[3].asNumber()
        );
        return 0;
    }

    // index(i)
    int manual_index(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::ManualObject *manual = static_cast<Ogre::ManualObject *>(data);
        if (!manual) return 0;

        manual->index((uint32_t)args[0].asNumber());
        return 0;
    }

    // end()
    int manual_end(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::ManualObject *manual = static_cast<Ogre::ManualObject *>(data);
        if (manual) manual->end();
        return 0;
    }

    // clear()
    int manual_clear(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::ManualObject *manual = static_cast<Ogre::ManualObject *>(data);
        if (manual) manual->clear();
        return 0;
    }

    // convertToMesh(meshName) -> returns mesh name
    int manual_convertToMesh(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            Error("convertToMesh: requires mesh name");
            return 0;
        }

        Ogre::ManualObject *manual = static_cast<Ogre::ManualObject *>(data);
        if (!manual) return 0;

        const char *meshName = args[0].asStringChars();
        manual->convertToMesh(meshName);

        vm->pushString(meshName);
        return 1;
    }

    // setBoundingBox(minX, minY, minZ, maxX, maxY, maxZ)
    int manual_setBoundingBox(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 6) return 0;

        Ogre::ManualObject *manual = static_cast<Ogre::ManualObject *>(data);
        if (!manual) return 0;

        Ogre::AxisAlignedBox bbox(
            Ogre::Vector3(
                (float)args[0].asNumber(),
                (float)args[1].asNumber(),
                (float)args[2].asNumber()
            ),
            Ogre::Vector3(
                (float)args[3].asNumber(),
                (float)args[4].asNumber(),
                (float)args[5].asNumber()
            )
        );

        manual->setBoundingBox(bbox);
        return 0;
    }

    // setDynamic(bool) - permite atualizar vértices em runtime
    int manual_setDynamic(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::ManualObject *manual = static_cast<Ogre::ManualObject *>(data);
        if (manual) manual->setDynamic(args[0].asBool());
        return 0;
    }

    // beginUpdate(sectionIndex)
    int manual_beginUpdate(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::ManualObject *manual = static_cast<Ogre::ManualObject *>(data);
        if (manual)
        {
            manual->beginUpdate((size_t)args[0].asNumber());
        }
        return 0;
    }

    // setCastShadows(bool)
    int manual_setCastShadows(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::ManualObject *manual = static_cast<Ogre::ManualObject *>(data);
        if (manual) manual->setCastShadows(args[0].asBool());
        return 0;
    }

    // ========== PROPERTIES ==========

    Value manual_getDynamic(Interpreter *vm, void *data)
    {
        Ogre::ManualObject *manual = static_cast<Ogre::ManualObject *>(data);
        return vm->makeBool(manual ? manual->getDynamic() : false);
    }

    void manual_setDynamicProp(Interpreter *vm, void *data, Value value)
    {
        Ogre::ManualObject *manual = static_cast<Ogre::ManualObject *>(data);
        if (manual) manual->setDynamic(value.asBool());
    }

    Value manual_getCastShadows(Interpreter *vm, void *data)
    {
        Ogre::ManualObject *manual = static_cast<Ogre::ManualObject *>(data);
        return vm->makeBool(manual ? manual->getCastShadows() : false);
    }

    void manual_setCastShadowsProp(Interpreter *vm, void *data, Value value)
    {
        Ogre::ManualObject *manual = static_cast<Ogre::ManualObject *>(data);
        if (manual) manual->setCastShadows(value.asBool());
    }

    // ========== HELPER FUNCTIONS ==========

    // createGrassQuad(manual, x, y, z, width, height) - helper para criar grass
    int createGrassQuad(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 6)
        {
            Error("createGrassQuad: requires manual, x, y, z, width, height");
            return 0;
        }

        NativeClassInstance *manualInstance = args[0].asNativeClassInstance();
        Ogre::ManualObject *manual = static_cast<Ogre::ManualObject *>(manualInstance->userData);

        if (!manual) return 0;

        float x = (float)args[1].asNumber();
        float y = (float)args[2].asNumber();
        float z = (float)args[3].asNumber();
        float width = (float)args[4].asNumber();
        float height = (float)args[5].asNumber();

        float hw = width * 0.5f;

        // Quad vertical (billboard)
        manual->position(x - hw, y, z);
        manual->normal(0, 0, 1);
        manual->textureCoord(0, 1);

        manual->position(x + hw, y, z);
        manual->normal(0, 0, 1);
        manual->textureCoord(1, 1);

        manual->position(x + hw, y + height, z);
        manual->normal(0, 0, 1);
        manual->textureCoord(1, 0);

        manual->position(x - hw, y + height, z);
        manual->normal(0, 0, 1);
        manual->textureCoord(0, 0);

        return 0;
    }

    // createDecalQuad(manual, x, y, z, sizeX, sizeZ, normalY) - helper para decals no chão
    int createDecalQuad(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 7)
        {
            Error("createDecalQuad: requires manual, x, y, z, sizeX, sizeZ, normalY");
            return 0;
        }

        NativeClassInstance *manualInstance = args[0].asNativeClassInstance();
        Ogre::ManualObject *manual = static_cast<Ogre::ManualObject *>(manualInstance->userData);

        if (!manual) return 0;

        float x = (float)args[1].asNumber();
        float y = (float)args[2].asNumber();
        float z = (float)args[3].asNumber();
        float sizeX = (float)args[4].asNumber();
        float sizeZ = (float)args[5].asNumber();
        float normalY = (float)args[6].asNumber();

        float hx = sizeX * 0.5f;
        float hz = sizeZ * 0.5f;

        // Quad horizontal (decal no chão)
        manual->position(x - hx, y, z - hz);
        manual->normal(0, normalY, 0);
        manual->textureCoord(0, 1);

        manual->position(x + hx, y, z - hz);
        manual->normal(0, normalY, 0);
        manual->textureCoord(1, 1);

        manual->position(x + hx, y, z + hz);
        manual->normal(0, normalY, 0);
        manual->textureCoord(1, 0);

        manual->position(x - hx, y, z + hz);
        manual->normal(0, normalY, 0);
        manual->textureCoord(0, 0);

        return 0;
    }

    void registerAll(Interpreter &vm)
    {
        // Register ManualObject class
        NativeClassDef *manual = vm.registerNativeClass(
            "ManualObject",
            nullptr,  // No constructor - use createManualObject()
            nullptr,  // No destructor - Ogre owns it
            2,        // properties: dynamic, castShadows
            false
        );

        // Properties
        vm.addNativeProperty(manual, "dynamic", manual_getDynamic, manual_setDynamicProp);
        vm.addNativeProperty(manual, "castShadows", manual_getCastShadows, manual_setCastShadowsProp);

        // Construction methods
        vm.addNativeMethod(manual, "begin", manual_begin);
        vm.addNativeMethod(manual, "position", manual_position);
        vm.addNativeMethod(manual, "normal", manual_normal);
        vm.addNativeMethod(manual, "textureCoord", manual_textureCoord);
        vm.addNativeMethod(manual, "colour", manual_colour);
        vm.addNativeMethod(manual, "triangle", manual_triangle);
        vm.addNativeMethod(manual, "quad", manual_quad);
        vm.addNativeMethod(manual, "index", manual_index);
        vm.addNativeMethod(manual, "end", manual_end);

        // Utility methods
        vm.addNativeMethod(manual, "clear", manual_clear);
        vm.addNativeMethod(manual, "convertToMesh", manual_convertToMesh);
        vm.addNativeMethod(manual, "setBoundingBox", manual_setBoundingBox);
        vm.addNativeMethod(manual, "setDynamic", manual_setDynamic);
        vm.addNativeMethod(manual, "beginUpdate", manual_beginUpdate);
        vm.addNativeMethod(manual, "setCastShadows", manual_setCastShadows);

        // Global functions
        vm.registerNative("createManualObject", createManualObject, 2);
        vm.registerNative("createGrassQuad", createGrassQuad, 6);
        vm.registerNative("createDecalQuad", createDecalQuad, 7);

        Info("ManualObject bindings registered");
    }

} // namespace OgreManualObjectBindings
