#include "camera_controller.hpp"
#include <OgreQuaternion.h>
#include <OgreMath.h>

namespace CameraControllers
{
    // ============== BASE CAMERA CONTROLLER ==============

    CameraController::CameraController(Ogre::SceneNode *cameraNode)
        : mCameraNode(cameraNode), mMoveSpeed(100.0f), mMouseSensitivity(0.1f), mEnabled(true)
    {
    }

    // ============== FPS CONTROLLER ==============

    FPSController::FPSController(Ogre::SceneNode *cameraNode)
        : CameraController(cameraNode), mPitch(0.0f), mYaw(0.0f), mMaxPitch(89.0f), mMinPitch(-89.0f), mMoveForward(false), mMoveBackward(false), mMoveLeft(false), mMoveRight(false), mMoveUp(false), mMoveDown(false)
    {
    }

    void FPSController::update(float deltaTime, int mouseDeltaX, int mouseDeltaY)
    {
        if (!mEnabled || !mCameraNode)
            return;

        // ========== MOVEMENT ==========
        Ogre::Vector3 movement = Ogre::Vector3::ZERO;

        if (mMoveForward)
            movement.z -= 1.0f;
        if (mMoveBackward)
            movement.z += 1.0f;
        if (mMoveLeft)
            movement.x -= 1.0f;
        if (mMoveRight)
            movement.x += 1.0f;
        if (mMoveUp)
            movement.y += 1.0f;
        if (mMoveDown)
            movement.y -= 1.0f;

        // Apply speed and deltaTime
        if (movement != Ogre::Vector3::ZERO)
        {
            movement.normalise();
            movement *= mMoveSpeed * deltaTime;
            mCameraNode->translate(movement, Ogre::Node::TS_LOCAL);
        }

        // ========== ROTATION ==========
        if (mouseDeltaX != 0 || mouseDeltaY != 0)
        {
            mYaw -= mouseDeltaX * mMouseSensitivity;
            mPitch += mouseDeltaY * mMouseSensitivity;

            // Clamp pitch
            if (mPitch > mMaxPitch)
                mPitch = mMaxPitch;
            if (mPitch < mMinPitch)
                mPitch = mMinPitch;

            // Apply rotation
            mCameraNode->resetOrientation();
            mCameraNode->yaw(Ogre::Degree(-mYaw));
            mCameraNode->pitch(Ogre::Degree(mPitch));
        }
    }

    void FPSController::setPitchLimits(float min, float max)
    {
        mMinPitch = min;
        mMaxPitch = max;
    }

    // ============== ORBIT CONTROLLER ==============

    OrbitController::OrbitController(Ogre::SceneNode *cameraNode, const Ogre::Vector3 &target)
        : CameraController(cameraNode), mTarget(target), mDistance(100.0f), mMinDistance(5.0f), mMaxDistance(500.0f), mPitch(0.0f), mYaw(0.0f), mMaxPitch(89.0f), mMinPitch(-89.0f), mZoomSpeed(10.0f)
    {
        if (mCameraNode)
        {
            Ogre::Vector3 pos = mCameraNode->getPosition();
            mDistance = (pos - mTarget).length();
        }
    }

    void OrbitController::update(float deltaTime, int mouseDeltaX, int mouseDeltaY)
    {
        if (!mEnabled || !mCameraNode)
            return;

        // ========== ROTATION ==========
        if (mouseDeltaX != 0 || mouseDeltaY != 0)
        {
            mYaw -= mouseDeltaX * mMouseSensitivity;
            mPitch += mouseDeltaY * mMouseSensitivity;

            // Clamp pitch
            if (mPitch > mMaxPitch)
                mPitch = mMaxPitch;
            if (mPitch < mMinPitch)
                mPitch = mMinPitch;

            updateCameraPosition();
        }
    }

    void OrbitController::setTarget(const Ogre::Vector3 &target)
    {
        mTarget = target;
        updateCameraPosition();
    }

    void OrbitController::setDistance(float distance)
    {
        mDistance = distance;
        if (mDistance < mMinDistance)
            mDistance = mMinDistance;
        if (mDistance > mMaxDistance)
            mDistance = mMaxDistance;
        updateCameraPosition();
    }

    void OrbitController::setDistanceLimits(float min, float max)
    {
        mMinDistance = min;
        mMaxDistance = max;
    }

    void OrbitController::zoom(float delta)
    {
        mDistance -= delta * mZoomSpeed;
        if (mDistance < mMinDistance)
            mDistance = mMinDistance;
        if (mDistance > mMaxDistance)
            mDistance = mMaxDistance;
        updateCameraPosition();
    }

    void OrbitController::setPitchLimits(float min, float max)
    {
        mMinPitch = min;
        mMaxPitch = max;
    }

    void OrbitController::updateCameraPosition()
    {
        if (!mCameraNode)
            return;

        // Calcular posição da câmera baseado em pitch, yaw e distance
        float yawRad = Ogre::Math::DegreesToRadians(mYaw);
        float pitchRad = Ogre::Math::DegreesToRadians(mPitch);

        // Posição relativa ao target
        float x = mDistance * Ogre::Math::Cos(pitchRad) * Ogre::Math::Sin(yawRad);
        float y = mDistance * Ogre::Math::Sin(pitchRad);
        float z = mDistance * Ogre::Math::Cos(pitchRad) * Ogre::Math::Cos(yawRad);

        Ogre::Vector3 position = mTarget + Ogre::Vector3(x, y, z);
        mCameraNode->setPosition(position);

        // Olhar para o target
        mCameraNode->setDirection(mTarget - position, Ogre::Node::TS_PARENT);
    }

    // ============== FREE CAMERA CONTROLLER ==============

    FreeCameraController::FreeCameraController(Ogre::SceneNode *cameraNode)
        : CameraController(cameraNode), mPitch(0.0f), mYaw(0.0f), mRoll(0.0f), mLockRoll(true), mMoveForward(false), mMoveBackward(false), mMoveLeft(false), mMoveRight(false), mMoveUp(false), mMoveDown(false)
    {
    }

    void FreeCameraController::update(float deltaTime, int mouseDeltaX, int mouseDeltaY)
    {
        if (!mEnabled || !mCameraNode)
            return;

        // ========== MOVEMENT ==========
        Ogre::Vector3 movement = Ogre::Vector3::ZERO;

        if (mMoveForward)
            movement.z -= 1.0f;
        if (mMoveBackward)
            movement.z += 1.0f;
        if (mMoveLeft)
            movement.x -= 1.0f;
        if (mMoveRight)
            movement.x += 1.0f;
        if (mMoveUp)
            movement.y += 1.0f;
        if (mMoveDown)
            movement.y -= 1.0f;

        // Apply speed and deltaTime
        if (movement != Ogre::Vector3::ZERO)
        {
            movement.normalise();
            movement *= mMoveSpeed * deltaTime;
            mCameraNode->translate(movement, Ogre::Node::TS_LOCAL);
        }

        // ========== ROTATION ==========
        if (mouseDeltaX != 0 || mouseDeltaY != 0)
        {
            mYaw -= mouseDeltaX * mMouseSensitivity;
            mPitch += mouseDeltaY * mMouseSensitivity;

            // Apply rotation
            mCameraNode->resetOrientation();
            mCameraNode->yaw(Ogre::Degree(-mYaw));
            mCameraNode->pitch(Ogre::Degree(mPitch));

            // Apply roll if not locked
            if (!mLockRoll && mRoll != 0.0f)
            {
                mCameraNode->roll(Ogre::Degree(mRoll));
            }
        }
    }
 // ============== THIRD PERSON CONTROLLER  T) ==============

ThirdPersonController::ThirdPersonController(Ogre::SceneNode *cameraNode, Ogre::SceneNode *targetNode)
    : CameraController(cameraNode)
    , mTargetNode(targetNode)
    , mCurrentPosition(Ogre::Vector3::ZERO)
    , mDesiredPosition(Ogre::Vector3::ZERO)
    , mVelocity(Ogre::Vector3::ZERO)
    , mOrientationOffset(Ogre::Vector3::ZERO)
    , mOffset(0.0f, 1.5f, 0.0f),
    mSpringStiffness(50.0f),
    mSpringDamping(10.0f)
 
{
     
}
 

void ThirdPersonController::update(float deltaTime, int mouseDeltaX, int mouseDeltaY)
{
    if (!mEnabled || !mCameraNode || !mTargetNode) return;

    Ogre::Vector3 targetPos = mTargetNode->_getDerivedPosition() + (mTargetNode->_getDerivedOrientation() * mOffset);

  // 2. Cálculo da força da mola (Usando as variáveis da tua classe: mSpringStiffness e mSpringDamping)
    Ogre::Vector3 displacement = targetPos - mCameraNode->getPosition();
    Ogre::Vector3 springForce = displacement * mSpringStiffness;

    // 3. Aplicar amortecimento à velocidade (mVelocity da tua classe)
    Ogre::Vector3 dampingForce = mVelocity * mSpringDamping;
    Ogre::Vector3 acceleration = springForce - dampingForce;

    // 4. Integrar física básica
    mVelocity += acceleration *  deltaTime;
    mCameraNode->translate(mVelocity * deltaTime   , Ogre::Node::TS_WORLD);

    // 5. Sempre olhar para o player
    mCameraNode->lookAt(mTargetNode->_getDerivedPosition() +  mOrientationOffset,  Ogre::Node::TS_WORLD);
   
}

 

 

} // namespace CameraControllers