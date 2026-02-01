#include "bindings.hpp"
#include <OgreOverlayManager.h>
#include <OgreOverlay.h>
#include <OgreOverlayContainer.h>
#include <OgreOverlayElement.h>
#include <OgreTextAreaOverlayElement.h>
#include <OgrePanelOverlayElement.h>

// ============== OGRE OVERLAY BINDINGS ==============
// 2D UI system (HUD, menus, debug text, FPS counter)

namespace OgreOverlayBindings
{
    // ========== OVERLAY CREATION ==========

    // createOverlay(name) - cria overlay container
    int createOverlay(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            Error("createOverlay: requires name");
            return 0;
        }

        const char *name = args[0].asStringChars();

        try
        {
            Ogre::Overlay *overlay = Ogre::OverlayManager::getSingleton().create(name);

            if (!overlay)
            {
                Error("createOverlay: failed to create overlay '%s'", name);
                return 0;
            }

            // Get the Overlay NativeClassDef
            NativeClassDef *overlayClass = nullptr;
            if (!vm->tryGetNativeClassDef("Overlay", &overlayClass))
            {
                Error("Overlay class not found in VM");
                return 0;
            }

            Value overlayValue = vm->makeNativeClassInstance(false);
            NativeClassInstance *instance = overlayValue.asNativeClassInstance();
            instance->klass = overlayClass;
            instance->userData = (void *)overlay;

            vm->push(overlayValue);
            Info("Overlay '%s' created", name);
            return 1;
        }
        catch (Ogre::Exception &e)
        {
            Error("createOverlay failed: %s", e.what());
            return 0;
        }
    }

    // createTextArea(name) - global function to create text area
    int createTextArea(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            Error("createTextArea: requires name");
            return 0;
        }

        const char *name = args[0].asStringChars();

        try
        {
            Ogre::OverlayElement *element = Ogre::OverlayManager::getSingleton().createOverlayElement(
                "TextArea", name
            );

            if (!element)
            {
                Error("createTextArea: failed to create text area '%s'", name);
                return 0;
            }

            NativeClassDef *textClass = nullptr;
            if (!vm->tryGetNativeClassDef("TextArea", &textClass))
            {
                Error("TextArea class not found in VM");
                return 0;
            }

            Value textValue = vm->makeNativeClassInstance(false);
            NativeClassInstance *instance = textValue.asNativeClassInstance();
            instance->klass = textClass;
            instance->userData = (void *)element;

            vm->push(textValue);
            Info("TextArea '%s' created", name);
            return 1;
        }
        catch (Ogre::Exception &e)
        {
            Error("createTextArea failed: %s", e.what());
            return 0;
        }
    }

    // createPanel(name) - global function to create panel
    int createPanel(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            Error("createPanel: requires name");
            return 0;
        }

        const char *name = args[0].asStringChars();

        try
        {
            Ogre::OverlayElement *element = Ogre::OverlayManager::getSingleton().createOverlayElement(
                "Panel", name
            );

            if (!element)
            {
                Error("createPanel: failed to create panel '%s'", name);
                return 0;
            }

            NativeClassDef *panelClass = nullptr;
            if (!vm->tryGetNativeClassDef("Panel", &panelClass))
            {
                Error("Panel class not found in VM");
                return 0;
            }

            Value panelValue = vm->makeNativeClassInstance(false);
            NativeClassInstance *instance = panelValue.asNativeClassInstance();
            instance->klass = panelClass;
            instance->userData = (void *)element;

            vm->push(panelValue);
            Info("Panel '%s' created", name);
            return 1;
        }
        catch (Ogre::Exception &e)
        {
            Error("createPanel failed: %s", e.what());
            return 0;
        }
    }

    // ========== OVERLAY METHODS ==========

    // show()
    int overlay_show(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::Overlay *overlay = static_cast<Ogre::Overlay *>(data);
        if (overlay) overlay->show();
        return 0;
    }

    // hide()
    int overlay_hide(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::Overlay *overlay = static_cast<Ogre::Overlay *>(data);
        if (overlay) overlay->hide();
        return 0;
    }

    // setZOrder(z)
    int overlay_setZOrder(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::Overlay *overlay = static_cast<Ogre::Overlay *>(data);
        if (overlay) overlay->setZOrder((uint16_t)args[0].asNumber());
        return 0;
    }

    // addElement(element)
    int overlay_addElement(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::Overlay *overlay = static_cast<Ogre::Overlay *>(data);
        NativeClassInstance *elementInstance = args[0].asNativeClassInstance();
        Ogre::OverlayElement *element = static_cast<Ogre::OverlayElement *>(elementInstance->userData);

        if (overlay && element)
        {
            // Cast to container first
            Ogre::OverlayContainer *container = dynamic_cast<Ogre::OverlayContainer *>(element);
            if (container)
            {
                overlay->add2D(container);
            }
            else
            {
                // If it's not a container, we need to create one or add to existing
                Error("addElement: element must be a container (Panel)");
            }
        }

        return 0;
    }

    // ========== TEXT AREA METHODS ==========

    // setCaption(text)
    int textarea_setCaption(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::OverlayElement *element = static_cast<Ogre::OverlayElement *>(data);
        if (element)
        {
            element->setCaption(args[0].asStringChars());
        }
        return 0;
    }

    // setPosition(x, y)
    int textarea_setPosition(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 2) return 0;

        Ogre::OverlayElement *element = static_cast<Ogre::OverlayElement *>(data);
        if (element)
        {
            element->setPosition(
                (float)args[0].asNumber(),
                (float)args[1].asNumber()
            );
        }
        return 0;
    }

    // setDimensions(width, height)
    int textarea_setDimensions(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 2) return 0;

        Ogre::OverlayElement *element = static_cast<Ogre::OverlayElement *>(data);
        if (element)
        {
            element->setDimensions(
                (float)args[0].asNumber(),
                (float)args[1].asNumber()
            );
        }
        return 0;
    }

    // setMetricsMode(mode)
    // 0 = GMM_PIXELS, 1 = GMM_RELATIVE
    int textarea_setMetricsMode(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::OverlayElement *element = static_cast<Ogre::OverlayElement *>(data);
        if (element)
        {
            int mode = (int)args[0].asNumber();
            Ogre::GuiMetricsMode gmm = (mode == 0) ? Ogre::GMM_PIXELS : Ogre::GMM_RELATIVE;
            element->setMetricsMode(gmm);
        }
        return 0;
    }

    // setCharHeight(height)
    int textarea_setCharHeight(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::OverlayElement *element = static_cast<Ogre::OverlayElement *>(data);
        Ogre::TextAreaOverlayElement *textArea = dynamic_cast<Ogre::TextAreaOverlayElement *>(element);

        if (textArea)
        {
            textArea->setCharHeight((float)args[0].asNumber());
        }
        return 0;
    }

    // setFontName(fontName)
    int textarea_setFontName(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::OverlayElement *element = static_cast<Ogre::OverlayElement *>(data);
        Ogre::TextAreaOverlayElement *textArea = dynamic_cast<Ogre::TextAreaOverlayElement *>(element);

        if (textArea)
        {
            textArea->setFontName(args[0].asStringChars());
        }
        return 0;
    }

    // setColour(r, g, b, a)
    int textarea_setColour(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 4) return 0;

        Ogre::OverlayElement *element = static_cast<Ogre::OverlayElement *>(data);
        Ogre::TextAreaOverlayElement *textArea = dynamic_cast<Ogre::TextAreaOverlayElement *>(element);

        if (textArea)
        {
            Ogre::ColourValue color(
                (float)args[0].asNumber(),
                (float)args[1].asNumber(),
                (float)args[2].asNumber(),
                (float)args[3].asNumber()
            );
            textArea->setColour(color);
        }
        return 0;
    }

    // ========== PANEL METHODS ==========

    // setMaterialName(materialName)
    int panel_setMaterialName(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::OverlayElement *element = static_cast<Ogre::OverlayElement *>(data);
        if (element)
        {
            element->setMaterialName(args[0].asStringChars());
        }
        return 0;
    }

    // addChild(childElement)
    int panel_addChild(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::OverlayElement *element = static_cast<Ogre::OverlayElement *>(data);
        Ogre::OverlayContainer *container = dynamic_cast<Ogre::OverlayContainer *>(element);

        NativeClassInstance *childInstance = args[0].asNativeClassInstance();
        Ogre::OverlayElement *child = static_cast<Ogre::OverlayElement *>(childInstance->userData);

        if (container && child)
        {
            container->addChild(child);
        }

        return 0;
    }

    // ========== HELPER: CREATE FPS OVERLAY ==========

    int createFPSOverlay(Interpreter *vm, int argCount, Value *args)
    {
        try
        {
            // Create overlay
            Ogre::Overlay *overlay = Ogre::OverlayManager::getSingleton().create("FPSOverlay");

            // Create panel container
            Ogre::OverlayContainer *panel = static_cast<Ogre::OverlayContainer *>(
                Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "FPSPanel")
            );
            panel->setMetricsMode(Ogre::GMM_PIXELS);
            panel->setPosition(10, 10);
            panel->setDimensions(200, 60);

            // Create FPS text
            Ogre::TextAreaOverlayElement *fpsText = static_cast<Ogre::TextAreaOverlayElement *>(
                Ogre::OverlayManager::getSingleton().createOverlayElement("TextArea", "FPSText")
            );
            fpsText->setMetricsMode(Ogre::GMM_PIXELS);
            fpsText->setPosition(0, 0);
            fpsText->setDimensions(200, 20);
            fpsText->setCharHeight(16);
            fpsText->setFontName("BlueHighway");
            fpsText->setColour(Ogre::ColourValue(1.0, 1.0, 0.0));
            fpsText->setCaption("FPS: 0");

            panel->addChild(fpsText);
            overlay->add2D(panel);
            overlay->show();

            Info("FPS Overlay created");
            vm->pushBool(true);
            return 1;
        }
        catch (Ogre::Exception &e)
        {
            Error("createFPSOverlay failed: %s", e.what());
            vm->pushBool(false);
            return 1;
        }
    }

    // updateFPSText(fps) - helper to update FPS counter
    int updateFPSText(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        try
        {
            Ogre::OverlayElement *fpsText = Ogre::OverlayManager::getSingleton().getOverlayElement("FPSText");
            if (fpsText)
            {
                int fps = (int)args[0].asNumber();
                char buffer[64];
                sprintf(buffer, "FPS: %d", fps);
                fpsText->setCaption(buffer);
            }
        }
        catch (Ogre::Exception &e)
        {
            // Silently fail if element doesn't exist
        }

        return 0;
    }

    void registerAll(Interpreter &vm)
    {
        // Register Overlay class
        NativeClassDef *overlay = vm.registerNativeClass(
            "Overlay",
            nullptr,
            nullptr,
            0,
            false
        );

        vm.addNativeMethod(overlay, "show", overlay_show);
        vm.addNativeMethod(overlay, "hide", overlay_hide);
        vm.addNativeMethod(overlay, "setZOrder", overlay_setZOrder);
        vm.addNativeMethod(overlay, "addElement", overlay_addElement);

        // Register TextArea class
        NativeClassDef *textarea = vm.registerNativeClass(
            "TextArea",
            nullptr,
            nullptr,
            0,
            false
        );

        vm.addNativeMethod(textarea, "setCaption", textarea_setCaption);
        vm.addNativeMethod(textarea, "setPosition", textarea_setPosition);
        vm.addNativeMethod(textarea, "setDimensions", textarea_setDimensions);
        vm.addNativeMethod(textarea, "setMetricsMode", textarea_setMetricsMode);
        vm.addNativeMethod(textarea, "setCharHeight", textarea_setCharHeight);
        vm.addNativeMethod(textarea, "setFontName", textarea_setFontName);
        vm.addNativeMethod(textarea, "setColour", textarea_setColour);

        // Register Panel class
        NativeClassDef *panel = vm.registerNativeClass(
            "Panel",
            nullptr,
            nullptr,
            0,
            false
        );

        vm.addNativeMethod(panel, "setPosition", textarea_setPosition);
        vm.addNativeMethod(panel, "setDimensions", textarea_setDimensions);
        vm.addNativeMethod(panel, "setMetricsMode", textarea_setMetricsMode);
        vm.addNativeMethod(panel, "setMaterialName", panel_setMaterialName);
        vm.addNativeMethod(panel, "addChild", panel_addChild);

        // Global functions
        vm.registerNative("createOverlay", createOverlay, 1);
        vm.registerNative("createTextArea", createTextArea, 1);
        vm.registerNative("createPanel", createPanel, 1);
        vm.registerNative("createFPSOverlay", createFPSOverlay, 0);
        vm.registerNative("updateFPSText", updateFPSText, 1);

        Info("Overlay bindings registered");
    }

} // namespace OgreOverlayBindings
