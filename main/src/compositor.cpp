#include "bindings.hpp"
#include <OgreCompositorManager.h>
#include <OgreViewport.h>

// ============== OGRE COMPOSITOR BINDINGS ==============
// Post-processing effects (Bloom, HDR, Motion Blur, etc.)

namespace OgreCompositorBindings
{
    
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
 
                // viewport, "Bloom"
    
                // viewport, "HDR"
          
 
                // viewport, "Motion Blur"
         

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

        Info("Compositor bindings registered");
    }

} // namespace OgreCompositorBindings
