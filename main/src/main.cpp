
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include "interpreter.hpp"
#include "bindings.hpp"
#include "platform.hpp"
#include "game_object.hpp"
#include <unistd.h> // for access()
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <sys/stat.h>

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreSceneNode.h>
#include <OgreFrameListener.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>
#include <OgreMaterialManager.h>
#include <OgreTextureManager.h>
#include <OgreMeshManager.h>
#include <OgreRTShaderSystem.h>
#include <OgreMovablePlane.h>
#include <OgreFrameListener.h>

 

using namespace Ogre;

struct FileLoaderContext
{
    const char *searchPaths[8];
    int pathCount;
    char fullPath[512];
    char buffer[1024 * 1024];
};

const char *multiPathFileLoader(const char *filename, size_t *outSize, void *userdata)
{
    FileLoaderContext *ctx = (FileLoaderContext *)userdata;

    for (int i = 0; i < ctx->pathCount; i++)
    {
        snprintf(ctx->fullPath, sizeof(ctx->fullPath), "%s/%s", ctx->searchPaths[i], filename);

        FILE *f = fopen(ctx->fullPath, "rb");
        if (!f)
            continue;

        fseek(f, 0, SEEK_END);
        long size = ftell(f);

        if (size <= 0)
        {
            fclose(f);
            continue;
        }

        if (size >= (long)sizeof(ctx->buffer))
        {
            fprintf(stderr, "File too large: %s (%ld bytes)\n", ctx->fullPath, size);
            fclose(f);
            *outSize = 0;
            return nullptr;
        }

        fseek(f, 0, SEEK_SET);
        size_t bytesRead = fread(ctx->buffer, 1, size, f);
        fclose(f);

        if (bytesRead != (size_t)size)
            continue;

        ctx->buffer[bytesRead] = '\0';
        *outSize = bytesRead;
        return ctx->buffer;
    }

    *outSize = 0;
    return nullptr;
}

// Helper: load file contents into a string
static std::string loadFile(const char *path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        fprintf(stderr, "Could not open file: %s\n", path);
        return "";
    }
    return std::string((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
}

// ============================================================
// Global Configuration Variables (set by script)
// ============================================================

int WINDDOW_WIDTH = 1280;
int WINDOW_HEIGHT = 720;
std::string WINDOW_TITLE = "BuOgre Engine";
int WINDOW_MONIOTR = 0;
bool FULLSCREEN = false;
bool CAN_RESIZE = true;
float BACKGROUND_R = 0.0f;
float BACKGROUND_G = 0.0f;
float BACKGROUND_B = 0.0f;

Ogre::Root *mRoot = nullptr;
//Ogre::SceneManager *scene = nullptr;
Ogre::RenderWindow *mWindow = nullptr;

SDL_Window *sdlWindow = nullptr;

// ============================================================
// Native Functions for Script Configuration
// ============================================================

void FreeResources()
{
    
    if (mRoot)
        delete mRoot;
    if (sdlWindow)
        SDL_DestroyWindow(sdlWindow);
    SDL_Quit();

 
    mRoot = nullptr;
    sdlWindow = nullptr;
}

// Configure window properties
// Usage: createEngine(width, height, "title", monitor, fullscreen, canResize)
int native_create_engine(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 6)
    {
        Error("ERROR: createEngine requires 6 arguments");
        Error("Usage: createEngine(width, height, \"title\", monitor, fullscreen, canResize)");
        vm->pushBool(false);
        return 1;
    }

    WINDDOW_WIDTH = (int)args[0].asNumber();
    WINDOW_HEIGHT = (int)args[1].asNumber();
    WINDOW_TITLE = args[2].asStringChars();
    WINDOW_MONIOTR = (int)args[3].asNumber();
    FULLSCREEN = args[4].asBool();
    CAN_RESIZE = args[5].asBool();

    // monitor

    try
    {

        int numDisplays = SDL_GetNumVideoDisplays();
        for (int i = 0; i < numDisplays; i++)
        {
            SDL_Rect bounds;
            SDL_GetDisplayBounds(i, &bounds);
            Info("Monitor %d (%s): %dx%d at (%d,%d)\n",
                 i, SDL_GetDisplayName(i),
                 bounds.w, bounds.h, bounds.x, bounds.y);
        }

        if (WINDOW_MONIOTR < 0 || WINDOW_MONIOTR >= numDisplays)
        {
            Info("Invalid monitor index %d, defaulting to 0", WINDOW_MONIOTR);
            WINDOW_MONIOTR = 0;
        }

        Uint32 sdlWindowFlags = SDL_WINDOW_SHOWN;
        if (CAN_RESIZE)
            sdlWindowFlags |= SDL_WINDOW_RESIZABLE;
        if (FULLSCREEN)
            sdlWindowFlags |= SDL_WINDOW_FULLSCREEN;

        // Create SDL window
        sdlWindow = SDL_CreateWindow(
            WINDOW_TITLE.c_str(),
            SDL_WINDOWPOS_CENTERED_DISPLAY(WINDOW_MONIOTR),
            SDL_WINDOWPOS_CENTERED_DISPLAY(WINDOW_MONIOTR),
            WINDDOW_WIDTH, WINDOW_HEIGHT,
            sdlWindowFlags);

        if (!sdlWindow)
        {
            Error("Failed to create SDL window: %s\n", SDL_GetError());
            SDL_Quit();
            vm->pushBool(false);
            return 1;
        }
        Info("✓ SDL window created");

        // Get native window handle
        SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);
        SDL_GetWindowWMInfo(sdlWindow, &wmInfo);

        // ==================== 3. OGRE ROOT ====================
        mRoot = new Root("plugins.cfg", "config.cfg", "ogre.log");
        Ogre::LogManager::getSingleton().setMinLogLevel(LogMessageLevel::LML_TRIVIAL);

        Info("✓ Ogre Root created");
        Ogre::ConfigFile cf;
        cf.load("resources.cfg");

        Ogre::String name, locType;
        Ogre::ConfigFile::SectionIterator secIt = cf.getSectionIterator();

        while (secIt.hasMoreElements())
        {
            Ogre::ConfigFile::SettingsMultiMap *settings = secIt.getNext();
            Ogre::ConfigFile::SettingsMultiMap::iterator it;

            for (it = settings->begin(); it != settings->end(); ++it)
            {
                locType = it->first;
                name = it->second;
                SDL_Log("%s  %s", name.c_str(), locType.c_str());

                Ogre::ResourceGroupManager::getSingleton().addResourceLocation(name, locType);
            }
        }

        const RenderSystemList &renderers = mRoot->getAvailableRenderers();
        if (renderers.empty())
        {
            Error("No render systems available!");
            delete mRoot;
            SDL_DestroyWindow(sdlWindow);
            SDL_Quit();
            vm->pushBool(false);
            return 1;
        }

        RenderSystem *renderSystem = renderers[0];
        Info("Using: %s", renderSystem->getName().c_str());
        mRoot->setRenderSystem(renderSystem);
        renderSystem->setConfigOption("Full Screen", FULLSCREEN ? "Yes" : "No");
        renderSystem->setConfigOption("VSync", "Yes");

        // ==================== 6. INITIALIZE ROOT ====================
        mRoot->initialise(false);
        Info("✓ Ogre initialized");

        // ==================== 7. CREATE OGRE WINDOW ====================
        NameValuePairList params;
        params["externalWindowHandle"] = StringConverter::toString((size_t)wmInfo.info.x11.window);
        params["externalGLContext"] = StringConverter::toString((size_t)wmInfo.info.x11.display);
        params["FSAA"] = "0";
        params["vsync"] = "true";

        mWindow = mRoot->createRenderWindow("MainWindow", WINDDOW_WIDTH, WINDOW_HEIGHT, false, &params);
        mWindow->setVisible(true);
        Ogre::Root::getSingleton().getRenderSystem()->_initRenderTargets();
        Ogre::Root::getSingleton().clearEventTimes();
        Info("✓ Ogre window created");

        Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
        Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

        vm->pushBool(true);
    }
    catch (const std::exception &e)
    {
        Error("Exception during engine creation: %s", e.what());
        vm->pushBool(false);
        FreeResources();
    }

    return 1;
}

int native_relase_engine(Interpreter *vm, int argCount, Value *args)
{
    FreeResources();
    Info("✓ Engine resources freed");
    return 0;
}

 



int native_engine_can_update(Interpreter *vm, int argCount, Value *args)
{
    if (!mRoot)
    {
        Error("ERROR: can_update called before engine created");
        vm->pushBool(false);
        return 1;
    }
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        // Update input state for all events
        InputBindings::updateInput(event);
        
        if (event.type == SDL_QUIT)
        {
            vm->pushBool(false);
            return 1;
        }
        else if (event.type == SDL_KEYDOWN)
        {
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                vm->pushBool(false);
                return 1;
            }
        }
        else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
        {
            mWindow->resize(event.window.data1, event.window.data2);
            mWindow->windowMovedOrResized();
        }
    }
    
    // Update input state tracking (previous frame state)
    InputBindings::updateInputState();
    
    // Update timer (delta time, FPS, elapsed time)
    TimerBindings::updateTimer();

    vm->pushBool(true);
    return 1;
}

int native_engine_update(Interpreter *vm, int argCount, Value *args)
{
    if (!mRoot)
    {
        Error("ERROR: engine update called before engine created");
        vm->pushBool(false);
        return 1;
    }
    if (argCount < 1)
    {
        Error("ERROR: engine_update requires 1 argument (time elapsed in seconds)");
        vm->pushBool(false);
        return 1;
    }

    double time = args[0].asNumber();
    (void)time;

    if (!mRoot->renderOneFrame())
    {
        vm->pushBool(false);
        return 1;
    }

    InputBindings::updateInputState();

    vm->pushBool(true);
    return 1;
}

void writeRenderToFile(const Ogre::String &filename)
{
    Image img(PF_A8B8G8R8, mWindow->getWidth(), mWindow->getHeight());
    PixelBox pb = img.getPixelBox();
    mWindow->copyContentsToMemory(pb, pb);
    img.save(filename);
}

void TakeScreenshot(const char *fileName)
{
    mWindow->writeContentsToFile(fileName);
    SDL_LogInfo(0, "SYSTEM: [%s] Screenshot taken successfully", fileName);
}

int main(int argc, char *argv[])
{
    Interpreter vm;

    vm.registerAll();
    ScriptManager engine(&vm);
    engine.registerNatives(); // exposes createGameObject() to script

    vm.registerNative("CreateEngine", native_create_engine, 6);
    vm.registerNative("ReleaseEngine", native_relase_engine, 0);
    vm.registerNative("UpdateEngine", native_engine_update, 1);
    vm.registerNative("EngineCanUpdate", native_engine_can_update, 0);
    

    // Register Ogre3D bindings

    OgreBindings::registerAll(vm);

    FileLoaderContext ctx;
    ctx.searchPaths[0] = "./bin";
    ctx.searchPaths[1] = "./scripts";
    ctx.searchPaths[2] = "./bin/scripts";
    ctx.searchPaths[3] = ".";
    ctx.pathCount = 4;
    vm.setFileLoader(multiPathFileLoader, &ctx);

    // try
    // {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
    {
        fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }
    Info("✓ SDL initialized");

    const char *scriptFile = nullptr;

    if (argc > 1)
    {
        if (OsFileExists(argv[1]))
        {
            scriptFile = argv[1];
        }
        else
        {
            fprintf(stderr, "Specified script file does not exist: %s\n", argv[1]);
            SDL_Quit();
            return 1;
        }
    }

    if (!scriptFile)
    {
        if (OsFileExists("scripts/main.bu"))
        {
            scriptFile = "scripts/main.bu";
        }
        else if (OsFileExists("main.bu"))
        {
            scriptFile = "main.bu";
        }
        else
        {
            fprintf(stderr, "No script file specified and no default found.\n");
            SDL_Quit();
            return 1;
        }
    }

    if (!engine.load(scriptFile))
    {
        fprintf(stderr, "Failed to load main script: %s\n", scriptFile);
        SDL_Quit();
        return 1;
    }

    //         // monitor
    // try
    // {

    //         int numDisplays = SDL_GetNumVideoDisplays();
    //         for (int i = 0; i < numDisplays; i++)
    //         {
    //             SDL_Rect bounds;
    //             SDL_GetDisplayBounds(i, &bounds);
    //             Info("Monitor %d (%s): %dx%d at (%d,%d)\n",
    //                  i, SDL_GetDisplayName(i),
    //                  bounds.w, bounds.h, bounds.x, bounds.y);
    //         }

    //         if (WINDOW_MONIOTR < 0 || WINDOW_MONIOTR >= numDisplays)
    //         {
    //             Info("Invalid monitor index %d, defaulting to 0", WINDOW_MONIOTR);
    //             WINDOW_MONIOTR = 0;
    //         }

    //         Uint32 sdlWindowFlags = SDL_WINDOW_SHOWN;
    //         if (CAN_RESIZE)
    //             sdlWindowFlags |= SDL_WINDOW_RESIZABLE;
    //         if (FULLSCREEN)
    //             sdlWindowFlags |= SDL_WINDOW_FULLSCREEN;

    //         // Create SDL window
    //         sdlWindow = SDL_CreateWindow(
    //             WINDOW_TITLE.c_str(),
    //             SDL_WINDOWPOS_CENTERED_DISPLAY(WINDOW_MONIOTR),
    //             SDL_WINDOWPOS_CENTERED_DISPLAY(WINDOW_MONIOTR),
    //             WINDDOW_WIDTH, WINDOW_HEIGHT,
    //             sdlWindowFlags);

    //         if (!sdlWindow)
    //         {
    //             Error("Failed to create SDL window: %s\n", SDL_GetError());
    //             SDL_Quit();
    //             return 1;
    //         }
    //         Info("✓ SDL window created");

    //         // Get native window handle
    //         SDL_SysWMinfo wmInfo;
    //         SDL_VERSION(&wmInfo.version);
    //         SDL_GetWindowWMInfo(sdlWindow, &wmInfo);

    //         // ==================== 3. OGRE ROOT ====================
    //         mRoot = new Root("plugins.cfg", "config.cfg", "ogre.log");
    //         Ogre::LogManager::getSingleton().setMinLogLevel(LogMessageLevel::LML_TRIVIAL);
    //         Info("✓ Ogre Root created");
    //         Ogre::ConfigFile cf;
    //         cf.load("resources.cfg");

    //         Ogre::String name, locType;
    //         Ogre::ConfigFile::SectionIterator secIt = cf.getSectionIterator();

    //         while (secIt.hasMoreElements())
    //         {
    //             Ogre::ConfigFile::SettingsMultiMap *settings = secIt.getNext();
    //             Ogre::ConfigFile::SettingsMultiMap::iterator it;

    //             for (it = settings->begin(); it != settings->end(); ++it)
    //             {
    //                 locType = it->first;
    //                 name = it->second;
    //                 SDL_Log("%s  %s", name.c_str(), locType.c_str());

    //                 Ogre::ResourceGroupManager::getSingleton().addResourceLocation(name, locType);
    //             }
    //         }

    //         const RenderSystemList &renderers = mRoot->getAvailableRenderers();
    //         if (renderers.empty())
    //         {
    //             Error("No render systems available!");
    //             delete mRoot;
    //             SDL_DestroyWindow(sdlWindow);
    //             SDL_Quit();
    //             return 1;
    //         }

    //         RenderSystem *renderSystem = renderers[0];
    //         Info("Using: %s", renderSystem->getName().c_str());
    //         mRoot->setRenderSystem(renderSystem);
    //         renderSystem->setConfigOption("Full Screen", FULLSCREEN ? "Yes" : "No");
    //         renderSystem->setConfigOption("VSync", "Yes");

    //         // ==================== 6. INITIALIZE ROOT ====================
    //         mRoot->initialise(false);
    //         Info("✓ Ogre initialized");

    //         // ==================== 7. CREATE OGRE WINDOW ====================
    //         NameValuePairList params;
    //         params["externalWindowHandle"] = StringConverter::toString((size_t)wmInfo.info.x11.window);
    //         params["externalGLContext"] = StringConverter::toString((size_t)wmInfo.info.x11.display);
    //         params["FSAA"] = "0";
    //         params["vsync"] = "true";

    //         mWindow = mRoot->createRenderWindow("MainWindow", WINDDOW_WIDTH, WINDOW_HEIGHT, false, &params);
    //         mWindow->setVisible(true);
    //         Ogre::Root::getSingleton().getRenderSystem()->_initRenderTargets();
    //         Ogre::Root::getSingleton().clearEventTimes();
    //         Info("✓ Ogre window created");

    //         Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
    //         Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    //         scene = mRoot->createSceneManager();

    //         scene->setShadowTechnique(Ogre::ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);
    //         // sm->setShadowTextureCasterMaterial(MaterialManager::getSingleton().getByName("DeferredShading/Shadows/Caster", "General"));
    //         // mSceneMgr->setShadowTextureCount(1);
    //         //  mSceneMgr->setShadowFarDistance(150);
    //         //  mSceneMgr->setShadowTextureConfig( 0, 512, 512, PF_FLOAT16_R, 0, 2 );
    //         //   mSceneMgr->setShadowDirectionalLightExtrusionDistance(75);
    //         Ogre::LogManager::getSingleton().setMinLogLevel(Ogre::LML_TRIVIAL);

    //         SceneNode *root_node = scene->getRootSceneNode();

    //         camera = scene->createCamera("MainCamera");
    //         camera->setNearClipDistance(1.0f);
    //         camera->setAutoAspectRatio(true);
    //         SceneNode *mTarget = root_node->createChildSceneNode();
    //         mTarget->lookAt(Vector3{0, 0, -1}, Node::TS_PARENT);
    //         mTarget->setPosition(0, 20, 500);
    //         mTarget->attachObject(camera);
    //         mCamMan = new CameraMan(mTarget);
    //         mCamMan->setStyle(CS_ORBIT);
    //         mCamMan->setYawPitchDist(Ogre::Degree(45), Ogre::Degree(45), 200);

    //         Ogre::Viewport *mViewport = mWindow->addViewport(camera);
    //         mViewport->setBackgroundColour(Ogre::ColourValue(0.8, 0.8, 1));

    //         Light *pointLight = scene->createLight("PointLight");
    //         pointLight->setType(Ogre::Light::LT_POINT);
    //         pointLight->setDiffuseColour(Ogre::ColourValue::White);
    //         SceneNode *light_node = root_node->createChildSceneNode();
    //         light_node->setPosition(0, 150, -250);
    //         light_node->attachObject(pointLight);

    //         Plane plane(Vector3::UNIT_Y, 0);
    //         MeshManager::getSingleton().createPlane(
    //             "ground", RGN_DEFAULT,
    //             plane,
    //             1500, 1500, 20, 20,
    //             true,
    //             1, 5, 5,
    //             Vector3::UNIT_Z);
    //         // Entity *groundEntity = scene->createEntity("ground");
    //         // scene->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);

    //         Entity *ogreHead = scene->createEntity("ogrehead.mesh");
    //         ogreHead->setCastShadows(true);
    //         SceneNode *ogreHeadNode = scene->getRootSceneNode()->createChildSceneNode();
    //         ogreHeadNode->attachObject(ogreHead);
    //         ogreHeadNode->setPosition(84, 10, 0);

    //         // ==================== 12. MAIN LOOP ====================
    //         auto lastTime = std::chrono::high_resolution_clock::now();
    //         bool running = true;

    //         while (running)
    //         {
    //             SDL_Event event;
    //             while (SDL_PollEvent(&event))
    //             {
    //                 if (event.type == SDL_QUIT)
    //                 {
    //                     running = false;
    //                 }
    //                 else if (event.type == SDL_KEYDOWN)
    //                 {
    //                     if (event.key.keysym.sym == SDLK_ESCAPE)
    //                         running = false;
    //                     mCamMan->onKeyDown(event.key);
    //                 }
    //                 else if (event.type == SDL_KEYUP)
    //                 {
    //                     mCamMan->onKeyUp(event.key);
    //                 }
    //                 else if (event.type == SDL_MOUSEMOTION)
    //                 {
    //                     mCamMan->onMouseMove(event.motion);
    //                 }
    //                 else if (event.type == SDL_MOUSEBUTTONDOWN)
    //                 {
    //                     mCamMan->onMouseDown(event.button);
    //                 }
    //                 else if (event.type == SDL_MOUSEBUTTONUP)
    //                 {
    //                     mCamMan->onMouseUp(event.button);
    //                 }
    //                 else if (event.type == SDL_MOUSEWHEEL)
    //                 {
    //                     mCamMan->onMouseWheel(event.wheel);
    //                 }
    //                 else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
    //                 {
    //                     mWindow->resize(event.window.data1, event.window.data2);
    //                     mWindow->windowMovedOrResized();
    //                 }
    //             }

    //             auto currentTime = std::chrono::high_resolution_clock::now();
    //             std::chrono::duration<float> elapsed = currentTime - lastTime;
    //             lastTime = currentTime;
    //             mCamMan->onUpdate(elapsed.count());

    //             if (!mRoot->renderOneFrame())
    //                 break;
    //         }

    //         Info("Game loop ended");
    //     }
    //     catch (const Exception &e)
    //     {
    //         Error("Ogre Exception: %s", e.getFullDescription().c_str());
    //         if (mCamMan)
    //             delete mCamMan;
    //         if (mRoot)
    //             delete mRoot;
    //         if (sdlWindow)
    //             SDL_DestroyWindow(sdlWindow);
    //         SDL_Quit();
    //         return 1;
    //     }

    //     if (mCamMan)
    //         delete mCamMan;

    //     // Cleanup
    //     if (mRoot)
    //         delete mRoot;
    //     if (sdlWindow)
    //         SDL_DestroyWindow(sdlWindow);
    //     SDL_Quit();

    Info("Engine shutdown complete");
    return 0;
}