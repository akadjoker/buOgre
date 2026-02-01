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
        Info("Entity ref released");
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
        if (argCount < 1) return 0;
        Ogre::Entity *ent = static_cast<Ogre::Entity *>(data);
        const char *matName = args[0].asStringChars();
        ent->setMaterialName(matName);
        return 0;
    }

    void registerAll(Interpreter &vm)
    {
        NativeClassDef *ent = vm.registerNativeClass(
            "Entity",
            entity_ctor,
            entity_dtor,
            2,      // properties: castShadows, visible
            false
        );

        vm.addNativeProperty(ent, "castShadows", entity_getCastShadows, entity_setCastShadows);
        vm.addNativeProperty(ent, "visible", entity_getVisible, entity_setVisible);
        vm.addNativeMethod(ent, "setMaterial", entity_setMaterial);

        Info("Entity bindings registered");
    }

} // namespace OgreEntityBindings
