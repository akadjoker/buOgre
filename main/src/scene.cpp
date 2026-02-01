#include "bindings.hpp"

extern Ogre::Root *mRoot;

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
        return mRoot->createSceneManager();
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
            Info("Skybox enabled: %s (distance=%.1f)", materialName, distance);
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

    vm.addNativeMethod(sc, "setAmbientLight", scene_setAmbientLight);
    vm.addNativeMethod(sc, "setShadowTechnique", scene_setShadowTechnique);

    vm.addNativeMethod(sc, "setSkyBox", scene_setSkyBox);
    vm.addNativeMethod(sc, "setSkyDome", scene_setSkyDome);
    vm.addNativeMethod(sc, "setSkyPlane", scene_setSkyPlane);
    vm.addNativeMethod(sc, "setSkyPlaneCustom", scene_setSkyPlaneCustom);

    vm.addNativeMethod(sc, "setFog", scene_setFog);
    vm.addNativeMethod(sc, "disableFog", scene_disableFog);

    Info("Scene bindings registered");
}

} // namespace OgreSceneBindings
