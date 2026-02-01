
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
#include <Ogre.h>
#include <chrono>

#include <Ogre.h>
#include <OgreRTShaderSystem.h>
#include <sys/stat.h>

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

// ============================================================
// Native Functions for Script Configuration
// ============================================================

// Configure window properties
// Usage: setWindow(width, height, "title", monitor, fullscreen, canResize)
int native_set_window(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 6)
    {
        fprintf(stderr, "ERROR: setWindow requires 6 arguments\n");
        fprintf(stderr, "Usage: setWindow(width, height, title, monitor, fullscreen, canResize)\n");
        fprintf(stderr, "Example: setWindow(1280, 720, \"My Game\", 0, false, true)\n");
        return 0;
    }

    WINDDOW_WIDTH = (int)args[0].asNumber();
    WINDOW_HEIGHT = (int)args[1].asNumber();
    WINDOW_TITLE = args[2].asStringChars();
    WINDOW_MONIOTR = (int)args[3].asNumber();
    FULLSCREEN = args[4].asBool();
    CAN_RESIZE = args[5].asBool();

    Info("✓ Window configured: %dx%d, title='%s', monitor=%d, fullscreen=%s, resizable=%s",
         WINDDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE.c_str(), WINDOW_MONIOTR,
         FULLSCREEN ? "yes" : "no", CAN_RESIZE ? "yes" : "no");

    return 0;
}

// Set background/clear color
// Usage: setBackgroundColor(red, green, blue)
// Values: 0.0 to 1.0
int native_set_background_color(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 3)
    {
        fprintf(stderr, "ERROR: setBackgroundColor requires 3 arguments\n");
        fprintf(stderr, "Usage: setBackgroundColor(red, green, blue)\n");
        fprintf(stderr, "Example: setBackgroundColor(0.2, 0.2, 0.2)  // Dark gray\n");
        return 0;
    }

    BACKGROUND_R = (float)args[0].asNumber();
    BACKGROUND_G = (float)args[1].asNumber();
    BACKGROUND_B = (float)args[2].asNumber();

    // Clamp values to 0.0-1.0
    if (BACKGROUND_R < 0.0f)
        BACKGROUND_R = 0.0f;
    if (BACKGROUND_R > 1.0f)
        BACKGROUND_R = 1.0f;
    if (BACKGROUND_G < 0.0f)
        BACKGROUND_G = 0.0f;
    if (BACKGROUND_G > 1.0f)
        BACKGROUND_G = 1.0f;
    if (BACKGROUND_B < 0.0f)
        BACKGROUND_B = 0.0f;
    if (BACKGROUND_B > 1.0f)
        BACKGROUND_B = 1.0f;

    //  Info("✓ Background color set to RGB(%.2f, %.2f, %.2f)", BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);

    return 0;
}

using namespace Ogre;

class SilentLogListener : public Ogre::LogListener {
public:
    void messageLogged(const Ogre::String&, Ogre::LogMessageLevel, bool, const Ogre::String&, bool&) override {}
};

int main(int argc, char *argv[])
{
    Interpreter vm;

    vm.registerAll();
    ScriptManager engine(&vm);
    engine.registerNatives(); // exposes createGameObject() to script

    vm.registerNative("setWindow", native_set_window, 6);
    vm.registerNative("setBackgroundColor", native_set_background_color, 3);

    // Register Ogre3D bindings

    OgreBindings::registerAll(vm);

    FileLoaderContext ctx;
    ctx.searchPaths[0] = "./bin";
    ctx.searchPaths[1] = "./scripts";
    ctx.searchPaths[2] = "./bin/scripts";
    ctx.searchPaths[3] = ".";
    ctx.pathCount = 4;
    vm.setFileLoader(multiPathFileLoader, &ctx);

    Ogre::Root *root = nullptr;
    Ogre::RenderWindow *window = nullptr;
    SDL_Window *sdlWindow = nullptr;

    try
    {
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
                return 1;
            }
        }

        if (!engine.load(scriptFile))
        {
            fprintf(stderr, "Failed to load main script: %s\n", scriptFile);
            return 1;
        }

        // monitor

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
            return 1;
        }
        Info("✓ SDL window created");

        // Get native window handle
        SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);
        SDL_GetWindowWMInfo(sdlWindow, &wmInfo);

        // Set library path
        std::string ogreLibPath = "/media/projectos/projects/cpp/buOgre/bin/lib";
        const char *currentPath = getenv("LD_LIBRARY_PATH");
        std::string newPath = ogreLibPath;
        if (currentPath)
            newPath = newPath + ":" + currentPath;
        setenv("LD_LIBRARY_PATH", newPath.c_str(), 1);
        
        root = new Ogre::Root("", "", "ogre.log");
        SilentLogListener listener;
        Ogre::LogManager::getSingleton().getDefaultLog()->addListener(&listener);
        Ogre::LogManager::getSingleton().getDefaultLog()->setDebugOutputEnabled(false);
        // Create Ogre Root
        

        // Load RenderSystem plugin
        Info("Loading RenderSystem_GL3Plus...");
        std::vector<std::string> pluginPaths = {
            ogreLibPath + "/RenderSystem_GL3Plus.so.14.5",
            ogreLibPath + "/RenderSystem_GL3Plus.so"};

        bool pluginLoaded = false;
        for (const auto &path : pluginPaths)
        {
            if (access(path.c_str(), F_OK) == 0)
            {
                try
                {
                    root->loadPlugin(path);
                    Info("✓ Loaded plugin: %s", path.c_str());
                    pluginLoaded = true;
                    break;
                }
                catch (const Ogre::Exception &e)
                {
                    Error("Failed: %s\n", e.what());
                }
            }
        }

        if (!pluginLoaded)
        {
            Error("ERROR: Could not load RenderSystem_GL3Plus\n");
            SDL_DestroyWindow(sdlWindow);
            SDL_Quit();
            delete root;
            return 1;
        }

        // Set render system
        const Ogre::RenderSystemList &renderers = root->getAvailableRenderers();
        if (renderers.empty())
        {
            Error("ERROR: No render systems available!\n");
            SDL_DestroyWindow(sdlWindow);
            SDL_Quit();
            delete root;
            return 1;
        }

        Ogre::RenderSystem *renderSystem = renderers[0];
        Info("Using: %s", renderSystem->getName().c_str());
        root->setRenderSystem(renderSystem);

        // Configure render system
        renderSystem->setConfigOption("Full Screen", "No");
        renderSystem->setConfigOption("VSync", "Yes");

        // Initialize Ogre WITHOUT creating a window (we use SDL's)
        root->initialise(false);

      

        // Create Ogre render window from SDL window
        Ogre::NameValuePairList params;
        params["externalWindowHandle"] = Ogre::StringConverter::toString((size_t)wmInfo.info.x11.window);
        params["externalGLContext"] = Ogre::StringConverter::toString((size_t)wmInfo.info.x11.display);
        params["FSAA"] = "0";
        params["vsync"] = "true";

        window = root->createRenderWindow("MainWindow", WINDDOW_WIDTH, WINDOW_HEIGHT, FULLSCREEN, &params);
        window->setVisible(true);

        Info("✓ Ogre window created from SDL");

        if (Ogre::RTShader::ShaderGenerator::initialize())
        {
            Info("✓ RTSS initialized");
            RTShader::ShaderGenerator *shadergen = RTShader::ShaderGenerator::getSingletonPtr();
 
            std::string cacheDir = "./shader_cache";
            mkdir(cacheDir.c_str(), 0755);
            shadergen->setShaderCachePath(cacheDir);

            shadergen->setTargetLanguage("glsl");
            
        }
        else
        {
            Error("Failed to initialize RTSS!");
            delete root;
            return 1;
        }


        std::string path = "/media/projectos/projects/cpp/buOgre/bin/media/";

        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path + "Main", "FileSystem");
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path + "RTShaderLib", "FileSystem");
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path + "Terrain", "FileSystem");


 

        path = "/media/projectos/projects/cpp/buOgre/bin/assets/";

        // ANTES de todos os outros addResourceLocation:
        // Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
        //     path + "RTShaderLib", "FileSystem");
        // Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
        //     path + "RTShaderLib/materials", "FileSystem");
        // Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
        //     path + "RTShaderLib/GLSL", "FileSystem");
        // Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
        //     path + "RTShaderLib/GLSL150", "FileSystem");

        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path + "PBR", "FileSystem");
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path + "PBR/filament", "FileSystem");
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path + "materials/programs/GLSL", "FileSystem");
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path + "materials/programs/GLSL120", "FileSystem");
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path + "materials/programs/GLSL150", "FileSystem");
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path + "materials/programs/GLSL400", "FileSystem");
        // Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path+ "materials/programs/GLSLES", "FileSystem");
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path + "materials/programs/SPIRV", "FileSystem");
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path + "materials/scripts", "FileSystem");
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path + "materials/textures", "FileSystem");
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path + "materials/textures/terrain", "FileSystem");
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path + "models", "FileSystem");
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path + "particle", "FileSystem");
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path + "DeferredShadingMedia", "FileSystem");
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path + "DeferredShadingMedia/DeferredShading/post", "FileSystem");
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path + "PCZAppMedia", "FileSystem");
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path + "materials/scripts/SSAO", "FileSystem");
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path + "materials/textures/SSAO", "FileSystem");
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path + "volumeTerrain", "FileSystem");
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path + "CSMShadows", "FileSystem");

        // Inicializar resources
        Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

        Info("✓ Resources loaded");

        SceneManager *scnMgr = root->createSceneManager();

        


        // register our scene with the RTSS
        RTShader::ShaderGenerator *shadergen = RTShader::ShaderGenerator::getSingletonPtr();
        shadergen->addSceneManager(scnMgr);

        // -- tutorial section start --
        //! [turnlights]
        scnMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));
        //! [turnlights]

        //! [newlight]
        Light *light = scnMgr->createLight("MainLight");
        SceneNode *lightNode = scnMgr->getRootSceneNode()->createChildSceneNode();
        lightNode->attachObject(light);
        //! [newlight]

        //! [lightpos]
        lightNode->setPosition(20, 80, 50);
        //! [lightpos]

        //! [camera]
        SceneNode *camNode = scnMgr->getRootSceneNode()->createChildSceneNode();

        // create the camera
        Camera *cam = scnMgr->createCamera("myCam");
        cam->setNearClipDistance(5); // specific to this sample
        cam->setAutoAspectRatio(true);
        camNode->attachObject(cam);
        camNode->setPosition(0, 0, 140);


        Ogre::Viewport* vp = window->addViewport(cam);
        vp->setBackgroundColour(Ogre::ColourValue(0.2f, 0.3f, 0.4f));
        vp->setMaterialScheme(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME); //
        

Entity* ogreEntity = scnMgr->createEntity("ogrehead.mesh");
    //! [entity1]

    //! [entity1node]
    SceneNode* ogreNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    //! [entity1node]

    //! [entity1nodeattach]
    ogreNode->attachObject(ogreEntity);
    //! [entity1nodeattach]

    //! [cameramove]
    camNode->setPosition(0, 47, 222);


        auto lastTime = std::chrono::high_resolution_clock::now();
        bool running = true;

        while (running)
        {
            // Process SDL events
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                case SDL_QUIT:
                    Info("Window close requested");
                    running = false;
                    break;

                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        Info("ESC pressed - exiting");
                        running = false;
                    }
                    else if (event.key.keysym.sym == SDLK_F11)
                    {
                        Uint32 flags = SDL_GetWindowFlags(sdlWindow);
                        if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
                        {
                            SDL_SetWindowFullscreen(sdlWindow, 0);
                            Info("Fullscreen OFF");
                        }
                        else
                        {
                            SDL_SetWindowFullscreen(sdlWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
                            Info("Fullscreen ON");
                        }
                    }
                    break;

                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                    {
                        window->resize(event.window.data1, event.window.data2);
                        window->windowMovedOrResized();
                    }
                    break;
                }
            }

            if (!running)
                break;

            // Calculate delta time
            auto currentTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> delta = currentTime - lastTime;
            float dt = delta.count();
            lastTime = currentTime;

            // Update game
            //  engine.updateAll(dt);

            // Render
            if (!root->renderOneFrame())
                break;
        }

        Info("Game loop ended");
    }
    catch (const Ogre::Exception &e)
    {
        fprintf(stderr, "Ogre Exception: %s\n", e.getFullDescription().c_str());
        if (sdlWindow)
            SDL_DestroyWindow(sdlWindow);
        SDL_Quit();
        if (root)
            delete root;
        return 1;
    }

    // Cleanup
    if (root)
        delete root;
    if (sdlWindow)
        SDL_DestroyWindow(sdlWindow);
    SDL_Quit();

    Info("Engine shutdown complete");
    return 0;
}