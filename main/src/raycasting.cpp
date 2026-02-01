#include "bindings.hpp"
#include <OgreRay.h>


// ============== OGRE RAYCASTING BINDINGS ==============

namespace OgreRaycastBindings
{
    // screenToWorldRay(camera, screenX, screenY) -> returns ray origin and direction
    int screenToWorldRay(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 3)
        {
            Error("screenToWorldRay: requires camera, screenX, screenY");
            return 0;
        }

        NativeClassInstance *cameraInstance = args[0].asNativeClassInstance();
        Ogre::Camera *camera = static_cast<Ogre::Camera *>(cameraInstance->userData);

        if (!camera)
        {
            Error("screenToWorldRay: invalid camera");
            return 0;
        }

        float screenX = (float)args[1].asNumber();
        float screenY = (float)args[2].asNumber();

        // Convert screen coordinates to normalized device coordinates (-1 to 1)
        // Assuming screen coordinates are 0-1 range
        Ogre::Ray ray = camera->getCameraToViewportRay(screenX, screenY);

        // Return ray origin and direction
        // We'll return them as separate Vector3 values (or could create a Ray class)

        // For now, push origin and direction as 6 numbers
        vm->pushFloat(ray.getOrigin().x);
        vm->pushFloat(ray.getOrigin().y);
        vm->pushFloat(ray.getOrigin().z);
        vm->pushFloat(ray.getDirection().x);
        vm->pushFloat(ray.getDirection().y);
        vm->pushFloat(ray.getDirection().z);

        return 6;
    }

    // raycast(scene, originX, originY, originZ, dirX, dirY, dirZ) -> returns hit info
    int raycast(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 7)
        {
            Error("raycast: requires scene, origin(x,y,z), direction(x,y,z)");
            return 0;
        }

        NativeClassInstance *sceneInstance = args[0].asNativeClassInstance();
        Ogre::SceneManager *scene = static_cast<Ogre::SceneManager *>(sceneInstance->userData);

        if (!scene)
        {
            Error("raycast: invalid scene");
            return 0;
        }

        Ogre::Vector3 origin(
            (float)args[1].asNumber(),
            (float)args[2].asNumber(),
            (float)args[3].asNumber()
        );

        Ogre::Vector3 direction(
            (float)args[4].asNumber(),
            (float)args[5].asNumber(),
            (float)args[6].asNumber()
        );

        Ogre::Ray ray(origin, direction);

        // Create ray scene query
        Ogre::RaySceneQuery *rayQuery = scene->createRayQuery(ray);
        rayQuery->setSortByDistance(true);

        // Execute query
        Ogre::RaySceneQueryResult &result = rayQuery->execute();

        if (result.size() > 0)
        {
            // Get first hit
            Ogre::RaySceneQueryResultEntry &entry = result[0];

            // Push hit info: distance, movableObject name
            vm->pushFloat(entry.distance);

            if (entry.movable)
            {
                vm->pushString(entry.movable->getName().c_str());
            }
            else
            {
                vm->pushString("");
            }

            scene->destroyQuery(rayQuery);
            return 2;  // distance, name
        }

        scene->destroyQuery(rayQuery);

        // No hit
        vm->pushFloat(-1.0f);
        vm->pushString("");
        return 2;
    }

    // raycastFromMouse(scene, camera, mouseX, mouseY, viewportWidth, viewportHeight) -> hit info
    int raycastFromMouse(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 6)
        {
            Error("raycastFromMouse: requires scene, camera, mouseX, mouseY, viewportWidth, viewportHeight");
            return 0;
        }

        NativeClassInstance *sceneInstance = args[0].asNativeClassInstance();
        Ogre::SceneManager *scene = static_cast<Ogre::SceneManager *>(sceneInstance->userData);

        NativeClassInstance *cameraInstance = args[1].asNativeClassInstance();
        Ogre::Camera *camera = static_cast<Ogre::Camera *>(cameraInstance->userData);

        if (!scene || !camera)
        {
            vm->pushFloat(-1.0f);
            vm->pushString("");
            return 2;
        }

        float mouseX = (float)args[2].asNumber();
        float mouseY = (float)args[3].asNumber();
        float viewportWidth = (float)args[4].asNumber();
        float viewportHeight = (float)args[5].asNumber();

        // Convert to normalized coordinates (0-1)
        float screenX = mouseX / viewportWidth;
        float screenY = mouseY / viewportHeight;

        // Get ray from camera
        Ogre::Ray ray = camera->getCameraToViewportRay(screenX, screenY);

        // Create ray scene query
        Ogre::RaySceneQuery *rayQuery = scene->createRayQuery(ray);
        rayQuery->setSortByDistance(true);

        // Execute query
        Ogre::RaySceneQueryResult &result = rayQuery->execute();

        if (result.size() > 0)
        {
            // Get first hit
            Ogre::RaySceneQueryResultEntry &entry = result[0];

            // Push hit info
            vm->pushFloat(entry.distance);

            if (entry.movable)
            {
                vm->pushString(entry.movable->getName().c_str());

                // Also push hit position
                Ogre::Vector3 hitPoint = ray.getPoint(entry.distance);
                vm->pushFloat(hitPoint.x);
                vm->pushFloat(hitPoint.y);
                vm->pushFloat(hitPoint.z);

                scene->destroyQuery(rayQuery);
                return 5;  // distance, name, hitX, hitY, hitZ
            }
            else
            {
                vm->pushString("");
                vm->pushFloat(0.0f);
                vm->pushFloat(0.0f);
                vm->pushFloat(0.0f);

                scene->destroyQuery(rayQuery);
                return 5;
            }
        }

        scene->destroyQuery(rayQuery);

        // No hit
        vm->pushFloat(-1.0f);
        vm->pushString("");
        vm->pushFloat(0.0f);
        vm->pushFloat(0.0f);
        vm->pushFloat(0.0f);
        return 5;
    }

    // raycastAll(scene, originX, originY, originZ, dirX, dirY, dirZ, maxResults) -> returns count
    // Results are stored internally and can be queried
    int raycastAll(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 7)
        {
            Error("raycastAll: requires scene, origin(x,y,z), direction(x,y,z)");
            return 0;
        }

        NativeClassInstance *sceneInstance = args[0].asNativeClassInstance();
        Ogre::SceneManager *scene = static_cast<Ogre::SceneManager *>(sceneInstance->userData);

        if (!scene)
        {
            vm->pushInt(0);
            return 1;
        }

        Ogre::Vector3 origin(
            (float)args[1].asNumber(),
            (float)args[2].asNumber(),
            (float)args[3].asNumber()
        );

        Ogre::Vector3 direction(
            (float)args[4].asNumber(),
            (float)args[5].asNumber(),
            (float)args[6].asNumber()
        );

        int maxResults = 100;
        if (argCount >= 8) maxResults = (int)args[7].asNumber();

        Ogre::Ray ray(origin, direction);

        // Create ray scene query
        Ogre::RaySceneQuery *rayQuery = scene->createRayQuery(ray);
        rayQuery->setSortByDistance(true);

        // Execute query
        Ogre::RaySceneQueryResult &result = rayQuery->execute();

        int count = (int)result.size();
        if (count > maxResults) count = maxResults;

        // For now, just return count
        // In a real implementation, we'd store results for later retrieval
        vm->pushInt(count);

        scene->destroyQuery(rayQuery);
        return 1;
    }

    void registerAll(Interpreter &vm)
    {
        // Global functions for raycasting
        // vm.registerNative("screenToWorldRay", screenToWorldRay, 3);
        // vm.registerNative("raycast", raycast, 7);
        // vm.registerNative("raycastFromMouse", raycastFromMouse, 6);
        // vm.registerNative("raycastAll", raycastAll, 8);

        // Info("Raycasting bindings registered");
    }

} // namespace OgreRaycastBindings
