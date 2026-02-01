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


        // vm.addNativeMethod(lt, "setPosition", light_setPosition);
        // vm.addNativeMethod(lt, "setDiffuse", light_setDiffuse);

 
    }
}
  