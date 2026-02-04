#include "bindings.hpp"
#include <OgreOverlaySystem.h>

extern Ogre::Root *mRoot;
extern Ogre::OverlaySystem *overlaySystem;

namespace OgreSceneBindings
{
    // Constructor: Scene() - creates the SceneManager (or returns existing)
    void *scene_ctor(Interpreter *vm, int argCount, Value *args)
    {
        if (!mRoot)
        {
            Error("Scene: engine not created yet (call CreateEngine first)");
            return nullptr;
        }
        Ogre::SceneManager *scene = mRoot->createSceneManager();

        scene->addRenderQueueListener(overlaySystem);

        return scene;
    }

    void scene_dtor(Interpreter *vm, void *data) {}

    // getRoot() -> SceneNode (the root node)
    int scene_getRoot(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::SceneManager *sm = static_cast<Ogre::SceneManager *>(data);
        Ogre::SceneNode *node = sm->getRootSceneNode();

        // Get the SceneNode NativeClassDef
        NativeClassDef *sceneNodeClass = nullptr;
        if (!vm->tryGetNativeClassDef("SceneNode", &sceneNodeClass))
        {
            Error("SceneNode class not found in VM");
            return 0;
        }

        // Create a NativeClassInstance of SceneNode
        // Use false so GC can manage the wrapper (Ogre owns the actual node)
        Value nodeValue = vm->makeNativeClassInstance(false);
        NativeClassInstance *instance = nodeValue.asNativeClassInstance();
        instance->klass = sceneNodeClass;
        instance->userData = (void *)node;

        vm->push(nodeValue);
        return 1;
    }

    // // createEntity(meshName) -> Entity pointer
    int scene_createEntity(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
            return 0;

        try
        {

            Ogre::SceneManager *sm = static_cast<Ogre::SceneManager *>(data);
            const char *meshName = args[0].asStringChars();
            Ogre::Entity *entity = sm->createEntity(meshName);

            NativeClassDef *entityClass = nullptr;
            if (!vm->tryGetNativeClassDef("Entity", &entityClass))
            {
                Error("Entity class not found in VM");
                return 0;
            }
            Value entityValue = vm->makeNativeClassInstance(false);
            NativeClassInstance *instance = entityValue.asNativeClassInstance();
            instance->klass = entityClass;
            instance->userData = (void *)entity;

            vm->push(entityValue);
        }
        catch (const std::exception &e)
        {

            vm->pushNil();
            Error("createEntity exception: %s", e.what());
        }

        return 1;
    }

    // createLight(name, type) -> Light
    // type: 0 = point, 1 = directional, 2 = spotlight
    int scene_createLight(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
            return 0;
        Ogre::SceneManager *sm = static_cast<Ogre::SceneManager *>(data);
        const char *name = args[0].asStringChars();

        try
        {

            Ogre::Light::LightTypes ltype = Ogre::Light::LT_POINT;
            if (argCount >= 2)
            {
                int t = (int)args[1].asNumber();
                if (t == 1)
                    ltype = Ogre::Light::LT_DIRECTIONAL;
                else if (t == 2)
                    ltype = Ogre::Light::LT_SPOTLIGHT;
            }

            Ogre::Light *light = sm->createLight(name);
            light->setType(ltype);

            // Get Light NativeClassDef
            NativeClassDef *lightClass = nullptr;
            if (!vm->tryGetNativeClassDef("Light", &lightClass))
            {
                Error("Light class not found in VM");
                return 0;
            }

            // Return Light instance
            Value lightValue = vm->makeNativeClassInstance(false);
            NativeClassInstance *instance = lightValue.asNativeClassInstance();
            instance->klass = lightClass;
            instance->userData = (void *)light;

            vm->push(lightValue);
        }
        catch (const std::exception &e)
        {
            vm->pushNil();
            Error("createLight exception: %s", e.what());
        }
        return 1;
    }

    // createCamera(name) -> Camera
    int scene_createCamera(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
            return 0;
        Ogre::SceneManager *sm = static_cast<Ogre::SceneManager *>(data);
        const char *name = args[0].asStringChars();
        try
        {
            Ogre::Camera *camera = sm->createCamera(name);
            // Get Camera NativeClassDef
            NativeClassDef *cameraClass = nullptr;
            if (!vm->tryGetNativeClassDef("Camera", &cameraClass))
            {
                Error("Camera class not found in VM");
                return 0;
            }
            // Return Camera instance
            Value cameraValue = vm->makeNativeClassInstance(false);
            NativeClassInstance *instance = cameraValue.asNativeClassInstance();
            instance->klass = cameraClass;
            instance->userData = (void *)camera;
            vm->push(cameraValue);
        }
        catch (const std::exception &e)
        {
            vm->pushNil();
            Error("createCamera exception: %s", e.what());
        }
        return 1;
    }

    // setAmbientLight(r, g, b)
    int scene_setAmbientLight(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3)
            return 0;
        Ogre::SceneManager *sm = static_cast<Ogre::SceneManager *>(data);
        sm->setAmbientLight(Ogre::ColourValue(
            (float)args[0].asNumber(),
            (float)args[1].asNumber(),
            (float)args[2].asNumber()));
        return 0;
    }

    // setShadowTechnique(type)
    int scene_setShadowTechnique(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
            return 0;
        Ogre::SceneManager *sm = static_cast<Ogre::SceneManager *>(data);
        int t = (int)args[0].asNumber();

        Ogre::ShadowTechnique tech = Ogre::SHADOWTYPE_NONE;
        switch (t)
        {
        case 1:
            tech = Ogre::SHADOWTYPE_STENCIL_ADDITIVE;
            break;
        case 2:
            tech = Ogre::SHADOWTYPE_STENCIL_MODULATIVE;
            break;
        case 3:
            tech = Ogre::SHADOWTYPE_TEXTURE_ADDITIVE;
            break;
        case 4:
            tech = Ogre::SHADOWTYPE_TEXTURE_MODULATIVE;
            break;
        }

        sm->setShadowTechnique(tech);

        return 0;
    }

    int scene_setShadowTextureSize(Interpreter *vm, void *data, int argCount, Value *args)
    {

        Ogre::SceneManager *sm = static_cast<Ogre::SceneManager *>(data);

        int size = (int)args[0].asNumber();

        sm->setShadowTextureSize(size);

        return 0;
    }

    int scene_setShadowTextureCount(Interpreter *vm, void *data, int argCount, Value *args)
    {

        Ogre::SceneManager *sm = static_cast<Ogre::SceneManager *>(data);

        int count = (int)args[0].asNumber();

        sm->setShadowTextureCount(count);

        return 0;
    }
    // setShadowFarDistance(distance)
    int scene_setShadowFarDistance(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
            return 0;
        Ogre::SceneManager *sm = static_cast<Ogre::SceneManager *>(data);
        float distance = (float)args[0].asNumber();
        sm->setShadowFarDistance(distance);
        return 0;
    }

    // setShadowDirLightTextureOffset(offset)
    int scene_setShadowDirLightTextureOffset(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
            return 0;
        Ogre::SceneManager *sm = static_cast<Ogre::SceneManager *>(data);
        float offset = (float)args[0].asNumber();
        sm->setShadowDirLightTextureOffset(offset);
        return 0;
    }

    // setShadowTextureSelfShadow(enabled)
    int scene_setShadowTextureSelfShadow(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
            return 0;
        Ogre::SceneManager *sm = static_cast<Ogre::SceneManager *>(data);
        bool enabled = args[0].asBool();
        sm->setShadowTextureSelfShadow(enabled);
        return 0;
    }

    // setShadowCasterRenderBackFaces(enabled)
    int scene_setShadowCasterRenderBackFaces(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
            return 0;
        Ogre::SceneManager *sm = static_cast<Ogre::SceneManager *>(data);
        bool enabled = args[0].asBool();
        sm->setShadowCasterRenderBackFaces(enabled);
        return 0;
    }

    // setShadowColour(r, g, b)
    int scene_setShadowColour(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3)
            return 0;
        Ogre::SceneManager *sm = static_cast<Ogre::SceneManager *>(data);
        float r = (float)args[0].asNumber();
        float g = (float)args[1].asNumber();
        float b = (float)args[2].asNumber();
        sm->setShadowColour(Ogre::ColourValue(r, g, b));
        return 0;
    }

    // setShadowTexturePixelFormat(format)
    // format: 0=PF_X8R8G8B8, 1=PF_FLOAT16_R, 2=PF_FLOAT32_R
    int scene_setShadowTexturePixelFormat(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
            return 0;
        Ogre::SceneManager *sm = static_cast<Ogre::SceneManager *>(data);

        int format = (int)args[0].asNumber();
        Ogre::PixelFormat pf = Ogre::PF_X8R8G8B8;

        switch (format)
        {
        case 1:
            pf = Ogre::PF_FLOAT16_R;
            break;
        case 2:
            pf = Ogre::PF_FLOAT32_R;
            break;
        default:
            pf = Ogre::PF_X8R8G8B8;
            break;
        }

        sm->setShadowTexturePixelFormat(pf);
        return 0;
    }

    // setShadowTextureCountPerLightType(lightType, count)
    // lightType: 0=point, 1=directional, 2=spotlight
    int scene_setShadowTextureCountPerLightType(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 2)
            return 0;
        Ogre::SceneManager *sm = static_cast<Ogre::SceneManager *>(data);

        int lightType = (int)args[0].asNumber();
        size_t count = (size_t)args[1].asNumber();

        Ogre::Light::LightTypes lt = Ogre::Light::LT_POINT;
        switch (lightType)
        {
        case 1:
            lt = Ogre::Light::LT_DIRECTIONAL;
            break;
        case 2:
            lt = Ogre::Light::LT_SPOTLIGHT;
            break;
        default:
            lt = Ogre::Light::LT_POINT;
            break;
        }

        sm->setShadowTextureCountPerLightType(lt, count);
        return 0;
    }

    // setSkyBox(scene, enabled, materialName, distance, drawFirst)
    int scene_setSkyBox(Interpreter *vm, void *data, int argCount, Value *args)
    {

        Ogre::SceneManager *scene = static_cast<Ogre::SceneManager *>(data);

        if (!scene)
        {
            Error("setSkyBox: invalid scene");
            return 0;
        }

        bool enabled = args[0].asBool();

        const char *materialName = "Examples/SpaceSkyBox";
        float distance = 5000.0f;
        bool drawFirst = true;

        materialName = args[1].asStringChars();

        distance = (float)args[2].asNumber();

        drawFirst = args[3].asBool();
        try
        {
            scene->setSkyBox(enabled, materialName, distance, drawFirst);
        }
        catch (Ogre::Exception &e)
        {
            Error("setSkyBox failed: %s", e.what());
            return 0;
        }

        return 0;
    }

    // setSkyDome( enabled, materialName, curvature, tiling, distance, drawFirst)
    int scene_setSkyDome(Interpreter *vm, void *data, int argCount, Value *args)
    {

        Ogre::SceneManager *scene = static_cast<Ogre::SceneManager *>(data);

        if (!scene)
        {
            Error("setSkyDome: invalid scene");
            return 0;
        }

        bool enabled = args[0].asBool();

        if (!enabled)
        {
            scene->setSkyDome(false, "");
            Info("Skydome disabled");
            return 0;
        }

        const char *materialName = "Examples/CloudySky";
        float curvature = 10.0f;
        float tiling = 8.0f;
        float distance = 4000.0f;
        bool drawFirst = true;

        materialName = args[1].asStringChars();
        curvature = (float)args[2].asNumber();
        tiling = (float)args[3].asNumber();
        distance = (float)args[4].asNumber();
        drawFirst = args[5].asBool();

        try
        {
            scene->setSkyDome(enabled, materialName, curvature, tiling, distance, drawFirst);
        }
        catch (Ogre::Exception &e)
        {
            Error("setSkyDome failed: %s", e.what());
            return 0;
        }

        return 0;
    }

    // setSkyPlane( enabled, plane, materialName, scale, tiling, drawFirst, bow)
    // plane: 0=XY, 1=XZ, 2=YZ
    int scene_setSkyPlane(Interpreter *vm, void *data, int argCount, Value *args)
    {

        Ogre::SceneManager *scene = static_cast<Ogre::SceneManager *>(data);

        if (!scene)
        {
            Error("setSkyPlane: invalid scene");
            return 0;
        }

        bool enabled = args[0].asBool();

        // Default plane is Y=1000 (horizontal, above camera)
        Ogre::Plane plane;
        plane.normal = Ogre::Vector3::UNIT_Y;
        plane.d = 1000;

        const char *materialName = "Examples/CloudyNoonSkyBox";
        float scale = 1000.0f;
        float tiling = 10.0f;
        bool drawFirst = true;
        float bow = 0.0f;

        materialName = args[1].asStringChars();
        scale = (float)args[2].asNumber();
        tiling = (float)args[3].asNumber();
        drawFirst = args[4].asBool();
        bow = (float)args[5].asNumber();

        try
        {
            scene->setSkyPlane(enabled, plane, materialName, scale, tiling, drawFirst, bow);
        }
        catch (Ogre::Exception &e)
        {
            Error("setSkyPlane failed: %s", e.what());
            return 0;
        }

        return 0;
    }

    // setSkyPlaneCustom( enabled, normalX, normalY, normalZ, distance, materialName, scale, tiling)
    int scene_setSkyPlaneCustom(Interpreter *vm, void *data, int argCount, Value *args)
    {

        Ogre::SceneManager *scene = static_cast<Ogre::SceneManager *>(data);

        if (!scene)
        {
            Error("setSkyPlaneCustom: invalid scene");
            return 0;
        }

        bool enabled = args[0].asBool();

        Ogre::Plane plane;
        plane.normal = Ogre::Vector3(
            (float)args[1].asNumber(),
            (float)args[2].asNumber(),
            (float)args[3].asNumber());
        plane.d = (float)args[4].asNumber();

        const char *materialName = "Examples/CloudyNoonSkyBox";
        float scale = 1000.0f;
        float tiling = 10.0f;
        bool drawFirst = true;
        float bow = 0.0f;

        materialName = args[5].asStringChars();
        scale = (float)args[6].asNumber();
        tiling = (float)args[7].asNumber();
        drawFirst = args[8].asBool();
        bow = (float)args[9].asNumber();

        try
        {
            scene->setSkyPlane(enabled, plane, materialName, scale, tiling, drawFirst, bow);
        }
        catch (Ogre::Exception &e)
        {
            Error("setSkyPlaneCustom failed: %s", e.what());
        }

        return 0;
    }

    // setFog( mode, color, density, start, end)
    // mode: 0=NONE, 1=EXP, 2=EXP2, 3=LINEAR
    int scene_setFog(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::SceneManager *scene = static_cast<Ogre::SceneManager *>(data);

        if (!scene)
        {
            Error("setFog: invalid scene");
            return 0;
        }

        int mode = (int)args[0].asNumber();

        // Default fog parameters
        Ogre::ColourValue color(0.5f, 0.5f, 0.5f);
        float density = 0.001f;
        float start = 0.0f;
        float end = 1000.0f;

        // Color (r, g, b)

        color.r = (float)args[1].asNumber();
        color.g = (float)args[2].asNumber();
        color.b = (float)args[3].asNumber();

        // Density (for EXP/EXP2)

        density = (float)args[4].asNumber();

        // Start distance (for LINEAR)

        start = (float)args[5].asNumber();

        // End distance (for LINEAR)

        end = (float)args[6].asNumber();

        Ogre::FogMode fogMode;
        const char *modeName = "";

        switch (mode)
        {
        case 1:
            fogMode = Ogre::FOG_EXP;
            modeName = "EXP";
            scene->setFog(fogMode, color, density);
            break;
        case 2:
            fogMode = Ogre::FOG_EXP2;
            modeName = "EXP2";
            scene->setFog(fogMode, color, density);
            break;
        case 3:
            fogMode = Ogre::FOG_LINEAR;
            modeName = "LINEAR";
            scene->setFog(fogMode, color, density, start, end);
            break;
        default:
            fogMode = Ogre::FOG_NONE;
            modeName = "NONE";
            scene->setFog(fogMode);
            break;
        }

        Info("Fog enabled: %s (color=%.2f,%.2f,%.2f)", modeName, color.r, color.g, color.b);
        return 0;
    }

    // disableFog(scene)
    int scene_disableFog(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            Error("disableFog: requires scene");
            return 0;
        }

        NativeClassInstance *sceneInstance = args[0].asNativeClassInstance();
        Ogre::SceneManager *scene = static_cast<Ogre::SceneManager *>(sceneInstance->userData);

        if (scene)
        {
            scene->setFog(Ogre::FOG_NONE);
            Info("Fog disabled");
        }

        return 0;
    }

    int native_create_scene(Interpreter *vm, int argCount, Value *args)
    {

        NativeClassDef *sceneNodeClass = nullptr;
        if (!vm->tryGetNativeClassDef("Scene", &sceneNodeClass))
        {
            Error("Scene class not found in VM");
            return 0;
        }
        Ogre::SceneManager *scene = mRoot->createSceneManager();

        // // ========== CONFIGURAÇÃO SIMPLES DE TEXTURE SHADOWS ==========
        // // Usar TEXTURE_MODULATIVE (não INTEGRATED) - mais simples
        // scene->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);

        // // Formato normal RGB (não depth)
        // scene->setShadowTexturePixelFormat(Ogre::PF_X8R8G8B8);

        // // Configurações básicas
        // scene->setShadowColour(Ogre::ColourValue(0.5, 0.5, 0.5));
        // scene->setShadowTextureSize(2048);     // Maior para melhor qualidade
        // scene->setShadowTextureCount(3);       // 3 texturas para CSM
        // scene->setShadowDirLightTextureOffset(0.6);  // Anti shadow-acne
        // scene->setShadowFarDistance(300.0);    // Distância maior
        // scene->setShadowTextureSelfShadow(true);
        // scene->setShadowCasterRenderBackFaces(false);

        // // SEM LiSPSM - usar default (mais simples e funcional)
        // // scene->setShadowCameraSetup(Ogre::DefaultShadowCameraSetup::create());

        // // LUZ DIRECIONAL (não point) - funciona melhor para texture shadows
        // Ogre::Light* light = scene->createLight(Ogre::Light::LT_DIRECTIONAL);
        // light->setDiffuseColour(1.0f, 0.95f, 0.9f);
        // light->setSpecularColour(Ogre::ColourValue::White);

        // Ogre::SceneNode* lightNode = scene->getRootSceneNode()->createChildSceneNode();
        // lightNode->attachObject(light);
        // lightNode->setDirection(0.3f, -0.75f, 0.5f);  // Aponta para baixo


        scene->addRenderQueueListener(overlaySystem);
        Value nodeValue = vm->makeNativeClassInstance(false);
        NativeClassInstance *instance = nodeValue.asNativeClassInstance();
        instance->klass = sceneNodeClass;
        instance->userData = (void *)scene;
        vm->push(nodeValue);
        return 1;
    }

    int scene_createCube(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3)
        {
            Error("createCube: requires name, size, material [group]");
            vm->pushNil();
            return 1;
        }

        try
        {
            Ogre::SceneManager *scene = static_cast<Ogre::SceneManager *>(data);
            const char *name = args[0].asStringChars();
            float size = (float)args[1].asNumber();
            const char *material = args[2].asStringChars();

            const char *group = NULL;
            if (argCount >= 4)
            {
                group = args[3].asStringChars();
            }

            Ogre::ManualObject *manual = scene->createManualObject(name);
            if (!manual)
            {
                Error("createCube: failed to create manual object '%s'", name);
                vm->pushNil();
                return 1;
            }

            ProceduralMesh::createCube(manual, material, size, group);

            NativeClassDef *objClass = nullptr;
            if (!vm->tryGetNativeClassDef("ManualObject", &objClass))
            {
                Error("ManualObject class not found in VM");
                vm->pushNil();
                return 1;
            }

            Value objValue = vm->makeNativeClassInstance(false);
            NativeClassInstance *instance = objValue.asNativeClassInstance();
            instance->klass = objClass;
            instance->userData = (void *)manual;

            vm->push(objValue);
        }
        catch (const std::exception &e)
        {
            Error("createCube exception: %s", e.what());
            vm->pushNil();
        }

        return 1;
    }

    int scene_createSphere(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 5)
        {
            Error("createSphere: requires name, radius, rings, segments, material [group]");
            vm->pushNil();
            return 1;
        }

        try
        {
            Ogre::SceneManager *scene = static_cast<Ogre::SceneManager *>(data);
            const char *name = args[0].asStringChars();
            float radius = (float)args[1].asNumber();
            int rings = (int)args[2].asNumber();
            int segments = (int)args[3].asNumber();
            const char *material = args[4].asStringChars();

            const char *group = NULL;
            if (argCount >= 6)
            {
                group = args[5].asStringChars();
            }

            Ogre::ManualObject *manual = scene->createManualObject(name);
            if (!manual)
            {
                Error("createSphere: failed to create manual object '%s'", name);
                vm->pushNil();
                return 1;
            }

            ProceduralMesh::createSphere(manual, material, radius, rings, segments, group);

            NativeClassDef *objClass = nullptr;
            if (!vm->tryGetNativeClassDef("ManualObject", &objClass))
            {
                Error("ManualObject class not found in VM");
                vm->pushNil();
                return 1;
            }

            Value objValue = vm->makeNativeClassInstance(false);
            NativeClassInstance *instance = objValue.asNativeClassInstance();
            instance->klass = objClass;
            instance->userData = (void *)manual;

            vm->push(objValue);
        }
        catch (const std::exception &e)
        {
            Error("createSphere exception: %s", e.what());
            vm->pushNil();
        }

        return 1;
    }

    // ========== CREATE CYLINDER ==========

    int scene_createCylinder(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 5)
        {
            Error("createCylinder: requires name, radius, height, segments, material [group]");
            vm->pushNil();
            return 1;
        }

        try
        {
            Ogre::SceneManager *scene = static_cast<Ogre::SceneManager *>(data);
            const char *name = args[0].asStringChars();
            float radius = (float)args[1].asNumber();
            float height = (float)args[2].asNumber();
            int segments = (int)args[3].asNumber();
            const char *material = args[4].asStringChars();

            const char *group = NULL;
            if (argCount >= 6)
            {
                group = args[5].asStringChars();
            }

            Ogre::ManualObject *manual = scene->createManualObject(name);
            if (!manual)
            {
                Error("createCylinder: failed to create manual object '%s'", name);
                vm->pushNil();
                return 1;
            }

            ProceduralMesh::createCylinder(manual, material, radius, height, segments, group);

            NativeClassDef *objClass = nullptr;
            if (!vm->tryGetNativeClassDef("ManualObject", &objClass))
            {
                Error("ManualObject class not found in VM");
                vm->pushNil();
                return 1;
            }

            Value objValue = vm->makeNativeClassInstance(false);
            NativeClassInstance *instance = objValue.asNativeClassInstance();
            instance->klass = objClass;
            instance->userData = (void *)manual;

            vm->push(objValue);
        }
        catch (const std::exception &e)
        {
            Error("createCylinder exception: %s", e.what());
            vm->pushNil();
        }

        return 1;
    }

    int scene_createCone(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 5)
        {
            Error("createCone: requires name, radius, height, segments, material [group]");
            vm->pushNil();
            return 1;
        }

        try
        {
            Ogre::SceneManager *scene = static_cast<Ogre::SceneManager *>(data);
            const char *name = args[0].asStringChars();
            float radius = (float)args[1].asNumber();
            float height = (float)args[2].asNumber();
            int segments = (int)args[3].asNumber();
            const char *material = args[4].asStringChars();

            const char *group = NULL;
            if (argCount >= 6)
            {
                group = args[5].asStringChars();
            }

            Ogre::ManualObject *manual = scene->createManualObject(name);
            if (!manual)
            {
                Error("createCone: failed to create manual object '%s'", name);
                vm->pushNil();
                return 1;
            }

            ProceduralMesh::createCone(manual, material, radius, height, segments, group);

            NativeClassDef *objClass = nullptr;
            if (!vm->tryGetNativeClassDef("ManualObject", &objClass))
            {
                Error("ManualObject class not found in VM");
                vm->pushNil();
                return 1;
            }

            Value objValue = vm->makeNativeClassInstance(false);
            NativeClassInstance *instance = objValue.asNativeClassInstance();
            instance->klass = objClass;
            instance->userData = (void *)manual;

            vm->push(objValue);
        }
        catch (const std::exception &e)
        {
            Error("createCone exception: %s", e.what());
            vm->pushNil();
        }

        return 1;
    }

    int scene_createTorus(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 6)
        {
            Error("createTorus: requires name, majorRadius, minorRadius, majorSegments, minorSegments, material [group]");
            vm->pushNil();
            return 1;
        }

        try
        {
            Ogre::SceneManager *scene = static_cast<Ogre::SceneManager *>(data);
            const char *name = args[0].asStringChars();
            float majorRadius = (float)args[1].asNumber();
            float minorRadius = (float)args[2].asNumber();
            int majorSegments = (int)args[3].asNumber();
            int minorSegments = (int)args[4].asNumber();
            const char *material = args[5].asStringChars();

            const char *group = NULL;
            if (argCount >= 7)
            {
                group = args[6].asStringChars();
            }

            Ogre::ManualObject *manual = scene->createManualObject(name);
            if (!manual)
            {
                Error("createTorus: failed to create manual object '%s'", name);
                vm->pushNil();
                return 1;
            }

            ProceduralMesh::createTorus(manual, material, majorRadius, minorRadius, majorSegments, minorSegments, group);

            NativeClassDef *objClass = nullptr;
            if (!vm->tryGetNativeClassDef("ManualObject", &objClass))
            {
                Error("ManualObject class not found in VM");
                vm->pushNil();
                return 1;
            }

            Value objValue = vm->makeNativeClassInstance(false);
            NativeClassInstance *instance = objValue.asNativeClassInstance();
            instance->klass = objClass;
            instance->userData = (void *)manual;

            vm->push(objValue);
        }
        catch (const std::exception &e)
        {
            Error("createTorus exception: %s", e.what());
            vm->pushNil();
        }

        return 1;
    }

    int scene_createQuad(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 4)
        {
            Error("createQuad: requires name, width, height, material [group]");
            return 0;
        }

        try
        {
            Ogre::SceneManager *sm = static_cast<Ogre::SceneManager *>(data);
            const char *name = args[0].asStringChars();
            float width = (float)args[1].asNumber();
            float height = (float)args[2].asNumber();
            const char *material = args[3].asStringChars();
            const char *group = nullptr;
            if (argCount >= 5)
                group = args[4].asStringChars();

            Ogre::ManualObject *manual = sm->createManualObject(Ogre::String(name) + "_manual");
            if (!manual)
            {
                Error("createQuad: failed to create manual object '%s'", name);
                vm->pushNil();
                return 1;
            }

            ProceduralMesh::createQuad(manual, material, width, height, group);

            NativeClassDef *objClass = nullptr;
            if (!vm->tryGetNativeClassDef("ManualObject", &objClass))
            {
                Error("ManualObject class not found in VM");
                vm->pushNil();
                return 1;
            }

            Value objValue = vm->makeNativeClassInstance(false);
            NativeClassInstance *instance = objValue.asNativeClassInstance();
            instance->klass = objClass;
            instance->userData = (void *)manual;

            vm->push(objValue);
        }
        catch (const std::exception &e)
        {
            Error("createQuad exception: %s", e.what());
            vm->pushNil();
        }

        return 1;
    }

    int scene_createPlane(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 5)
        {
            Error("createPlane: requires name, width, depth, widthSegments, depthSegments, material [group]");
            return 0;
        }

        try
        {
            Ogre::SceneManager *sm = static_cast<Ogre::SceneManager *>(data);
            const char *name = args[0].asStringChars();
            float width = (float)args[1].asNumber();
            float depth = (float)args[2].asNumber();
            int widthSegments = (int)args[3].asNumber();
            int depthSegments = (int)args[4].asNumber();
            const char *material = argCount >= 6 ? args[5].asStringChars() : "BaseWhiteNoLighting";
            const char *group = nullptr;
            if (argCount >= 7)
                group = args[6].asStringChars();

            Ogre::ManualObject *manual = sm->createManualObject(Ogre::String(name) + "_manual");
            if (!manual)
            {
                Error("createPlane: failed to create manual object '%s'", name);
                vm->pushNil();
                return 1;
            }

            ProceduralMesh::createPlane(manual, material, width, depth, widthSegments, depthSegments, group);

            NativeClassDef *objClass = nullptr;
            if (!vm->tryGetNativeClassDef("ManualObject", &objClass))
            {
                Error("ManualObject class not found in VM");
                vm->pushNil();
                return 1;
            }

            Value objValue = vm->makeNativeClassInstance(false);
            NativeClassInstance *instance = objValue.asNativeClassInstance();
            instance->klass = objClass;
            instance->userData = (void *)manual;

            vm->push(objValue);
        }
        catch (const std::exception &e)
        {
            Error("createPlane exception: %s", e.what());
            vm->pushNil();
        }

        return 1;
    }

    int scene_createCapsule(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 5)
        {
            Error("createCapsule: requires name, radius, height, segments, rings, material [group]");
            return 0;
        }

        try
        {
            Ogre::SceneManager *sm = static_cast<Ogre::SceneManager *>(data);
            const char *name = args[0].asStringChars();
            float radius = (float)args[1].asNumber();
            float height = (float)args[2].asNumber();
            int segments = (int)args[3].asNumber();
            int rings = (int)args[4].asNumber();
            const char *material = argCount >= 6 ? args[5].asStringChars() : "BaseWhiteNoLighting";
            const char *group = nullptr;
            if (argCount >= 7)
                group = args[6].asStringChars();

            Ogre::ManualObject *manual = sm->createManualObject(Ogre::String(name) + "_manual");
            if (!manual)
            {
                Error("createCapsule: failed to create manual object '%s'", name);
                vm->pushNil();
                return 1;
            }

            ProceduralMesh::createCapsule(manual, material, radius, height, segments, rings, group);

            NativeClassDef *objClass = nullptr;
            if (!vm->tryGetNativeClassDef("ManualObject", &objClass))
            {
                Error("ManualObject class not found in VM");
                vm->pushNil();
                return 1;
            }

            Value objValue = vm->makeNativeClassInstance(false);
            NativeClassInstance *instance = objValue.asNativeClassInstance();
            instance->klass = objClass;
            instance->userData = (void *)manual;

            vm->push(objValue);
        }
        catch (const std::exception &e)
        {
            Error("createCapsule exception: %s", e.what());
            vm->pushNil();
        }

        return 1;
    }

    // createParticleSystem(scene, "name", "templateName") - função global
    int createParticleSystem(Interpreter *vm, void *data, int argCount, Value *args)
    {

        Ogre::SceneManager *scene = static_cast<Ogre::SceneManager *>(data);

        if (!scene)
        {
            Error("createParticleSystem: invalid scene");
            return 0;
        }
        if (argCount < 2)
        {
            Error("createParticleSystem: requires name and templateName");
            return 0;
        }

        if (!args[0].isString() || !args[1].isString())
        {
            Error("createParticleSystem: name and templateName must be strings");
            return 0;
        }

        const char *name = args[0].asStringChars();
        const char *templateName = args[1].asStringChars();

        Ogre::ParticleSystem *particleSystem = scene->createParticleSystem(name, templateName);

        if (!particleSystem)
        {
            Error("createParticleSystem: failed to create particle system '%s' with template '%s'", name, templateName);
            return 0;
        }

        // Get the ParticleSystem NativeClassDef
        NativeClassDef *particleClass = nullptr;
        if (!vm->tryGetNativeClassDef("ParticleSystem", &particleClass))
        {
            Error("ParticleSystem class not found in VM");
            return 0;
        }

        // Create NativeClassInstance
        Value particleValue = vm->makeNativeClassInstance(false);
        NativeClassInstance *instance = particleValue.asNativeClassInstance();
        instance->klass = particleClass;
        instance->userData = (void *)particleSystem;

        vm->push(particleValue);
        return 1;
    }

    // createRibbonTrail(scene, "name") - função global
    int createRibbonTrail(Interpreter *vm, void *data, int argCount, Value *args)
    {

        Ogre::SceneManager *scene = static_cast<Ogre::SceneManager *>(data);

        if (!scene)
        {
            Error("createRibbonTrail: invalid scene");
            return 0;
        }
        if (argCount < 1)
        {
            Error("createRibbonTrail: requires name");
            return 0;
        }

        if (!args[0].isString())
        {
            Error("createRibbonTrail: name must be a string");
            return 0;
        }

        const char *name = args[0].asStringChars();

        Ogre::RibbonTrail *ribbonTrail = scene->createRibbonTrail(name);

        if (!ribbonTrail)
        {
            Error("createRibbonTrail: failed to create ribbon trail '%s'", name);
            return 0;
        }

        // Get the RibbonTrail NativeClassDef
        NativeClassDef *ribbonClass = nullptr;
        if (!vm->tryGetNativeClassDef("RibbonTrail", &ribbonClass))
        {
            Error("RibbonTrail class not found in VM");
            return 0;
        }

        // Create NativeClassInstance
        Value ribbonValue = vm->makeNativeClassInstance(false);
        NativeClassInstance *instance = ribbonValue.asNativeClassInstance();
        instance->klass = ribbonClass;
        instance->userData = (void *)ribbonTrail;

        vm->push(ribbonValue);
        return 1;
    }

    // createBillboardSet(scene, name, poolSize) - global function
    int createBillboardSet(Interpreter *vm, void *data, int argCount, Value *args)
    {

        Ogre::SceneManager *scene = static_cast<Ogre::SceneManager *>(data);

        if (!scene)
        {
            Error("createBillboardSet: invalid scene");
            vm->pushNil();
            return 1;
        }
        if (argCount < 2)
        {
            Error("createBillboardSet: requires name and poolSize");
            vm->pushNil();
            return 1;
        }

        if (!args[0].isString() || !args[1].isNumber())
        {
            Error("createBillboardSet: name must be a string and poolSize must be a number");
            vm->pushNil();
            return 1;
        }

        const char *name = args[0].asStringChars();
        unsigned int poolSize = (unsigned int)args[1].asNumber();
        try
        {
            Ogre::BillboardSet *billboardSet = scene->createBillboardSet(name, poolSize);

            if (!billboardSet)
            {
                Error("createBillboardSet: failed to create billboard set '%s'", name);
                vm->pushNil();
                return 1;
            }

            // Get the BillboardSet NativeClassDef
            NativeClassDef *bbSetClass = nullptr;
            if (!vm->tryGetNativeClassDef("BillboardSet", &bbSetClass))
            {
                Error("BillboardSet class not found in VM");
                vm->pushNil();
                return 1;
            }

            Value bbSetValue = vm->makeNativeClassInstance(false);
            NativeClassInstance *instance = bbSetValue.asNativeClassInstance();
            instance->klass = bbSetClass;
            instance->userData = (void *)billboardSet;

            vm->push(bbSetValue);

            return 1;
        }
        catch (Ogre::Exception &e)
        {
            Error("createBillboardSet failed: %s", e.what());
            vm->pushNil();
            return 1;
        }
    }

    void registerAll(Interpreter &vm)
    {
        NativeClassDef *sc = vm.registerNativeClass(
            "Scene",
            scene_ctor,
            scene_dtor,
            0,
            false);

        vm.addNativeMethod(sc, "getRoot", scene_getRoot);
        vm.addNativeMethod(sc, "createEntity", scene_createEntity);
        vm.addNativeMethod(sc, "createLight", scene_createLight);
        vm.addNativeMethod(sc, "createCamera", scene_createCamera);

        vm.addNativeMethod(sc, "createCube", scene_createCube);
        vm.addNativeMethod(sc, "createSphere", scene_createSphere);
        vm.addNativeMethod(sc, "createCylinder", scene_createCylinder);
        vm.addNativeMethod(sc, "createCone", scene_createCone);
        vm.addNativeMethod(sc, "createTorus", scene_createTorus);
        vm.addNativeMethod(sc, "createQuad", scene_createQuad);
        vm.addNativeMethod(sc, "createPlane", scene_createPlane);
        vm.addNativeMethod(sc, "createCapsule", scene_createCapsule);
        vm.addNativeMethod(sc, "createRibbonTrail", createRibbonTrail);
        vm.addNativeMethod(sc, "createParticleSystem", createParticleSystem);
        vm.addNativeMethod(sc, "createBillboardSet", createBillboardSet);

        vm.addNativeMethod(sc, "setAmbientLight", scene_setAmbientLight);
        vm.addNativeMethod(sc, "setShadowTechnique", scene_setShadowTechnique);
        vm.addNativeMethod(sc, "setShadowTextureSize", scene_setShadowTextureSize);
        vm.addNativeMethod(sc, "setShadowTextureCount", scene_setShadowTextureCount);

        vm.addNativeMethod(sc, "setShadowFarDistance", scene_setShadowFarDistance);
        vm.addNativeMethod(sc, "setShadowDirLightTextureOffset", scene_setShadowDirLightTextureOffset);
        vm.addNativeMethod(sc, "setShadowTextureSelfShadow", scene_setShadowTextureSelfShadow);
        vm.addNativeMethod(sc, "setShadowCasterRenderBackFaces", scene_setShadowCasterRenderBackFaces);
        vm.addNativeMethod(sc, "setShadowColour", scene_setShadowColour);
        vm.addNativeMethod(sc, "setShadowTexturePixelFormat", scene_setShadowTexturePixelFormat);
        vm.addNativeMethod(sc, "setShadowTextureCountPerLightType", scene_setShadowTextureCountPerLightType);

        vm.addNativeMethod(sc, "setSkyBox", scene_setSkyBox);
        vm.addNativeMethod(sc, "setSkyDome", scene_setSkyDome);
        vm.addNativeMethod(sc, "setSkyPlane", scene_setSkyPlane);
        vm.addNativeMethod(sc, "setSkyPlaneCustom", scene_setSkyPlaneCustom);

        vm.addNativeMethod(sc, "setFog", scene_setFog);
        vm.addNativeMethod(sc, "disableFog", scene_disableFog);

        vm.registerNative("CreateScene", native_create_scene, 0);

        //  Info("Scene bindings registered");
    }

} // namespace OgreSceneBindings
