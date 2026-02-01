
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include "interpreter.hpp"
#include "bindings.hpp"
#include "platform.hpp"
#include "game_object.hpp"



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
// GameObject demo: C++ is just the engine, scripts define the game
// ============================================================


int main(int argc, char **argv)
{
    Interpreter vm;

    vm.registerAll();
    ScriptManager engine(&vm);
    engine.registerNatives();  // exposes createGameObject() to script

    FileLoaderContext ctx;
    ctx.searchPaths[0] = "./bin";
    ctx.searchPaths[1] = "./scripts";
    ctx.searchPaths[2] = "./bin/scripts";
    ctx.searchPaths[3] = ".";
    ctx.pathCount = 4;
    vm.setFileLoader(multiPathFileLoader, &ctx);

    const char* scriptFile = nullptr;

    if (argc>1)
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

    engine.startAll();
    float dt = 1.0f / 60.0f;
    for (int frame = 0; frame < 10; frame++)
    {
        engine.updateAll(dt);
        engine.renderAll();
    }

    vm.unloadAllPlugins();
    return 0;
}
