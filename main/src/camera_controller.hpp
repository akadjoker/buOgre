#pragma once

#include <OgreSceneNode.h>
#include <OgreCamera.h>
#include <OgreVector3.h>

namespace CameraControllers
{
    // ============== BASE CAMERA CONTROLLER ==============

    class CameraController
    {
    protected:
        Ogre::SceneNode *mCameraNode;
        float mMoveSpeed;
        float mMouseSensitivity;
        bool mEnabled;

    public:
        CameraController(Ogre::SceneNode *cameraNode);
        virtual ~CameraController() = default;

        // Update controller (called every frame)
        virtual void update(float deltaTime, int mouseDeltaX, int mouseDeltaY) = 0;

        // Setters
        void setMoveSpeed(float speed) { mMoveSpeed = speed; }
        void setMouseSensitivity(float sensitivity) { mMouseSensitivity = sensitivity; }
        void setEnabled(bool enabled) { mEnabled = enabled; }

        // Getters
        float getMoveSpeed() const { return mMoveSpeed; }
        float getMouseSensitivity() const { return mMouseSensitivity; }
        bool isEnabled() const { return mEnabled; }
        Ogre::SceneNode *getCameraNode() const { return mCameraNode; }
    };

    // ============== FPS CONTROLLER ==============

    class FPSController : public CameraController
    {
    private:
        float mPitch;
        float mYaw;
        float mMaxPitch;
        float mMinPitch;

        // Movement flags
        bool mMoveForward;
        bool mMoveBackward;
        bool mMoveLeft;
        bool mMoveRight;
        bool mMoveUp;
        bool mMoveDown;

    public:
        FPSController(Ogre::SceneNode *cameraNode);
        virtual ~FPSController() = default;

        void update(float deltaTime, int mouseDeltaX, int mouseDeltaY) override;

        // Movement input
        void setMoveForward(bool move) { mMoveForward = move; }
        void setMoveBackward(bool move) { mMoveBackward = move; }
        void setMoveLeft(bool move) { mMoveLeft = move; }
        void setMoveRight(bool move) { mMoveRight = move; }
        void setMoveUp(bool move) { mMoveUp = move; }
        void setMoveDown(bool move) { mMoveDown = move; }

        // Rotation
        void setPitch(float pitch) { mPitch = pitch; }
        void setYaw(float yaw) { mYaw = yaw; }
        float getPitch() const { return mPitch; }
        float getYaw() const { return mYaw; }

        // Pitch limits
        void setPitchLimits(float min, float max);
    };

    // ============== ORBIT CONTROLLER ==============

    class OrbitController : public CameraController
    {
    private:
        Ogre::Vector3 mTarget;
        float mDistance;
        float mMinDistance;
        float mMaxDistance;
        float mPitch;
        float mYaw;
        float mMaxPitch;
        float mMinPitch;
        float mZoomSpeed;

    public:
        OrbitController(Ogre::SceneNode *cameraNode, const Ogre::Vector3 &target = Ogre::Vector3::ZERO);
        virtual ~OrbitController() = default;

        void update(float deltaTime, int mouseDeltaX, int mouseDeltaY) override;

        // Target
        void setTarget(const Ogre::Vector3 &target);
        Ogre::Vector3 getTarget() const { return mTarget; }

        // Distance
        void setDistance(float distance);
        void setDistanceLimits(float min, float max);
        float getDistance() const { return mDistance; }

        // Zoom
        void zoom(float delta);
        void setZoomSpeed(float speed) { mZoomSpeed = speed; }

        // Rotation
        void setPitch(float pitch) { mPitch = pitch; }
        void setYaw(float yaw) { mYaw = yaw; }
        float getPitch() const { return mPitch; }
        float getYaw() const { return mYaw; }

        // Pitch limits
        void setPitchLimits(float min, float max);

    private:
        void updateCameraPosition();
    };

    // ============== FREE CAMERA CONTROLLER ==============

    class FreeCameraController : public CameraController
    {
    private:
        float mPitch;
        float mYaw;
        float mRoll;
        bool mLockRoll;

        // Movement flags
        bool mMoveForward;
        bool mMoveBackward;
        bool mMoveLeft;
        bool mMoveRight;
        bool mMoveUp;
        bool mMoveDown;

    public:
        FreeCameraController(Ogre::SceneNode *cameraNode);
        virtual ~FreeCameraController() = default;

        void update(float deltaTime, int mouseDeltaX, int mouseDeltaY) override;

        // Movement input
        void setMoveForward(bool move) { mMoveForward = move; }
        void setMoveBackward(bool move) { mMoveBackward = move; }
        void setMoveLeft(bool move) { mMoveLeft = move; }
        void setMoveRight(bool move) { mMoveRight = move; }
        void setMoveUp(bool move) { mMoveUp = move; }
        void setMoveDown(bool move) { mMoveDown = move; }

        // Rotation
        void setPitch(float pitch) { mPitch = pitch; }
        void setYaw(float yaw) { mYaw = yaw; }
        void setRoll(float roll) { mRoll = roll; }
        float getPitch() const { return mPitch; }
        float getYaw() const { return mYaw; }
        float getRoll() const { return mRoll; }

        // Roll lock (previne inclinação lateral)
        void setLockRoll(bool lock) { mLockRoll = lock; }
        bool isRollLocked() const { return mLockRoll; }
    };

    // ============== THIRD PERSON CONTROLLER (with Spring System) ==============
 
class ThirdPersonController : public CameraController
{
private:
    Ogre::SceneNode *mTargetNode;  // Node do player/alvo

    // Camera position
    Ogre::Vector3 mCurrentPosition;
    Ogre::Vector3 mDesiredPosition;
    Ogre::Vector3 mVelocity;  // Para spring damping
    Ogre::Vector3 mOffset;  // Offset do ombro (shoulder offset)
    Ogre::Vector3 mOrientationOffset;

    float mSpringDamping;
    float mSpringStiffness;

 

public:
    ThirdPersonController(Ogre::SceneNode *cameraNode, Ogre::SceneNode *targetNode);
    virtual ~ThirdPersonController() = default;

    void update(float deltaTime, int mouseDeltaX, int mouseDeltaY) override;

    // Target
    void setTarget(Ogre::SceneNode *target) { mTargetNode = target; }
    Ogre::SceneNode *getTarget() const { return mTargetNode; }

    // Orientation Offset
    void setOrientationOffset(const Ogre::Vector3 &offset) { mOrientationOffset = offset; }
    void setOrientationOffset(float x, float y, float z) { mOrientationOffset = Ogre::Vector3(x, y, z); }
    Ogre::Vector3 getOrientationOffset() const { return mOrientationOffset; }
 
    void setSpringDamping(float damping) { mSpringDamping = damping; }
    float getSpringDamping() const { return mSpringDamping; }

    void setSpringStiffness(float stiffness) { mSpringStiffness = stiffness; }
    float getSpringStiffness() const { return mSpringStiffness; }


    // Offset (shoulder position)
    void setOffset(const Ogre::Vector3 &offset) { mOffset = offset; }
    void setOffset(float x, float y, float z) { mOffset = Ogre::Vector3(x, y, z); }
    Ogre::Vector3 getOffset() const { return mOffset; }

 

private:
 
};
} // namespace CameraControllers
