// ============================================
// FUNÇÕES DE SHADOW MAPS PARA ADICIONAR EM scene.cpp
// ============================================
// Adicionar após scene_setShadowTextureCount (linha ~255)

// setShadowFarDistance(distance)
int scene_setShadowFarDistance(Interpreter *vm, void *data, int argCount, Value *args)
{
    if (argCount < 1) return 0;
    Ogre::SceneManager *sm = static_cast<Ogre::SceneManager *>(data);
    float distance = (float)args[0].asNumber();
    sm->setShadowFarDistance(distance);
    return 0;
}

// setShadowDirLightTextureOffset(offset)
int scene_setShadowDirLightTextureOffset(Interpreter *vm, void *data, int argCount, Value *args)
{
    if (argCount < 1) return 0;
    Ogre::SceneManager *sm = static_cast<Ogre::SceneManager *>(data);
    float offset = (float)args[0].asNumber();
    sm->setShadowDirLightTextureOffset(offset);
    return 0;
}

// setShadowTextureSelfShadow(enabled)
int scene_setShadowTextureSelfShadow(Interpreter *vm, void *data, int argCount, Value *args)
{
    if (argCount < 1) return 0;
    Ogre::SceneManager *sm = static_cast<Ogre::SceneManager *>(data);
    bool enabled = args[0].asBool();
    sm->setShadowTextureSelfShadow(enabled);
    return 0;
}

// setShadowCasterRenderBackFaces(enabled)
int scene_setShadowCasterRenderBackFaces(Interpreter *vm, void *data, int argCount, Value *args)
{
    if (argCount < 1) return 0;
    Ogre::SceneManager *sm = static_cast<Ogre::SceneManager *>(data);
    bool enabled = args[0].asBool();
    sm->setShadowCasterRenderBackFaces(enabled);
    return 0;
}

// setShadowColour(r, g, b)
int scene_setShadowColour(Interpreter *vm, void *data, int argCount, Value *args)
{
    if (argCount < 3) return 0;
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
    if (argCount < 1) return 0;
    Ogre::SceneManager *sm = static_cast<Ogre::SceneManager *>(data);

    int format = (int)args[0].asNumber();
    Ogre::PixelFormat pf = Ogre::PF_X8R8G8B8;

    switch(format)
    {
        case 1: pf = Ogre::PF_FLOAT16_R; break;
        case 2: pf = Ogre::PF_FLOAT32_R; break;
        default: pf = Ogre::PF_X8R8G8B8; break;
    }

    sm->setShadowTexturePixelFormat(pf);
    return 0;
}

// setShadowTextureCountPerLightType(lightType, count)
// lightType: 0=point, 1=directional, 2=spotlight
int scene_setShadowTextureCountPerLightType(Interpreter *vm, void *data, int argCount, Value *args)
{
    if (argCount < 2) return 0;
    Ogre::SceneManager *sm = static_cast<Ogre::SceneManager *>(data);

    int lightType = (int)args[0].asNumber();
    size_t count = (size_t)args[1].asNumber();

    Ogre::Light::LightTypes lt = Ogre::Light::LT_POINT;
    switch(lightType)
    {
        case 1: lt = Ogre::Light::LT_DIRECTIONAL; break;
        case 2: lt = Ogre::Light::LT_SPOTLIGHT; break;
        default: lt = Ogre::Light::LT_POINT; break;
    }

    sm->setShadowTextureCountPerLightType(lt, count);
    return 0;
}

// ============================================
// REGISTROS PARA ADICIONAR EM registerAll()
// ============================================
// Adicionar na função registerAll() após vm.addNativeMethod(sc, "setShadowTechnique", scene_setShadowTechnique);

vm.addNativeMethod(sc, "setupShadowDefaults", scene_setupShadowDefaults);
vm.addNativeMethod(sc, "setShadowTextureSettings", scene_setShadowTextureSettings);
vm.addNativeMethod(sc, "setShadowTextureCount", scene_setShadowTextureCount);
vm.addNativeMethod(sc, "setShadowFarDistance", scene_setShadowFarDistance);
vm.addNativeMethod(sc, "setShadowDirLightTextureOffset", scene_setShadowDirLightTextureOffset);
vm.addNativeMethod(sc, "setShadowTextureSelfShadow", scene_setShadowTextureSelfShadow);
vm.addNativeMethod(sc, "setShadowCasterRenderBackFaces", scene_setShadowCasterRenderBackFaces);
vm.addNativeMethod(sc, "setShadowColour", scene_setShadowColour);
vm.addNativeMethod(sc, "setShadowTexturePixelFormat", scene_setShadowTexturePixelFormat);
vm.addNativeMethod(sc, "setShadowTextureCountPerLightType", scene_setShadowTextureCountPerLightType);
