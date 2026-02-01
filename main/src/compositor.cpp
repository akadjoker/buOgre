#include "bindings.hpp"
#include <OgreCompositorManager.h>
#include <OgreViewport.h>

// ============== OGRE COMPOSITOR BINDINGS ==============
// Post-processing effects (Bloom, HDR, Motion Blur, etc.)

namespace OgreCompositorBindings
{
    // addCompositor(viewport, compositorName, position)
    // Returns compositor instance
    int addCompositor(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 2)
        {
            Error("addCompositor: requires viewport and compositor name");
            return 0;
        }

        NativeClassInstance *viewportInstance = args[0].asNativeClassInstance();
        Ogre::Viewport *viewport = static_cast<Ogre::Viewport *>(viewportInstance->userData);

        if (!viewport)
        {
            Error("addCompositor: invalid viewport");
            return 0;
        }

        const char *compositorName = args[1].asStringChars();
        int position = -1;  // -1 = append to end

        if (argCount >= 3) position = (int)args[2].asNumber();

        try
        {
            Ogre::CompositorInstance *instance = Ogre::CompositorManager::getSingleton().addCompositor(
                viewport, compositorName, position
            );

            if (!instance)
            {
                Error("addCompositor: failed to add compositor '%s'", compositorName);
                return 0;
            }

            Info("Compositor '%s' added to viewport", compositorName);

            // Get the CompositorInstance NativeClassDef
            NativeClassDef *compClass = nullptr;
            if (!vm->tryGetNativeClassDef("CompositorInstance", &compClass))
            {
                Error("CompositorInstance class not found in VM");
                return 0;
            }

            Value compValue = vm->makeNativeClassInstance(false);
            NativeClassInstance *compInst = compValue.asNativeClassInstance();
            compInst->klass = compClass;
            compInst->userData = (void *)instance;

            vm->push(compValue);
            return 1;
        }
        catch (Ogre::Exception &e)
        {
            Error("addCompositor failed: %s", e.what());
            return 0;
        }
    }

    // setCompositorEnabled(viewport, compositorName, enabled)
    int setCompositorEnabled(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 3)
        {
            Error("setCompositorEnabled: requires viewport, compositor name, and enabled");
            return 0;
        }

        NativeClassInstance *viewportInstance = args[0].asNativeClassInstance();
        Ogre::Viewport *viewport = static_cast<Ogre::Viewport *>(viewportInstance->userData);

        if (!viewport) return 0;

        const char *compositorName = args[1].asStringChars();
        bool enabled = args[2].asBool();

        try
        {
            Ogre::CompositorManager::getSingleton().setCompositorEnabled(
                viewport, compositorName, enabled
            );
            Info("Compositor '%s' %s", compositorName, enabled ? "enabled" : "disabled");
        }
        catch (Ogre::Exception &e)
        {
            Error("setCompositorEnabled failed: %s", e.what());
        }

        return 0;
    }

    // removeCompositor(viewport, compositorName)
    int removeCompositor(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 2)
        {
            Error("removeCompositor: requires viewport and compositor name");
            return 0;
        }

        NativeClassInstance *viewportInstance = args[0].asNativeClassInstance();
        Ogre::Viewport *viewport = static_cast<Ogre::Viewport *>(viewportInstance->userData);

        if (!viewport) return 0;

        const char *compositorName = args[1].asStringChars();

        try
        {
            Ogre::CompositorManager::getSingleton().removeCompositor(
                viewport, compositorName
            );
            Info("Compositor '%s' removed", compositorName);
        }
        catch (Ogre::Exception &e)
        {
            Error("removeCompositor failed: %s", e.what());
        }

        return 0;
    }

    // ========== COMPOSITOR INSTANCE METHODS ==========

    // setEnabled(bool)
    int compositor_setEnabled(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::CompositorInstance *comp = static_cast<Ogre::CompositorInstance *>(data);
        if (comp) comp->setEnabled(args[0].asBool());
        return 0;
    }

    // getEnabled() -> bool
    int compositor_getEnabled(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::CompositorInstance *comp = static_cast<Ogre::CompositorInstance *>(data);
        vm->pushBool(comp ? comp->getEnabled() : false);
        return 1;
    }

    // ========== PROPERTIES ==========

    Value compositor_getEnabledProp(Interpreter *vm, void *data)
    {
        Ogre::CompositorInstance *comp = static_cast<Ogre::CompositorInstance *>(data);
        return vm->makeBool(comp ? comp->getEnabled() : false);
    }

    void compositor_setEnabledProp(Interpreter *vm, void *data, Value value)
    {
        Ogre::CompositorInstance *comp = static_cast<Ogre::CompositorInstance *>(data);
        if (comp) comp->setEnabled(value.asBool());
    }

    // ========== HELPER: SETUP COMMON COMPOSITORS ==========

    // setupBloom(viewport) - adds and enables Bloom compositor
    int setupBloom(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            Error("setupBloom: requires viewport");
            return 0;
        }

        NativeClassInstance *viewportInstance = args[0].asNativeClassInstance();
        Ogre::Viewport *viewport = static_cast<Ogre::Viewport *>(viewportInstance->userData);

        if (!viewport) return 0;

        try
        {
            // Add bloom compositor (requires Bloom compositor script in resources)
            Ogre::CompositorInstance *bloom = Ogre::CompositorManager::getSingleton().addCompositor(
                viewport, "Bloom"
            );

            if (bloom)
            {
                bloom->setEnabled(true);
                Info("Bloom effect enabled");
                vm->pushBool(true);
            }
            else
            {
                Info("Bloom compositor not found (check if Bloom.compositor is loaded)");
                vm->pushBool(false);
            }

            return 1;
        }
        catch (Ogre::Exception &e)
        {
            Error("setupBloom failed: %s", e.what());
            vm->pushBool(false);
            return 1;
        }
    }

    // setupHDR(viewport)
    int setupHDR(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        NativeClassInstance *viewportInstance = args[0].asNativeClassInstance();
        Ogre::Viewport *viewport = static_cast<Ogre::Viewport *>(viewportInstance->userData);

        if (!viewport) return 0;

        try
        {
            Ogre::CompositorInstance *hdr = Ogre::CompositorManager::getSingleton().addCompositor(
                viewport, "HDR"
            );

            if (hdr)
            {
                hdr->setEnabled(true);
                Info("HDR effect enabled");
                vm->pushBool(true);
            }
            else
            {
                Info("HDR compositor not found");
                vm->pushBool(false);
            }

            return 1;
        }
        catch (Ogre::Exception &e)
        {
            Error("setupHDR failed: %s", e.what());
            vm->pushBool(false);
            return 1;
        }
    }

    // setupMotionBlur(viewport)
    int setupMotionBlur(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        NativeClassInstance *viewportInstance = args[0].asNativeClassInstance();
        Ogre::Viewport *viewport = static_cast<Ogre::Viewport *>(viewportInstance->userData);

        if (!viewport) return 0;

        try
        {
            Ogre::CompositorInstance *mb = Ogre::CompositorManager::getSingleton().addCompositor(
                viewport, "Motion Blur"
            );

            if (mb)
            {
                mb->setEnabled(true);
                Info("Motion Blur effect enabled");
                vm->pushBool(true);
            }
            else
            {
                Info("Motion Blur compositor not found");
                vm->pushBool(false);
            }

            return 1;
        }
        catch (Ogre::Exception &e)
        {
            Error("setupMotionBlur failed: %s", e.what());
            vm->pushBool(false);
            return 1;
        }
    }

    void registerAll(Interpreter &vm)
    {
        // Register CompositorInstance class
        NativeClassDef *comp = vm.registerNativeClass(
            "CompositorInstance",
            nullptr,  // No constructor
            nullptr,  // No destructor
            1,        // properties: enabled
            false
        );

        // Properties
        vm.addNativeProperty(comp, "enabled", compositor_getEnabledProp, compositor_setEnabledProp);

        // Methods
        vm.addNativeMethod(comp, "setEnabled", compositor_setEnabled);
        vm.addNativeMethod(comp, "getEnabled", compositor_getEnabled);

        // Global functions
        vm.registerNative("addCompositor", addCompositor, 3);
        vm.registerNative("setCompositorEnabled", setCompositorEnabled, 3);
        vm.registerNative("removeCompositor", removeCompositor, 2);

        // Helpers
        vm.registerNative("setupBloom", setupBloom, 1);
        vm.registerNative("setupHDR", setupHDR, 1);
        vm.registerNative("setupMotionBlur", setupMotionBlur, 1);

        Info("Compositor bindings registered");
    }

} // namespace OgreCompositorBindings
