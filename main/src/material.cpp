#include "bindings.hpp"
#include <OgreMaterialManager.h>
#include <OgreTechnique.h>
#include <OgrePass.h>
#include <OgreTextureUnitState.h>

// ============== OGRE MATERIAL BINDINGS ==============

namespace OgreMaterialBindings
{
    // createMaterial(name, groupName) - função global
    int createMaterial(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 2)
        {
            Error("createMaterial: requires material name");
            return 0;
        }

        const char *name = args[0].asStringChars();
        const char *group = "General";

        if (argCount >= 2) group = args[1].asStringChars();

        try
        {
            Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(
                name, group
            );

            if (material.isNull())
            {
                Error("createMaterial: failed to create material '%s'", name);
                return 0;
            }

            // Get the Material NativeClassDef
            NativeClassDef *materialClass = nullptr;
            if (!vm->tryGetNativeClassDef("Material", &materialClass))
            {
                Error("Material class not found in VM");
                return 0;
            }

            // Create NativeClassInstance
            // Note: We store the raw pointer, but Ogre manages the lifecycle via ResourcePtr
            Value materialValue = vm->makeNativeClassInstance(false);
            NativeClassInstance *instance = materialValue.asNativeClassInstance();
            instance->klass = materialClass;
            instance->userData = (void *)material.get();

            vm->push(materialValue);
            return 1;
        }
        catch (Ogre::Exception &e)
        {
            Error("createMaterial failed: %s", e.what());
            return 0;
        }
    }

    // getMaterial(name) - pegar material existente
    int getMaterial(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            Error("getMaterial: requires material name");
            return 0;
        }

        const char *name = args[0].asStringChars();

        try
        {
            Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName(name);

            if (material.isNull())
            {
                Error("getMaterial: material '%s' not found", name);
                vm->pushNil();
                return 1;
            }

            NativeClassDef *materialClass = nullptr;
            if (!vm->tryGetNativeClassDef("Material", &materialClass))
            {
                Error("Material class not found in VM");
                vm->pushNil();
                return 1;
            }

            Value materialValue = vm->makeNativeClassInstance(false);
            NativeClassInstance *instance = materialValue.asNativeClassInstance();
            instance->klass = materialClass;
            instance->userData = (void *)material.get();

            vm->push(materialValue);
            return 1;
        }
        catch (Ogre::Exception &e)
        {
            Error("getMaterial failed: %s", e.what());
            vm->pushNil();
            return 1;
        }
    }

    // ========== MATERIAL METHODS ==========

    // setAmbient(r, g, b)
    int material_setAmbient(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3) return 0;

        Ogre::Material *material = static_cast<Ogre::Material *>(data);
        if (!material) return 0;

        Ogre::ColourValue color(
            (float)args[0].asNumber(),
            (float)args[1].asNumber(),
            (float)args[2].asNumber()
        );

        material->setAmbient(color);
        return 0;
    }

    // setDiffuse(r, g, b, a)
    int material_setDiffuse(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 4) return 0;

        Ogre::Material *material = static_cast<Ogre::Material *>(data);
        if (!material) return 0;

        Ogre::ColourValue color(
            (float)args[0].asNumber(),
            (float)args[1].asNumber(),
            (float)args[2].asNumber(),
            (float)args[3].asNumber()
        );

        material->setDiffuse(color);
        return 0;
    }

    // setSpecular(r, g, b, shininess)
    int material_setSpecular(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 4) return 0;

        Ogre::Material *material = static_cast<Ogre::Material *>(data);
        if (!material) return 0;

        Ogre::ColourValue color(
            (float)args[0].asNumber(),
            (float)args[1].asNumber(),
            (float)args[2].asNumber()
        );

        float shininess = (float)args[3].asNumber();

        material->setSpecular(color);
        material->setShininess(shininess);
        return 0;
    }

    // setEmissive(r, g, b)
    int material_setEmissive(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3) return 0;

        Ogre::Material *material = static_cast<Ogre::Material *>(data);
        if (!material) return 0;

        Ogre::ColourValue color(
            (float)args[0].asNumber(),
            (float)args[1].asNumber(),
            (float)args[2].asNumber()
        );

        material->setSelfIllumination(color);
        return 0;
    }

    // setShininess(value)
    int material_setShininess(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::Material *material = static_cast<Ogre::Material *>(data);
        if (material) material->setShininess((float)args[0].asNumber());
        return 0;
    }

    // setTexture(textureName, textureUnit)
    int material_setTexture(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::Material *material = static_cast<Ogre::Material *>(data);
        if (!material) return 0;

        const char *textureName = args[0].asStringChars();
        size_t unit = 0;

        if (argCount >= 2) unit = (size_t)args[1].asNumber();

        try
        {
            Ogre::Technique *tech = material->getTechnique(0);
            if (tech)
            {
                Ogre::Pass *pass = tech->getPass(0);
                if (pass)
                {
                    Ogre::TextureUnitState *texUnit = nullptr;

                    if (pass->getNumTextureUnitStates() > unit)
                    {
                        texUnit = pass->getTextureUnitState(unit);
                    }
                    else
                    {
                        texUnit = pass->createTextureUnitState();
                    }

                    if (texUnit)
                    {
                        texUnit->setTextureName(textureName);
                    }
                }
            }
        }
        catch (Ogre::Exception &e)
        {
            Error("setTexture failed: %s", e.what());
        }

        return 0;
    }

    // setSceneBlending(type)
    // type: 0=REPLACE, 1=ADD, 2=MODULATE, 3=ALPHA_BLEND, 4=TRANSPARENT_ALPHA
    int material_setSceneBlending(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::Material *material = static_cast<Ogre::Material *>(data);
        if (!material) return 0;

        int blendType = (int)args[0].asNumber();

        Ogre::SceneBlendType sbt = Ogre::SBT_REPLACE;
        switch (blendType)
        {
            case 0: sbt = Ogre::SBT_REPLACE; break;
            case 1: sbt = Ogre::SBT_ADD; break;
            case 2: sbt = Ogre::SBT_MODULATE; break;
            case 3: sbt = Ogre::SBT_TRANSPARENT_ALPHA; break;
            case 4: sbt = Ogre::SBT_TRANSPARENT_COLOUR; break;
            default: sbt = Ogre::SBT_REPLACE; break;
        }

        material->setSceneBlending(sbt);
        return 0;
    }

    // setDepthWrite(enabled)
    int material_setDepthWrite(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::Material *material = static_cast<Ogre::Material *>(data);
        if (material) material->setDepthWriteEnabled(args[0].asBool());
        return 0;
    }

    // setLightingEnabled(enabled)
    int material_setLightingEnabled(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::Material *material = static_cast<Ogre::Material *>(data);
        if (material) material->setLightingEnabled(args[0].asBool());
        return 0;
    }

    // setReceiveShadows(enabled)
    int material_setReceiveShadows(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::Material *material = static_cast<Ogre::Material *>(data);
        if (material) material->setReceiveShadows(args[0].asBool());
        return 0;
    }

    // compile()
    int material_compile(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::Material *material = static_cast<Ogre::Material *>(data);
        if (material) material->compile();
        return 0;
    }

    // clone(newName)
    int material_clone(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            Error("clone: requires new material name");
            return 0;
        }

        Ogre::Material *material = static_cast<Ogre::Material *>(data);
        if (!material) return 0;

        const char *newName = args[0].asStringChars();
        const char *group = "General";

        if (argCount >= 2) group = args[1].asStringChars();

        try
        {
            Ogre::MaterialPtr cloned = material->clone(newName, true, group);

            NativeClassDef *materialClass = nullptr;
            if (!vm->tryGetNativeClassDef("Material", &materialClass))
            {
                Error("Material class not found in VM");
                return 0;
            }

            Value materialValue = vm->makeNativeClassInstance(false);
            NativeClassInstance *instance = materialValue.asNativeClassInstance();
            instance->klass = materialClass;
            instance->userData = (void *)cloned.get();

            vm->push(materialValue);
            return 1;
        }
        catch (Ogre::Exception &e)
        {
            Error("clone failed: %s", e.what());
            return 0;
        }
    }

    void registerAll(Interpreter &vm)
    {
        // Register Material class
        NativeClassDef *material = vm.registerNativeClass(
            "Material",
            nullptr,  // No constructor - use createMaterial()
            nullptr,  // No destructor - Ogre manages via ResourcePtr
            0,        // no properties for now
            false
        );

        // Methods
        vm.addNativeMethod(material, "setAmbient", material_setAmbient);
        vm.addNativeMethod(material, "setDiffuse", material_setDiffuse);
        vm.addNativeMethod(material, "setSpecular", material_setSpecular);
        vm.addNativeMethod(material, "setEmissive", material_setEmissive);
        vm.addNativeMethod(material, "setShininess", material_setShininess);
        vm.addNativeMethod(material, "setTexture", material_setTexture);
        vm.addNativeMethod(material, "setSceneBlending", material_setSceneBlending);
        vm.addNativeMethod(material, "setDepthWrite", material_setDepthWrite);
        vm.addNativeMethod(material, "setLightingEnabled", material_setLightingEnabled);
        vm.addNativeMethod(material, "setReceiveShadows", material_setReceiveShadows);
        vm.addNativeMethod(material, "compile", material_compile);
        vm.addNativeMethod(material, "clone", material_clone);

        // Global functions
        vm.registerNative("CreateMaterial", createMaterial, 2);
        vm.registerNative("GetMaterial", getMaterial, 1);

        //Info("Material bindings registered");
    }

} // namespace OgreMaterialBindings
