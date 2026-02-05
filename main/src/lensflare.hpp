#pragma once
#include <OgreBillboardSet.h>
#include <OgreBillboard.h>

/* ------------------------------------------------------------------------- */
/// A lens Flare effect.
/**
This class will create a lensflare effect, between The light position and the
camera position.
Some functions will allow to change the lensflare color effect, in case of coloured
light, for instance.
*/
/* ------------------------------------------------------------------------- */

using namespace Ogre;

class LensFlare
{
public:
    LensFlare(Vector3 LightPosition, Camera *camera, SceneManager *SceneMgr );
    virtual ~LensFlare();
    void createLensFlare();
    void update();
    void setVisible(bool visible);
    void setLightPosition(Vector3 pos);
    void setHaloColour(ColourValue color);
    void setBurstColour(ColourValue color);

protected:
    SceneManager *mSceneMgr;
    Camera *mCamera;
    ColourValue mColour;
    SceneNode *mNode;
   
    BillboardSet *mHaloSet;
    BillboardSet *mBurstSet;
    Vector3 mLightPosition;
    bool mHidden;
};

 

// class AutomaticLensFlare
// {
// private:
//     struct FlareElement
//     {
//         Ogre::Billboard *billboard;
//         float offset; // Posição ao longo da linha (0 = centro, 1 = sol)
//         float size;
//         Ogre::ColourValue color;
//     };

//     Ogre::SceneManager *mSceneMgr;
//     Ogre::Camera *mCamera;
//     Ogre::BillboardSet *mFlareSet;
//     Ogre::SceneNode *mFlareNode;
//     Ogre::Vector3 mLightPosition;

//     std::vector<FlareElement> mFlares;

//     bool mVisible;
//     float mIntensity;
//     float mFadeSpeed;

// public:
//     AutomaticLensFlare(Ogre::SceneManager *sceneMgr,
//                        Ogre::Camera *camera,
//                        const Ogre::Vector3 &lightPos)
//         : mSceneMgr(sceneMgr), mCamera(camera), mLightPosition(lightPos), mVisible(true), mIntensity(1.0f), mFadeSpeed(2.0f)
//     {
//         // Criar BillboardSet
//         mFlareSet = mSceneMgr->createBillboardSet("AutoLensFlare", 10);
//         mFlareSet->setMaterialName("Examples/Flare");
//         mFlareSet->setBillboardType(Ogre::BBT_POINT);
//         mFlareSet->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY - 1);

//         // Criar node
//         mFlareNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
//         mFlareNode->attachObject(mFlareSet);

//         // Criar elementos do flare
//         createFlareElements();
//     }

//     ~AutomaticLensFlare()
//     {
//         // if (mFlareSet)
//         // {
//         //     mSceneMgr->destroyBillboardSet(mFlareSet);
//         // }
//         // if (mFlareNode)
//         // {
//         //     mSceneMgr->destroySceneNode(mFlareNode);
//         // }
//     }

//     void createFlareElements()
//     {
//         // Flare central (sol)
//         addFlare(1.0f, 3.0f, Ogre::ColourValue(1.0f, 1.0f, 0.95f, 1.0f));

//         // Ghosts ao longo da linha centro-sol
//         addFlare(0.8f, 1.5f, Ogre::ColourValue(1.0f, 0.9f, 0.7f, 0.8f));
//         addFlare(0.6f, 1.2f, Ogre::ColourValue(0.9f, 0.8f, 1.0f, 0.6f));
//         addFlare(0.4f, 0.8f, Ogre::ColourValue(1.0f, 0.7f, 0.5f, 0.5f));
//         addFlare(0.2f, 0.6f, Ogre::ColourValue(0.7f, 0.9f, 1.0f, 0.4f));
//         addFlare(0.0f, 0.5f, Ogre::ColourValue(1.0f, 0.8f, 0.6f, 0.3f));
//         addFlare(-0.2f, 0.4f, Ogre::ColourValue(0.8f, 0.8f, 1.0f, 0.3f));
//         addFlare(-0.4f, 0.3f, Ogre::ColourValue(1.0f, 0.6f, 0.4f, 0.2f));
//         addFlare(-0.6f, 0.25f, Ogre::ColourValue(0.6f, 0.8f, 1.0f, 0.2f));
//     }

//     void addFlare(float offset, float size, const Ogre::ColourValue &color)
//     {
//         Ogre::Billboard *bb = mFlareSet->createBillboard(Ogre::Vector3::ZERO);
//         bb->setDimensions(size, size);
//         bb->setColour(color);

//         FlareElement element;
//         element.billboard = bb;
//         element.offset = offset;
//         element.size = size;
//         element.color = color;

//         mFlares.push_back(element);
//     }

//     void update(float deltaTime)
//     {
//         // 1. Calcular posição do sol em screen space
//         Ogre::Vector3 lightViewSpace = mCamera->getViewMatrix() * mLightPosition;

//         // Se luz está atrás da câmera, fade out
//         if (lightViewSpace.z > 0)
//         {
//             mIntensity = Ogre::Math::Clamp(mIntensity - mFadeSpeed * deltaTime, 0.0f, 1.0f);
//         }
//         else
//         {
//             mIntensity = Ogre::Math::Clamp(mIntensity + mFadeSpeed * deltaTime, 0.0f, 1.0f);
//         }

//         // Se invisível, esconder e retornar
//         if (mIntensity <= 0.0f)
//         {
//             mFlareSet->setVisible(false);
//             return;
//         }

//         mFlareSet->setVisible(true);

//         // 2. Projetar para clip space
//         Ogre::Vector4 lightClip = mCamera->getProjectionMatrix() * Ogre::Vector4(
//                                                                        lightViewSpace.x, lightViewSpace.y, lightViewSpace.z, 1.0f);

//         // Perspective divide para NDC
//         if (Ogre::Math::Abs(lightClip.w) < 0.0001f)
//         {
//             mFlareSet->setVisible(false);
//             return;
//         }

//         Ogre::Vector2 lightNDC(
//             lightClip.x / lightClip.w,
//             lightClip.y / lightClip.w);

//         // 3. Verificar se está dentro da tela
//         float distanceFromCenter = lightNDC.length();
//         float screenFade = 1.0f - Ogre::Math::Clamp(distanceFromCenter / 1.5f, 0.0f, 1.0f);
//         float finalIntensity = mIntensity * screenFade;

//         if (finalIntensity <= 0.01f)
//         {
//             mFlareSet->setVisible(false);
//             return;
//         }

//         // 4. Centro da tela em NDC
//         Ogre::Vector2 screenCenter(0.0f, 0.0f);
//         Ogre::Vector2 sunToCenterDir = screenCenter - lightNDC;

//         // 5. Posicionar cada flare ao longo da linha
//         for (size_t i = 0; i < mFlares.size(); i++)
//         {
//             FlareElement &flare = mFlares[i];

//             // Calcular posição do flare em NDC
//             Ogre::Vector2 flareNDC = lightNDC + sunToCenterDir * flare.offset;

//             // Converter NDC para world space (aproximação simplificada)
//             // Na prática, faremos unproject
//             Ogre::Vector3 flareWorld = unprojectNDC(flareNDC, mCamera);

//             // Posicionar billboard
//             flare.billboard->setPosition(flareWorld);

//             // Ajustar intensidade baseado na distância e fade geral
//             Ogre::ColourValue adjustedColor = flare.color;
//             adjustedColor.a *= finalIntensity;
//             flare.billboard->setColour(adjustedColor);

//             // Ajustar tamanho baseado na distância da câmera
//             float distance = (flareWorld - mCamera->getDerivedPosition()).length();
//             float scale = Ogre::Math::Clamp(distance / 100.0f, 0.5f, 2.0f);
//             flare.billboard->setDimensions(flare.size * scale, flare.size * scale);
//         }
//     }

//     Ogre::Vector3 unprojectNDC(const Ogre::Vector2 &ndc, Ogre::Camera *camera)
//     {
//         // Billboard fica sempre a 50 units à frente da câmera
//         float distance = 50.0f;

//         // Direção da câmera
//         Ogre::Vector3 camDir = camera->getDerivedOrientation() * Ogre::Vector3::NEGATIVE_UNIT_Z;
//         Ogre::Vector3 camRight = camera->getDerivedOrientation() * Ogre::Vector3::UNIT_X;
//         Ogre::Vector3 camUp = camera->getDerivedOrientation() * Ogre::Vector3::UNIT_Y;

//         // Posição base (à frente da câmera)
//         Ogre::Vector3 basePos = camera->getDerivedPosition() + camDir * distance;

//         // Offset horizontal e vertical
//         float scale = distance * 0.5f; // Ajustar
//         basePos += camRight * ndc.x * scale;
//         basePos += camUp * ndc.y * scale;

//         return basePos;
//     }

//     // Ogre::Vector3 unprojectNDC(const Ogre::Vector2& ndc, Ogre::Camera* camera)
//     // {
//     //     // Simplified unprojection - coloca os flares a uma distância fixa da câmera
//     //     float depth = 50.0f; // Distância dos flares

//     //     // Converter NDC para view space
//     //     Ogre::Vector3 viewPos(
//     //         ndc.x * depth * Ogre::Math::Tan(camera->getFOVy() * 0.5f) * camera->getAspectRatio(),
//     //         ndc.y * depth * Ogre::Math::Tan(camera->getFOVy() * 0.5f),
//     //         -depth
//     //     );

//     //     // Converter para world space
//     //     Ogre::Vector3 worldPos = camera->getDerivedOrientation() * viewPos +
//     //                              camera->getDerivedPosition();

//     //     return worldPos;
//     // }

//     void setLightPosition(const Ogre::Vector3 &pos)
//     {
//         mLightPosition = pos;
//     }

//     void setVisible(bool visible)
//     {
//         mVisible = visible;
//         if (!visible)
//         {
//             mFlareSet->setVisible(false);
//         }
//     }

//     void setIntensity(float intensity)
//     {
//         mIntensity = Ogre::Math::Clamp(intensity, 0.0f, 1.0f);
//     }
// };
