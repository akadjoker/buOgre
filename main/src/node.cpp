#include "bindings.hpp"

// ============== OGRE SCENENODE BINDINGS ==============

namespace OgreSceneNodeBindings
{

    // Position getters
    Value node_getPosX(Interpreter *vm, void *data)
    {
        return vm->makeFloat(static_cast<Ogre::SceneNode *>(data)->getPosition().x);
    }

    Value node_getPosY(Interpreter *vm, void *data)
    {
        return vm->makeFloat(static_cast<Ogre::SceneNode *>(data)->getPosition().y);
    }

    Value node_getPosZ(Interpreter *vm, void *data)
    {
        return vm->makeFloat(static_cast<Ogre::SceneNode *>(data)->getPosition().z);
    }

    void node_setPosX(Interpreter *vm, void *data, Value value)
    {
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        Ogre::Vector3 pos = node->getPosition();
        pos.x = (float)value.asNumber();
        node->setPosition(pos);
    }

    void node_setPosY(Interpreter *vm, void *data, Value value)
    {
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        Ogre::Vector3 pos = node->getPosition();
        pos.y = (float)value.asNumber();
        node->setPosition(pos);
    }

    void node_setPosZ(Interpreter *vm, void *data, Value value)
    {
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        Ogre::Vector3 pos = node->getPosition();
        pos.z = (float)value.asNumber();
        node->setPosition(pos);
    }

    // Scale getters
    Value node_getScaleX(Interpreter *vm, void *data)
    {
        return vm->makeFloat(static_cast<Ogre::SceneNode *>(data)->getScale().x);
    }

    Value node_getScaleY(Interpreter *vm, void *data)
    {
        return vm->makeFloat(static_cast<Ogre::SceneNode *>(data)->getScale().y);
    }

    Value node_getScaleZ(Interpreter *vm, void *data)
    {
        return vm->makeFloat(static_cast<Ogre::SceneNode *>(data)->getScale().z);
    }

    void node_setScaleX(Interpreter *vm, void *data, Value value)
    {
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        Ogre::Vector3 s = node->getScale();
        s.x = (float)value.asNumber();
        node->setScale(s);
    }

    void node_setScaleY(Interpreter *vm, void *data, Value value)
    {
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        Ogre::Vector3 s = node->getScale();
        s.y = (float)value.asNumber();
        node->setScale(s);
    }

    void node_setScaleZ(Interpreter *vm, void *data, Value value)
    {
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        Ogre::Vector3 s = node->getScale();
        s.z = (float)value.asNumber();
        node->setScale(s);
    }

    // Position property getter - returns Vector3 (as NativeClass)
    Value node_getPosition(Interpreter *vm, void *data)
    {
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        Ogre::Vector3 pos = node->getPosition();

        // Get the Vector3 NativeClassDef
        NativeClassDef *vec3Class = nullptr;
        if (!vm->tryGetNativeClassDef("Vector3", &vec3Class))
        {
            Error("Vector3 class not found in VM");
            return vm->makeNil();
        }

        // Create Vector3 natively using VM (GC will manage it)
        Ogre::Vector3 *vec = new Ogre::Vector3(pos.x, pos.y, pos.z);

        Value vecValue = vm->makeNativeClassInstance(false);
        NativeClassInstance *instance = vecValue.asNativeClassInstance();
        instance->klass = vec3Class;
        instance->userData = (void *)vec;

        return vecValue;
    }

    // Position property setter - accepts Vector3
    void node_setPosition(Interpreter *vm, void *data, Value value)
    {
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        NativeClassInstance *vecInstance = value.asNativeClassInstance();
        Ogre::Vector3 *vec = static_cast<Ogre::Vector3 *>(vecInstance->userData);
        node->setPosition(*vec);
    }

    // Scale property getter - returns Vector3 (as NativeClass)
    Value node_getScale(Interpreter *vm, void *data)
    {
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        Ogre::Vector3 scale = node->getScale();

        // Get the Vector3 NativeClassDef
        NativeClassDef *vec3Class = nullptr;
        if (!vm->tryGetNativeClassDef("Vector3", &vec3Class))
        {
            Error("Vector3 class not found in VM");
            return vm->makeNil();
        }

        // Create Vector3 natively using VM (GC will manage it)
        Ogre::Vector3 *vec = new Ogre::Vector3(scale.x, scale.y, scale.z);

        Value vecValue = vm->makeNativeClassInstance(false);
        NativeClassInstance *instance = vecValue.asNativeClassInstance();
        instance->klass = vec3Class;
        instance->userData = (void *)vec;

        return vecValue;
    }

    // Scale property setter - accepts Vector3
    void node_setScale(Interpreter *vm, void *data, Value value)
    {
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);

        if (value.isPointer())
        {
            Ogre::Vector3 *vec = static_cast<Ogre::Vector3 *>(value.asPointer());
            if (vec)
            {
                node->setScale(*vec);
            }
        }
    }

    // Methods
    // setPosition(x, y, z) - method version
    int node_method_setPosition(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3)
        {
            Error("setPosition: Expected 3 arguments (x, y, z)");
            return 0;
        }
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        node->setPosition(
            (float)args[0].asNumber(),
            (float)args[1].asNumber(),
            (float)args[2].asNumber());
        return 0;
    }

    // translate(x, y, z)

    int node_moveRelative(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 4)
        {
            Error("node_translate: Expected 4 arguments (x, y, z), relative");
            return 0;
        }
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        node->translate(
            Ogre::Vector3((float)args[0].asNumber(), (float)args[1].asNumber(), (float)args[2].asNumber()),
            IntToSpace((int)args[3].asNumber()));
        return 0;
    }

    // setScale(x, y, z) - method version
    int node_method_setScale(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3)
        {
            Error("setScale: Expected 3 arguments (x, y, z)");
            return 0;
        }
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        node->setScale(
            (float)args[0].asNumber(),
            (float)args[1].asNumber(),
            (float)args[2].asNumber());
        return 0;
    }

    // lookAt(x, y, z)
    int node_lookAt(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3)
        {
            Error("lookAt: Missing arguments (x, y, z) [space]");
            return 0;
        }

        Ogre::Node::TransformSpace space = Ogre::Node::TS_LOCAL;
        if (argCount >= 4)
        {
            space = IntToSpace((int)args[3].asNumber());
        }

        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        node->lookAt(
            Ogre::Vector3(
                (float)args[0].asNumber(),
                (float)args[1].asNumber(),
                (float)args[2].asNumber()),
            space);
        return 0;
    }

    // attachObject(object) - attach an Entity/Light/Camera
    int node_attachObject(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            Error("attachObject: Missing argument");
            return 0;
        }
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);

        NativeClassInstance *sceneInstance = args[0].asNativeClassInstance();
        Ogre::MovableObject *obj = static_cast<Ogre::MovableObject *>(sceneInstance->userData);
        if (!obj)
        {
            Error("attachObject: Invalid MovableObject");
            return 0;
        }

        node->attachObject(obj);
        return 0;
    }

    // createChild() - returns a new child SceneNode
    int node_createChild(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        Ogre::SceneNode *child = node->createChildSceneNode();

        // Get the SceneNode NativeClassDef
        NativeClassDef *sceneNodeClass = nullptr;
        if (!vm->tryGetNativeClassDef("SceneNode", &sceneNodeClass))
        {
            Error("SceneNode class not found in VM");
            return 0;
        }

        // Create a NativeClassInstance of SceneNode
        // Use false so GC can manage the wrapper (Ogre owns the actual node)
        Value childValue = vm->makeNativeClassInstance(false);
        NativeClassInstance *instance = childValue.asNativeClassInstance();
        instance->klass = sceneNodeClass;
        instance->userData = (void *)child;

        vm->push(childValue);
        return 1;
    }

    // rotate(x, y, z, angleDegrees) - rotacao em torno de um eixo
    int node_rotate(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 5)
        {
            Error("rotate: Expected 5 arguments (x, y, z, angleDegrees, space)  ");
            return 0;
        }
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        Ogre::Vector3 axis(
            (float)args[0].asNumber(),
            (float)args[1].asNumber(),
            (float)args[2].asNumber());
        float angleDegrees = (float)args[3].asNumber();

        node->rotate(axis, Ogre::Degree(angleDegrees), IntToSpace((int)args[4].asNumber()));
        return 0;
    }

    // pitch(angleDegrees) - rotacao em torno do eixo X
    int node_pitch(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 2)
        {
            Error("pitch: Expected  argument (angleDegrees, space)");
            return 0;
        }
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        float angleDegrees = (float)args[0].asNumber();
        node->pitch(Ogre::Degree(angleDegrees), IntToSpace((int)args[1].asNumber()));
        return 0;
    }

    // yaw(angleDegrees) - rotacao em torno do eixo Y
    int node_yaw(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 2)
        {
            Error("yaw: Expected  argument (angleDegrees,space)");
            return 0;
        }
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        float angleDegrees = (float)args[0].asNumber();
        node->yaw(Ogre::Degree(angleDegrees), IntToSpace((int)args[1].asNumber()));
        return 0;
    }

    // roll(angleDegrees) - rotacao em torno do eixo Z
    int node_roll(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 2)
        {
            Error("roll: Expected  argument (angleDegrees,space)");
            return 0;
        }
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        float angleDegrees = (float)args[0].asNumber();
        node->roll(Ogre::Degree(angleDegrees), IntToSpace((int)args[1].asNumber()));
        return 0;
    }

    // setVisible(visible) - mostra/esconde o nó e seus filhos
    int node_setVisible(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            Error("setVisible: Expected 1 argument (visible)");
            return 0;
        }
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        node->setVisible(args[0].asBool());
        return 0;
    }

    // getWorldPosition() - retorna posição global
    int node_getWorldPosition(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        Ogre::Vector3 worldPos = node->_getDerivedPosition();

        // Get the Vector3 NativeClassDef
        NativeClassDef *vec3Class = nullptr;
        if (!vm->tryGetNativeClassDef("Vector3", &vec3Class))
        {
            Error("Vector3 class not found in VM");
            return 0;
        }

        Ogre::Vector3 *vec = new Ogre::Vector3(worldPos.x, worldPos.y, worldPos.z);

        Value vecValue = vm->makeNativeClassInstance(false);
        NativeClassInstance *instance = vecValue.asNativeClassInstance();
        instance->klass = vec3Class;
        instance->userData = (void *)vec;

        vm->push(vecValue);
        return 1;
    }

    // getWorldScale() - retorna scale global
    int node_getWorldScale(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        Ogre::Vector3 worldScale = node->_getDerivedScale();

        NativeClassDef *vec3Class = nullptr;
        if (!vm->tryGetNativeClassDef("Vector3", &vec3Class))
        {
            Error("Vector3 class not found in VM");
            return 0;
        }

        Ogre::Vector3 *vec = new Ogre::Vector3(worldScale.x, worldScale.y, worldScale.z);

        Value vecValue = vm->makeNativeClassInstance(false);
        NativeClassInstance *instance = vecValue.asNativeClassInstance();
        instance->klass = vec3Class;
        instance->userData = (void *)vec;

        vm->push(vecValue);
        return 1;
    }

    // resetOrientation() - volta à orientação padrão
    int node_resetOrientation(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        node->resetOrientation();
        return 0;
    }

    // ========== FUNÇÕES DE MOVIMENTO DIRECIONAL ==========

    // move(speed, deltaTime, [space]) - movimento para frente/trás ao longo do eixo Z local
    int node_Move(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 2)
        {
            Error("move: Expected 2-3 arguments (speed, deltaTime, [space])");
            return 0;
        }
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        float speed = (float)args[0].asNumber();
        float deltaTime = (float)args[1].asNumber();

        // Optional TransformSpace parameter (default: TS_LOCAL)
        Ogre::Node::TransformSpace space = Ogre::Node::TS_LOCAL;
        if (argCount >= 3)
        {
            space = IntToSpace((int)args[2].asNumber());
        }

        Ogre::Quaternion orient = node->_getDerivedOrientation();
        Ogre::Vector3 forward = orient * Ogre::Vector3::NEGATIVE_UNIT_Z;
        forward.normalise();
        node->translate(forward * speed * deltaTime, space);

        return 0;
    }

    // strafe(speed, deltaTime, [space]) - movimento lateral ao longo do eixo X local
    // Positivo = direita, Negativo = esquerda
    int node_Strafe(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 2)
        {
            Error("strafe: Expected 2-3 arguments (speed, deltaTime, [space])");
            return 0;
        }
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        float speed = (float)args[0].asNumber();
        float deltaTime = (float)args[1].asNumber();

        // Optional TransformSpace parameter (default: TS_LOCAL)
        Ogre::Node::TransformSpace space = Ogre::Node::TS_LOCAL;
        if (argCount >= 3)
        {
            space = IntToSpace((int)args[2].asNumber());
        }

        Ogre::Quaternion orient = node->_getDerivedOrientation();
        Ogre::Vector3 right = orient * Ogre::Vector3::UNIT_X;
        right.normalise();
        node->translate(right * speed * deltaTime, space);

        return 0;
    }

    // climb(speed, deltaTime, [space]) - movimento vertical ao longo do eixo Y local
    // Positivo = cima, Negativo = baixo
    int node_Climb(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 2)
        {
            Error("climb: Expected 2-3 arguments (speed, deltaTime, [space])");
            return 0;
        }
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        float speed = (float)args[0].asNumber();
        float deltaTime = (float)args[1].asNumber();

        // Optional TransformSpace parameter (default: TS_LOCAL)
        Ogre::Node::TransformSpace space = Ogre::Node::TS_LOCAL;
        if (argCount >= 3)
        {
            space = IntToSpace((int)args[2].asNumber());
        }

        Ogre::Quaternion orient = node->_getDerivedOrientation();
        Ogre::Vector3 up = orient * Ogre::Vector3::UNIT_Y;
        up.normalise();
        node->translate(up * speed * deltaTime, space);

        return 0;
    }

    // ========== FUNÇÕES DE ORIENTAÇÃO COM QUATERNION ==========

    // getOrientation() - retorna orientação como Quaternion
    int node_getOrientation(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        Ogre::Quaternion orientation = node->getOrientation();

        // Get the Quaternion NativeClassDef
        NativeClassDef *quatClass = nullptr;
        if (!vm->tryGetNativeClassDef("Quaternion", &quatClass))
        {
            Error("Quaternion class not found in VM");
            return 0;
        }

        // Create Quaternion natively using VM (GC will manage it)
        Ogre::Quaternion *quat = new Ogre::Quaternion(orientation.w, orientation.x, orientation.y, orientation.z);

        Value quatValue = vm->makeNativeClassInstance(false);
        NativeClassInstance *instance = quatValue.asNativeClassInstance();
        instance->klass = quatClass;
        instance->userData = (void *)quat;

        vm->push(quatValue);
        return 1;
    }

    // setOrientation(w, x, y, z) - define orientação com quaternion
    int node_setOrientation(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 4)
        {
            Error("setOrientation: Expected 4 arguments (w, x, y, z)");
            return 0;
        }
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);

        Ogre::Quaternion quat(
            (float)args[0].asNumber(), // w
            (float)args[1].asNumber(), // x
            (float)args[2].asNumber(), // y
            (float)args[3].asNumber()  // z
        );

        node->setOrientation(quat);
        return 0;
    }

    // ========== FUNÇÕES DE DIREÇÃO ==========

    // setDirection(x, y, z, [relativeTo], [localDirectionVector]) - define a direção do nó
    int node_setDirection(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 7)
        {
            Error("setDirection: Expected arguments (x, y, z) space, dx,d,y,dz");
            return 0;
        }
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);

        Ogre::Vector3 direction(
            (float)args[0].asNumber(),
            (float)args[1].asNumber(),
            (float)args[2].asNumber());

        // Argumentos opcionais: relativeTo (default: TS_PARENT) e localDirectionVector (default: -Z)
        Ogre::Node::TransformSpace relativeTo = Ogre::Node::TS_PARENT;
        Ogre::Vector3 localDirection = Ogre::Vector3::NEGATIVE_UNIT_Z;

        relativeTo = IntToSpace((int)args[3].asNumber());

        localDirection = Ogre::Vector3(
            (float)args[4].asNumber(),
            (float)args[5].asNumber(),
            (float)args[6].asNumber());

        node->setDirection(direction, relativeTo, localDirection);
        return 0;
    }

    // getDirection() - retorna a direção que o nó está apontando
    int node_getDirection(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);

        // Por padrão, a direção é -Z no sistema local
        Ogre::Vector3 direction = node->getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_Z;

        NativeClassDef *vec3Class = nullptr;
        if (!vm->tryGetNativeClassDef("Vector3", &vec3Class))
        {
            Error("Vector3 class not found in VM");
            return 0;
        }

        Ogre::Vector3 *vec = new Ogre::Vector3(direction.x, direction.y, direction.z);

        Value vecValue = vm->makeNativeClassInstance(false);
        NativeClassInstance *instance = vecValue.asNativeClassInstance();
        instance->klass = vec3Class;
        instance->userData = (void *)vec;

        vm->push(vecValue);
        return 1;
    }

    int node_getPosition(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        Ogre::Vector3 pos = node->getPosition();

        // Get the Vector3 NativeClassDef
        NativeClassDef *vec3Class = nullptr;
        if (!vm->tryGetNativeClassDef("Vector3", &vec3Class))
        {
            Error("Vector3 class not found in VM");
            return 0;
        }

        Ogre::Vector3 *vec = new Ogre::Vector3(pos.x, pos.y, pos.z);

        Value vecValue = vm->makeNativeClassInstance(false);
        NativeClassInstance *instance = vecValue.asNativeClassInstance();
        instance->klass = vec3Class;
        instance->userData = (void *)vec;

        vm->push(vecValue);
        return 1;
    }

    int node_getPositionV(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        Ogre::Vector3 pos = node->getPosition();

        vm->push(vm->makeFloat(pos.x));
        vm->push(vm->makeFloat(pos.y));
        vm->push(vm->makeFloat(pos.z));

        return 3;
    }

 

    // ========== ROTAÇÃO AVANÇADA ==========

    // rotateAround(pivotX, pivotY, pivotZ, axisX, axisY, axisZ, angleDegrees)
    int node_rotateAround(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 7)
        {
            Error("rotateAround: Expected 7 arguments (pivotX, pivotY, pivotZ, axisX, axisY, axisZ, angleDegrees)");
            return 0;
        }
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);

        Ogre::Vector3 pivot(
            (float)args[0].asNumber(),
            (float)args[1].asNumber(),
            (float)args[2].asNumber());

        Ogre::Vector3 axis(
            (float)args[3].asNumber(),
            (float)args[4].asNumber(),
            (float)args[5].asNumber());

        float angleDegrees = (float)args[6].asNumber();
        Ogre::Radian angleRad = Ogre::Degree(angleDegrees);

        // Algoritmo para rotacionar em torno de um ponto:
        // 1. Mover para a origem (relativo ao pivot)
        // 2. Rotacionar
        // 3. Mover de volta

        Ogre::Vector3 currentPos = node->getPosition();
        Ogre::Vector3 offset = currentPos - pivot;

        // Criar quaternion de rotação
        Ogre::Quaternion rotation(angleRad, axis);

        // Rotacionar o offset
        Ogre::Vector3 rotatedOffset = rotation * offset;

        // Nova posição
        node->setPosition(pivot + rotatedOffset);

        // Rotacionar também a orientação do nó
        node->rotate(axis, Ogre::Degree(angleDegrees));

        return 0;
    }

    int node_getYawRad(Interpreter *vm, void *data, int argCount, Value *args)
    {
        auto *node = static_cast<Ogre::SceneNode *>(data);
        float yaw = node->getOrientation().getYaw().valueRadians();
        vm->pushFloat(yaw);
        return 1;
    }
    int node_setYawRad(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            Error("setYaw: expected 1 arg (yaw)");
            return 0;
        }
        auto *node = static_cast<Ogre::SceneNode *>(data);

        // Define apenas o yaw (rotação no eixo Y), zerando pitch e roll
        // Isso é apropriado para personagens que andam no chão
        Ogre::Radian yaw((Ogre::Real)args[0].asNumber());
        Ogre::Quaternion orientation(yaw, Ogre::Vector3::UNIT_Y);

        node->setOrientation(orientation);
        return 0;
    }

    int node_getPitchRad(Interpreter *vm, void *data, int argCount, Value *args)
    {
        auto *node = static_cast<Ogre::SceneNode *>(data);
        float pitch = node->getOrientation().getPitch().valueRadians();
        vm->pushFloat(pitch);
        return 1;
    }

    int node_setPitchRad(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            Error("setPitch: expected 1 arg (pitch)");
            return 0;
        }
        auto *node = static_cast<Ogre::SceneNode *>(data);

        // Define apenas o pitch (rotação no eixo X), zerando yaw e roll
        Ogre::Radian pitch((Ogre::Real)args[0].asNumber());
        Ogre::Quaternion orientation(pitch, Ogre::Vector3::UNIT_X);

        node->setOrientation(orientation);
        return 0;
    }

    int node_getRollRad(Interpreter *vm, void *data, int argCount, Value *args)
    {
        auto *node = static_cast<Ogre::SceneNode *>(data);
        float roll = node->getOrientation().getRoll().valueRadians();
        vm->pushFloat(roll);
        return 1;
    }

    int node_setRollRad(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            Error("setRoll: expected 1 arg (roll)");
            return 0;
        }
        auto *node = static_cast<Ogre::SceneNode *>(data);

        // Define apenas o roll (rotação no eixo Z), zerando yaw e pitch
        Ogre::Radian roll((Ogre::Real)args[0].asNumber());
        Ogre::Quaternion orientation(roll, Ogre::Vector3::UNIT_Z);

        node->setOrientation(orientation);
        return 0;
    }

    void registerAll(Interpreter &vm)
    {
        NativeClassDef *node = vm.registerNativeClass(
            "SceneNode",
            nullptr, // No constructor - SceneNodes are created via scene.getRoot() or node.createChild()
            nullptr, // No destructor - Ogre owns SceneNodes
            8,       // properties: posX, posY, posZ, scaleX, scaleY, scaleZ, position, scale
            false);

        // Individual position properties (X, Y, Z)
        vm.addNativeProperty(node, "posX", node_getPosX, node_setPosX);
        vm.addNativeProperty(node, "posY", node_getPosY, node_setPosY);
        vm.addNativeProperty(node, "posZ", node_getPosZ, node_setPosZ);

        // Individual scale properties (X, Y, Z)
        vm.addNativeProperty(node, "scaleX", node_getScaleX, node_setScaleX);
        vm.addNativeProperty(node, "scaleY", node_getScaleY, node_setScaleY);
        vm.addNativeProperty(node, "scaleZ", node_getScaleZ, node_setScaleZ);

        // rotation properties could be added here similarly

        // Vector3 properties (full position and scale)
        vm.addNativeProperty(node, "position", node_getPosition, node_setPosition);
        vm.addNativeProperty(node, "scale", node_getScale, node_setScale);

        // Methods
        vm.addNativeMethod(node, "setPosition", node_method_setPosition);
        vm.addNativeMethod(node, "getPosition", node_getPosition);
        vm.addNativeMethod(node, "getPositionV", node_getPositionV);
 

        vm.addNativeMethod(node, "translate", node_moveRelative);
        vm.addNativeMethod(node, "setScale", node_method_setScale);
        vm.addNativeMethod(node, "lookAt", node_lookAt);
        vm.addNativeMethod(node, "attachObject", node_attachObject);
        vm.addNativeMethod(node, "createChild", node_createChild);
        vm.addNativeMethod(node, "rotate", node_rotate);
        vm.addNativeMethod(node, "setVisible", node_setVisible);

        vm.addNativeMethod(node, "getWorldPosition", node_getWorldPosition);
        vm.addNativeMethod(node, "getWorldScale", node_getWorldScale);
        vm.addNativeMethod(node, "resetOrientation", node_resetOrientation);

        // Movimento direcional
        vm.addNativeMethod(node, "move", node_Move);
        vm.addNativeMethod(node, "strafe", node_Strafe);
        vm.addNativeMethod(node, "climb", node_Climb);

        // Orientação com Quaternion
        vm.addNativeMethod(node, "getOrientation", node_getOrientation);
        vm.addNativeMethod(node, "setOrientation", node_setOrientation);

        // Direção
        vm.addNativeMethod(node, "setDirection", node_setDirection);
        vm.addNativeMethod(node, "getDirection", node_getDirection);

        // Rotação incremental (em graus, requer space como 2º argumento)
        vm.addNativeMethod(node, "pitch", node_pitch);
        vm.addNativeMethod(node, "yaw", node_yaw);
        vm.addNativeMethod(node, "roll", node_roll);

        // Rotação absoluta get/set (em radianos)
        vm.addNativeMethod(node, "getYaw", node_getYawRad);
        vm.addNativeMethod(node, "setYaw", node_setYawRad);
        vm.addNativeMethod(node, "getPitch", node_getPitchRad);
        vm.addNativeMethod(node, "setPitch", node_setPitchRad);
        vm.addNativeMethod(node, "getRoll", node_getRollRad);
        vm.addNativeMethod(node, "setRoll", node_setRollRad);

        // Rotação avançada
        vm.addNativeMethod(node, "rotateAround", node_rotateAround);

        // Info("SceneNode bindings registered");
    }

} // namespace OgreSceneNodeBindings
