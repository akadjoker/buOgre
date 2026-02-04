#include "bindings.hpp"

// ============== OGRE SCENENODE BINDINGS ==============

extern Ogre::RenderWindow *mWindow ;

namespace OgreCoreBindings
{
    
     // void *light_ctor(Interpreter *vm, int argCount, Value *args)
    void* viewport_ctor(Interpreter *vm, int argCount, Value *args)
    {
        if (!mWindow)
        {
            Error("Viewport: window not created yet");
            return nullptr;
        }

        NativeClassInstance *camInstance = args[0].asNativeClassInstance();

        Ogre::Camera *cam = static_cast<Ogre::Camera *>(camInstance->userData);
        if (!cam)
        {
            Error("Viewport: camera not provided");
            return nullptr;
        }

        Ogre::Viewport* vp = mWindow->addViewport(cam);
        if (!vp)
        {
            Error("Viewport: no viewport found");
            return nullptr;
        }
        Info("Viewport constructed");
        return vp;
    }

    void viewport_dtor(Interpreter *vm, void *data)
    {
        
        
    }

    int viewport_setBackgroundColour(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3) 
        {
            Error("setBackgroundColour requires 3 arguments (r, g, b)");
            return 0;
        }

        Ogre::Viewport* vp = static_cast<Ogre::Viewport *>(data);
        float r = (float)args[0].asNumber();
        float g = (float)args[1].asNumber();
        float b = (float)args[2].asNumber();

        vp->setBackgroundColour(Ogre::ColourValue(r, g, b));
        return 0;
    }

    int viewport_setDepthClear(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            Error("setDepthClear requires 1 argument (depth)");
            return 0;
        }


        Ogre::Viewport* vp = static_cast<Ogre::Viewport *>(data);
        float depth = (float)args[0].asNumber();

        vp->setDepthClear(depth);
        return 0;
    }


    // addCompositor(viewport, compositorName, position)
    int addCompositor(Interpreter *vm, void *data, int argCount, Value *args)
    {
        
        Ogre::Viewport *viewport = static_cast<Ogre::Viewport *>( data);
        if (!viewport)
        {
            Error("addCompositor: invalid viewport");
            vm->pushNil();
            return 1;
        }

        if (argCount < 1)
        {
            Error("addCompositor: requires compositor name");
            vm->pushNil();
            return 1;
        }

        if (!args[0].isString())
        {
            Error("addCompositor: compositor name must be a string");
            vm->pushNil();
            return 1;
        }

        const char *compositorName = args[0].asStringChars();
        int position = -1;  // -1 = append to end

        if (argCount >= 2) position = (int)args[1].asNumber();

        try
        {
            Ogre::CompositorInstance *instance = Ogre::CompositorManager::getSingleton().addCompositor(
                viewport, compositorName, position
            );

            if (!instance)
            {
                Error("addCompositor: failed to add compositor '%s'", compositorName);
                vm->pushNil();
                return 1;
            }

            Info("Compositor '%s' added to viewport", compositorName);

            // Get the CompositorInstance NativeClassDef
            NativeClassDef *compClass = nullptr;
            if (!vm->tryGetNativeClassDef("CompositorInstance", &compClass))
            {
                Error("CompositorInstance class not found in VM");
                vm->pushNil();
                return 1;
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
            vm->pushNil();
            return 1;
        }
    }

    // setCompositorEnabled(viewport, compositorName, enabled)
    int setCompositorEnabled(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::Viewport *viewport = static_cast<Ogre::Viewport *>(data);

        if (!viewport)
        {
            Error("setCompositorEnabled: invalid viewport");
            return 0;
        }
        if (argCount < 2)
        {
            Error("setCompositorEnabled: requires compositor name and enabled flag");
            return 0;
        }

        const char *compositorName = args[0].asStringChars();
        bool enabled = args[1].asBool();

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
    int removeCompositor(Interpreter *vm, void *data, int argCount, Value *args)
    {
        
        Ogre::Viewport *viewport = static_cast<Ogre::Viewport *>(data);

        if (!viewport)
        {
            Error("removeCompositor: invalid viewport");
            return 0;
        }
        if (argCount < 1)
        {
            Error("removeCompositor: requires compositor name");
            return 0;
        }
        const char *compositorName = args[0].asStringChars();

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

//    void setBackgroundColour(const ColourValue& colour) { mBackColour = colour; }
// void setDepthClear( float depth ) { mDepthClearValue = depth; }


     void registerAll(Interpreter &vm)
    {
        NativeClassDef *lt = vm.registerNativeClass(
        "Viewport",
            viewport_ctor,
            nullptr,  // destructor
            1,      // properties: posX, posY, posZ
            false
        );

            vm.addNativeMethod(lt, "setBackgroundColor", viewport_setBackgroundColour);
            vm.addNativeMethod(lt, "setDepthClear", viewport_setDepthClear);
            vm.addNativeMethod(lt, "addCompositor", addCompositor);
            vm.addNativeMethod(lt, "setCompositorEnabled", setCompositorEnabled);
            vm.addNativeMethod(lt, "removeCompositor", removeCompositor);


        // vm.addNativeMethod(lt, "setPosition", light_setPosition);
        // vm.addNativeMethod(lt, "setDiffuse", light_setDiffuse);

 
    }
}
  