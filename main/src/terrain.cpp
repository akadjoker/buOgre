#include "bindings.hpp"
#include <OgreTerrain.h>
#include <OgreTerrainGroup.h>

// ============== OGRE TERRAIN BINDINGS ==============

namespace OgreTerrainBindings
{
   
    // ========== TERRAIN GROUP METHODS ==========

    // defineTerrain(x, z, heightData)
    int terrain_defineTerrain(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 2)
        {
            Error("defineTerrain: requires x and z coordinates");
            return 0;
        }

        Ogre::TerrainGroup *tg = static_cast<Ogre::TerrainGroup *>(data);
        if (!tg) return 0;

        long x = (long)args[0].asNumber();
        long z = (long)args[1].asNumber();

        // Define terrain at position (will create flat terrain if no heightmap)
        tg->defineTerrain(x, z);

        return 0;
    }

    // loadAllTerrains()
    int terrain_loadAllTerrains(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::TerrainGroup *tg = static_cast<Ogre::TerrainGroup *>(data);
        if (tg)
        {
            tg->loadAllTerrains(true);
        }
        return 0;
    }

    // freeTemporaryResources()
    int terrain_freeTemporaryResources(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::TerrainGroup *tg = static_cast<Ogre::TerrainGroup *>(data);
        if (tg)
        {
            tg->freeTemporaryResources();
        }
        return 0;
    }

    // setOrigin(x, y, z)
    int terrain_setOrigin(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3) return 0;

        Ogre::TerrainGroup *tg = static_cast<Ogre::TerrainGroup *>(data);
        if (tg)
        {
            Ogre::Vector3 origin(
                (float)args[0].asNumber(),
                (float)args[1].asNumber(),
                (float)args[2].asNumber()
            );
            tg->setOrigin(origin);
        }
        return 0;
    }

    // setTerrainSize(size)
    int terrain_setTerrainSize(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::TerrainGroup *tg = static_cast<Ogre::TerrainGroup *>(data);
        if (tg)
        {
            tg->setTerrainSize((Ogre::uint16)args[0].asNumber());
        }
        return 0;
    }

    // setTerrainWorldSize(size)
    int terrain_setTerrainWorldSize(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::TerrainGroup *tg = static_cast<Ogre::TerrainGroup *>(data);
        if (tg)
        {
            tg->setTerrainWorldSize((float)args[0].asNumber());
        }
        return 0;
    }

    // getHeightAtWorldPosition(x, y, z) -> float
    int terrain_getHeightAtWorldPosition(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3)
        {
            vm->pushFloat(0.0f);
            return 1;
        }

        Ogre::TerrainGroup *tg = static_cast<Ogre::TerrainGroup *>(data);
        if (tg)
        {
            Ogre::Vector3 pos(
                (float)args[0].asNumber(),
                (float)args[1].asNumber(),
                (float)args[2].asNumber()
            );
            float height = tg->getHeightAtWorldPosition(pos);
            vm->pushFloat(height);
            return 1;
        }

        vm->pushFloat(0.0f);
        return 1;
    }

    // update()
    int terrain_update(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::TerrainGroup *tg = static_cast<Ogre::TerrainGroup *>(data);
        if (tg)
        {
            tg->update();
        }
        return 0;
    }

    // ========== HELPER FUNCTIONS ==========

    // configureDefaults(light)
    int terrain_configureDefaults(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            Error("configureDefaults: requires light");
            return 0;
        }

        Ogre::TerrainGroup *tg = static_cast<Ogre::TerrainGroup *>(data);

        NativeClassInstance *lightInstance = args[0].asNativeClassInstance();
        Ogre::Light *light = static_cast<Ogre::Light *>(lightInstance->userData);

        if (!tg || !light) return 0;

        // Configure default import settings
        Ogre::Terrain::ImportData &defaultimp = tg->getDefaultImportSettings();

        defaultimp.terrainSize = 513;
        defaultimp.worldSize = 1000.0f;
        defaultimp.inputScale = 50;  // Height scale
        defaultimp.minBatchSize = 33;
        defaultimp.maxBatchSize = 65;

        // Textures - usando paths relativos
        defaultimp.layerList.resize(3);

        // Layer 0 - Ground
        defaultimp.layerList[0].worldSize = 120;
        defaultimp.layerList[0].textureNames.push_back("Ground37_diffspec.dds");
        defaultimp.layerList[0].textureNames.push_back("Ground37_normheight.dds");

        // Layer 1 - Rock
        defaultimp.layerList[1].worldSize = 70;
        defaultimp.layerList[1].textureNames.push_back("Rock20_diffspec.dds");
        defaultimp.layerList[1].textureNames.push_back("Rock20_normheight.dds");

        // Layer 2 - Bricks
        defaultimp.layerList[2].worldSize = 200;
        defaultimp.layerList[2].textureNames.push_back("Bricks076C_diffspec.dds");
        defaultimp.layerList[2].textureNames.push_back("Bricks076C_normheight.dds");

        Info("Terrain defaults configured");
        return 0;
    }

    // generateRandom(x, z, seed)
    int terrain_generateRandom(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3)
        {
            Error("generateRandom: requires x, z, seed");
            return 0;
        }

        Ogre::TerrainGroup *tg = static_cast<Ogre::TerrainGroup *>(data);

        if (!tg) return 0;

        long x = (long)args[0].asNumber();
        long z = (long)args[1].asNumber();
        int seed = (int)args[2].asNumber();

        // Generate random heightmap
        Ogre::Terrain::ImportData &imp = tg->getDefaultImportSettings();

        // Create heightmap data
        int size = imp.terrainSize;
        float *heightData = new float[size * size];

        // Simple procedural generation (Perlin-like)
        srand(seed);
        for (int i = 0; i < size * size; ++i)
        {
            heightData[i] = ((float)rand() / RAND_MAX) * 0.5f;
        }

        // Smooth the terrain
        float *smoothData = new float[size * size];
        for (int y = 1; y < size - 1; ++y)
        {
            for (int x = 1; x < size - 1; ++x)
            {
                int idx = y * size + x;
                smoothData[idx] = (
                    heightData[idx - 1] +
                    heightData[idx + 1] +
                    heightData[idx - size] +
                    heightData[idx + size] +
                    heightData[idx] * 4.0f
                ) / 8.0f;
            }
        }

        delete[] heightData;
        imp.inputImage = new Ogre::Image();
        imp.inputImage->loadDynamicImage(
            (Ogre::uchar*)smoothData,
            size, size, 1,
            Ogre::PF_FLOAT32_R,
            true  // auto-delete height data with the image
        );

        // Use the generated height image directly to avoid .dat dependency
        tg->defineTerrain(x, z, imp.inputImage);

        delete imp.inputImage;;
        imp.inputImage = nullptr;

        Info("Random terrain generated at (%d, %d)", x, z);
        return 0;
    }

    // loadHeightmap(x, z, filename)
    int terrain_loadHeightmap(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3)
        {
            Error("loadHeightmap: requires x, z, filename");
            return 0;
        }

        Ogre::TerrainGroup *tg = static_cast<Ogre::TerrainGroup *>(data);
        if (!tg) return 0;

        long x = (long)args[0].asNumber();
        long z = (long)args[1].asNumber();
        const char *filename = args[2].asStringChars();

        try
        {
            Ogre::Image *img = new Ogre::Image();
            img->load(filename, "General");
            tg->defineTerrain(x, z, img);
            delete img;
            Info("Heightmap loaded for terrain (%d, %d): %s", x, z, filename);
        }
        catch (Ogre::Exception &e)
        {
            Error("loadHeightmap failed: %s", e.what());
        }

        return 0;
    }

    void registerAll(Interpreter &vm)
    {
        // Register TerrainGroup class
        NativeClassDef *terrain = vm.registerNativeClass(
            "TerrainGroup",
            nullptr,  // No constructor - use Scene.createTerrainGroup()
            nullptr,  // No destructor - managed externally
            0,        // no properties for now
            false
        );

        // Methods
        vm.addNativeMethod(terrain, "defineTerrain", terrain_defineTerrain);
        vm.addNativeMethod(terrain, "loadAllTerrains", terrain_loadAllTerrains);
        vm.addNativeMethod(terrain, "freeTemporaryResources", terrain_freeTemporaryResources);
        vm.addNativeMethod(terrain, "setOrigin", terrain_setOrigin);
        vm.addNativeMethod(terrain, "setTerrainSize", terrain_setTerrainSize);
        vm.addNativeMethod(terrain, "setTerrainWorldSize", terrain_setTerrainWorldSize);
        vm.addNativeMethod(terrain, "getHeightAtWorldPosition", terrain_getHeightAtWorldPosition);
        vm.addNativeMethod(terrain, "update", terrain_update);
        vm.addNativeMethod(terrain, "configureDefaults", terrain_configureDefaults);
        vm.addNativeMethod(terrain, "generateRandom", terrain_generateRandom);
        vm.addNativeMethod(terrain, "loadHeightmap", terrain_loadHeightmap);

        Info("Terrain bindings registered");
    }

} // namespace OgreTerrainBindings
