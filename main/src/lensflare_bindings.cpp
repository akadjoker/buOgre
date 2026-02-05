#include "bindings.hpp"
#include "lensflare.hpp"


using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// Constructor
/// @param LightPosition The 3D position of the Light, relative to the camera.
/// @param camera        The camera on which the lensflare effect will appear.
/// @param SceneMgr      Pointer on the SceneManager.
/* ------------------------------------------------------------------------- */
LensFlare::LensFlare(Vector3 LightPosition, Camera *camera, SceneManager *SceneMgr )
{
    mSceneMgr = SceneMgr;
    mCamera = camera;
    mHidden = true;
 
    this->createLensFlare();
    this->setLightPosition(LightPosition);
}

/* ------------------------------------------------------------------------- */
/// Destructor
/* ------------------------------------------------------------------------- */
LensFlare::~LensFlare()
{
    // mNode->detachObject(mHaloSet);
    // mNode->detachObject(mBurstSet);
    // mSceneMgr->destroyBillboardSet(mHaloSet);
    // mSceneMgr->destroyBillboardSet(mBurstSet);
    /// TODO destroy mNode
}

/* ------------------------------------------------------------------------- */
/// this function creates and shows all the LensFlare graphical elements.
/* ------------------------------------------------------------------------- */
void LensFlare::createLensFlare()
{
    Real LF_scale = 10.0f;

    // -----------------------------------------------------
    // We create 2 sets of billboards for the lensflare
    // -----------------------------------------------------
    mHaloSet = mSceneMgr->createBillboardSet("halo");
    mHaloSet->setMaterialName("lensflare/halo");
    mHaloSet->setCullIndividually(true);
    mHaloSet->setQueryFlags(0); // They should not be detected by rays.

    mBurstSet = mSceneMgr->createBillboardSet("burst");
    mBurstSet->setMaterialName("lensflare/burst");
    mBurstSet->setCullIndividually(true);
    mBurstSet->setQueryFlags(0);

    // The node is located at the light source.
    mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

    mNode->attachObject(mBurstSet);
    mNode->attachObject(mHaloSet);

    // -------------------------------
    // Creation of the Halo billboards
    // -------------------------------
    Billboard *LF_Halo1 = mHaloSet->createBillboard(0, 0, 0);
    LF_Halo1->setDimensions(LF_scale * 0.5, LF_scale * 0.5);
    Billboard *LF_Halo2 = mHaloSet->createBillboard(0, 0, 0);
    LF_Halo2->setDimensions(LF_scale, LF_scale);
    Billboard *LF_Halo3 = mHaloSet->createBillboard(0, 0, 0);
    LF_Halo3->setDimensions(LF_scale * 0.25, LF_scale * 0.25);

    // -------------------------------
    // Creation of the "Burst" billboards
    // -------------------------------
    Billboard *LF_Burst1 = mBurstSet->createBillboard(0, 0, 0);
    LF_Burst1->setDimensions(LF_scale * 0.25, LF_scale * 0.25);
    Billboard *LF_Burst2 = mBurstSet->createBillboard(0, 0, 0);
    LF_Burst2->setDimensions(LF_scale * 0.5, LF_scale * 0.5);
    Billboard *LF_Burst3 = mBurstSet->createBillboard(0, 0, 0);
    LF_Burst3->setDimensions(LF_scale * 0.25, LF_scale * 0.25);
}

 
void LensFlare::update()
{
    // If the Light is out of the Camera field Of View, the lensflare is hidden.
    if (!mCamera->isVisible(mLightPosition))
    {
        mHaloSet->setVisible(false);
        mBurstSet->setVisible(false);
        return;
    }
    else
    {
        mHaloSet->setVisible(true);
        mBurstSet->setVisible(true);
    }

    Real LightDistance = mLightPosition.distance(mCamera->getDerivedPosition());
    Vector3 CameraVect =    mCamera->getDerivedDirection(); // normalized vector (length 1)

        //Ogre::Vector3 camPos = mCameraNode->_getDerivedPosition();
        //Ogre::Vector3 CameraVect = mCameraNode->_getDerivedOrientation() * Ogre::Vector3::NEGATIVE_UNIT_Z;
        //CameraVect.normalise();

    CameraVect = mCamera->getDerivedPosition() + (LightDistance * CameraVect);

    // The LensFlare effect takes place along this vector.
    Vector3 LFvect = (CameraVect - mLightPosition);
    // LFvect += Vector3(-64,-64,0);  // sprite dimension (to be adjusted, but not necessary)

    // The different sprites are placed along this line.
    mHaloSet->getBillboard(0)->setPosition(LFvect * 0.500);
    mHaloSet->getBillboard(1)->setPosition(LFvect * 0.125);
    mHaloSet->getBillboard(2)->setPosition(-LFvect * 0.250);

    mBurstSet->getBillboard(0)->setPosition(LFvect * 0.333);
    mBurstSet->getBillboard(1)->setPosition(-LFvect * 0.500);
    mBurstSet->getBillboard(2)->setPosition(-LFvect * 0.180);

    // We redraw the lensflare (in case it was previouly out of the camera field, and hidden)
    this->setVisible(true);
}

/* ------------------------------------------------------------------------- */
/// This function shows (or hide) the lensflare effect.
/* ------------------------------------------------------------------------- */
void LensFlare::setVisible(bool visible)
{
    mHaloSet->setVisible(visible);
    mBurstSet->setVisible(visible);
    mHidden = !visible;
}

/* ------------------------------------------------------------------------- */
/// This function updates the light source position.
/** This function can be used if the light source is moving.*/
/* ------------------------------------------------------------------------- */
void LensFlare::setLightPosition(Vector3 pos)
{
    mLightPosition = pos;
    mNode->setPosition(mLightPosition);
}

/* ------------------------------------------------------------------------- */
/// This function changes the colour of the burst.
/* ------------------------------------------------------------------------- */
void LensFlare::setBurstColour(ColourValue color)
{
    mBurstSet->getBillboard(0)->setColour(color);
    mBurstSet->getBillboard(1)->setColour(color * 0.8);
    mBurstSet->getBillboard(2)->setColour(color * 0.6);
}

/* ------------------------------------------------------------------------- */
/// This function changes the colour of the halos.
/* ------------------------------------------------------------------------- */
void LensFlare::setHaloColour(ColourValue color)
{
    mHaloSet->getBillboard(0)->setColour(color * 0.8);
    mHaloSet->getBillboard(1)->setColour(color * 0.6);
    mHaloSet->getBillboard(2)->setColour(color);
}

namespace OgreLensFlareBindings
{
    void lensflare_dtor(Interpreter *vm, void *data)
    {
        delete static_cast<LensFlare *>(data);
    }

    int lensflare_update(Interpreter *vm, void *data, int argCount, Value *args)
    {
         
        LensFlare *flare = static_cast<LensFlare *>(data);
        if (flare)
            flare->update( );
        return 0;
    }

    int lensflare_setLightPosition(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3)
            return 0;
        LensFlare *flare = static_cast<LensFlare *>(data);
        if (!flare)
            return 0;

        float x = (float)args[0].asNumber();
        float y = (float)args[1].asNumber();
        float z = (float)args[2].asNumber();
        flare->setLightPosition(Ogre::Vector3(x, y, z));
        return 0;
    }

    int lensflare_setIntensity(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
            return 0;
        LensFlare *flare = static_cast<LensFlare *>(data);
        // if (flare)
        //     flare->setIntensity((float)args[0].asNumber());
        return 0;
    }

    int lensflare_setVisible(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
            return 0;
        LensFlare *flare = static_cast<LensFlare *>(data);
        if (flare)
            flare->setVisible(args[0].asBool());
        return 0;
    }

    void registerAll(Interpreter &vm)
    {
        NativeClassDef *lensFlare = vm.registerNativeClass(
            "LensFlare",
            nullptr,
            lensflare_dtor,
            0,
            false);

        vm.addNativeMethod(lensFlare, "update", lensflare_update);
        vm.addNativeMethod(lensFlare, "setLightPosition", lensflare_setLightPosition);
        vm.addNativeMethod(lensFlare, "setIntensity", lensflare_setIntensity);
        vm.addNativeMethod(lensFlare, "setVisible", lensflare_setVisible);
    }

} // namespace OgreLensFlareBindings
