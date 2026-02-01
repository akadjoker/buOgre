#include "bindings.hpp"
#include <OgreResourceGroupManager.h>

// ============== OGRE RESOURCE MANAGER BINDINGS ==============
// Gerenciamento de resources (meshes, textures, materials, scripts)

namespace OgreResourceManagerBindings
{
    // addResourceLocation(path, type, groupName)
    // type: "FileSystem", "Zip"
    // groupName: "General" (default), ou qualquer nome custom
    int addResourceLocation(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 2)
        {
            Error("addResourceLocation: requires path and type");
            return 0;
        }

        const char *path = args[0].asStringChars();
        const char *type = args[1].asStringChars();
        const char *group = "General";

        if (argCount >= 3)
        {
            group = args[2].asStringChars();
        }

        try
        {
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                path, type, group
            );
            Info("Resource location added: %s (%s) to group '%s'", path, type, group);
        }
        catch (Ogre::Exception &e)
        {
            Error("addResourceLocation failed: %s", e.what());
            return 0;
        }

        return 0;
    }

    // initialiseResourceGroup(groupName)
    int initialiseResourceGroup(Interpreter *vm, int argCount, Value *args)
    {
        const char *group = "General";
        if (argCount >= 1) group = args[0].asStringChars();

        try
        {
            Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(group);
            Info("Resource group '%s' initialised", group);
        }
        catch (Ogre::Exception &e)
        {
            Error("initialiseResourceGroup failed: %s", e.what());
            return 0;
        }

        return 0;
    }

    // loadResourceGroup(groupName)
    int loadResourceGroup(Interpreter *vm, int argCount, Value *args)
    {
        const char *group = "General";
        if (argCount >= 1) group = args[0].asStringChars();

        try
        {
            Ogre::ResourceGroupManager::getSingleton().loadResourceGroup(group);
            Info("Resource group '%s' loaded", group);
        }
        catch (Ogre::Exception &e)
        {
            Error("loadResourceGroup failed: %s", e.what());
            return 0;
        }

        return 0;
    }

    // initialiseAllResourceGroups()
    int initialiseAllResourceGroups(Interpreter *vm, int argCount, Value *args)
    {
        try
        {
            Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
            Info("All resource groups initialised");
        }
        catch (Ogre::Exception &e)
        {
            Error("initialiseAllResourceGroups failed: %s", e.what());
            return 0;
        }

        return 0;
    }

    // unloadResourceGroup(groupName)
    int unloadResourceGroup(Interpreter *vm, int argCount, Value *args)
    {
        const char *group = "General";
        if (argCount >= 1) group = args[0].asStringChars();

        try
        {
            Ogre::ResourceGroupManager::getSingleton().unloadResourceGroup(group);
            Info("Resource group '%s' unloaded", group);
        }
        catch (Ogre::Exception &e)
        {
            Error("unloadResourceGroup failed: %s", e.what());
            return 0;
        }

        return 0;
    }

    // removeResourceLocation(path, groupName)
    int removeResourceLocation(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            Error("removeResourceLocation: requires path");
            return 0;
        }

        const char *path = args[0].asStringChars();
        const char *group = "General";

        if (argCount >= 2) group = args[1].asStringChars();

        try
        {
            Ogre::ResourceGroupManager::getSingleton().removeResourceLocation(path, group);
            Info("Resource location removed: %s from group '%s'", path, group);
        }
        catch (Ogre::Exception &e)
        {
            Error("removeResourceLocation failed: %s", e.what());
            return 0;
        }

        return 0;
    }

    // resourceGroupExists(groupName) -> bool
    int resourceGroupExists(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            vm->pushBool(false);
            return 1;
        }

        const char *group = args[0].asStringChars();
        bool exists = Ogre::ResourceGroupManager::getSingleton().resourceGroupExists(group);
        vm->pushBool(exists);
        return 1;
    }

    // declareResource(name, type, groupName)
    // type: "Mesh", "Material", "Texture", etc.
    int declareResource(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 2)
        {
            Error("declareResource: requires name and type");
            return 0;
        }

        const char *name = args[0].asStringChars();
        const char *type = args[1].asStringChars();
        const char *group = "General";

        if (argCount >= 3) group = args[2].asStringChars();

        try
        {
            Ogre::ResourceGroupManager::getSingleton().declareResource(
                name, type, group
            );
            Info("Resource declared: %s (%s) in group '%s'", name, type, group);
        }
        catch (Ogre::Exception &e)
        {
            Error("declareResource failed: %s", e.what());
            return 0;
        }

        return 0;
    }

    // createResourceGroup(groupName)
    int createResourceGroup(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            Error("createResourceGroup: requires group name");
            return 0;
        }

        const char *group = args[0].asStringChars();

        try
        {
            if (!Ogre::ResourceGroupManager::getSingleton().resourceGroupExists(group))
            {
                Ogre::ResourceGroupManager::getSingleton().createResourceGroup(group);
                Info("Resource group '%s' created", group);
            }
            else
            {
                Info("Resource group '%s' already exists", group);
            }
        }
        catch (Ogre::Exception &e)
        {
            Error("createResourceGroup failed: %s", e.what());
            return 0;
        }

        return 0;
    }

    // destroyResourceGroup(groupName)
    int destroyResourceGroup(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            Error("destroyResourceGroup: requires group name");
            return 0;
        }

        const char *group = args[0].asStringChars();

        try
        {
            Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup(group);
            Info("Resource group '%s' destroyed", group);
        }
        catch (Ogre::Exception &e)
        {
            Error("destroyResourceGroup failed: %s", e.what());
            return 0;
        }

        return 0;
    }

    void registerAll(Interpreter &vm)
    {
        // Global functions for resource management
        vm.registerNative("addResourceLocation", addResourceLocation, 3);
        vm.registerNative("initialiseResourceGroup", initialiseResourceGroup, 1);
        vm.registerNative("loadResourceGroup", loadResourceGroup, 1);
        vm.registerNative("initialiseAllResourceGroups", initialiseAllResourceGroups, 0);
        vm.registerNative("unloadResourceGroup", unloadResourceGroup, 1);
        vm.registerNative("removeResourceLocation", removeResourceLocation, 2);
        vm.registerNative("resourceGroupExists", resourceGroupExists, 1);
        vm.registerNative("declareResource", declareResource, 3);
        vm.registerNative("createResourceGroup", createResourceGroup, 1);
        vm.registerNative("destroyResourceGroup", destroyResourceGroup, 1);

        Info("ResourceManager bindings registered");
    }

} // namespace OgreResourceManagerBindings
