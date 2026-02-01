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
        if (argCount < 3) return 0;
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        node->setPosition(
            (float)args[0].asNumber(),
            (float)args[1].asNumber(),
            (float)args[2].asNumber());
        return 0;
    }

    // translate(x, y, z)
    int node_translate(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3) return 0;
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        node->translate(
            (float)args[0].asNumber(),
            (float)args[1].asNumber(),
            (float)args[2].asNumber());
        return 0;
    }

    // moveRelative(x, y, z) - move in local space
    int node_moveRelative(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3) return 0;
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        node->translate(
            Ogre::Vector3((float)args[0].asNumber(), (float)args[1].asNumber(), (float)args[2].asNumber()),
            Ogre::Node::TS_LOCAL);
        return 0;
    }

    // setScale(x, y, z) - method version
    int node_method_setScale(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3) return 0;
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
        if (argCount < 4) 
        {
            Info("lookAt: Missing arguments (x, y, z) and space");
            return 0;
        }
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        node->lookAt(
            Ogre::Vector3(
                (float)args[0].asNumber(),
                (float)args[1].asNumber(),
                (float)args[2].asNumber()),
            IntToSpace((int)args[3].asNumber()));
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
        if (argCount < 4) return 0;
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        Ogre::Vector3 axis(
            (float)args[0].asNumber(),
            (float)args[1].asNumber(),
            (float)args[2].asNumber());
        float angleDegrees = (float)args[3].asNumber();
        
        node->rotate(axis, Ogre::Degree(angleDegrees));
        return 0;
    }

    // pitch(angleDegrees) - rotacao em torno do eixo X
    int node_pitch(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        float angleDegrees = (float)args[0].asNumber();
        node->pitch(Ogre::Degree(angleDegrees));
        return 0;
    }

    // yaw(angleDegrees) - rotacao em torno do eixo Y
    int node_yaw(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        float angleDegrees = (float)args[0].asNumber();
        node->yaw(Ogre::Degree(angleDegrees));
        return 0;
    }

    // roll(angleDegrees) - rotacao em torno do eixo Z
    int node_roll(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        float angleDegrees = (float)args[0].asNumber();
        node->roll(Ogre::Degree(angleDegrees));
        return 0;
    }

    // setVisible(visible) - mostra/esconde o nó e seus filhos
    int node_setVisible(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
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

    // moveForward(distance) - move para frente no sistema local
    int node_moveForward(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        float distance = (float)args[0].asNumber();
        node->translate(0, 0, -distance, Ogre::Node::TS_LOCAL);
        return 0;
    }

    // moveBackward(distance) - move para trás no sistema local
    int node_moveBackward(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        float distance = (float)args[0].asNumber();
        node->translate(0, 0, distance, Ogre::Node::TS_LOCAL);
        return 0;
    }

    // moveLeft(distance) - move para esquerda no sistema local
    int node_moveLeft(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        float distance = (float)args[0].asNumber();
        node->translate(-distance, 0, 0, Ogre::Node::TS_LOCAL);
        return 0;
    }

    // moveRight(distance) - move para direita no sistema local
    int node_moveRight(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        float distance = (float)args[0].asNumber();
        node->translate(distance, 0, 0, Ogre::Node::TS_LOCAL);
        return 0;
    }

    // moveUp(distance) - move para cima no sistema local
    int node_moveUp(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        float distance = (float)args[0].asNumber();
        node->translate(0, distance, 0, Ogre::Node::TS_LOCAL);
        return 0;
    }

    // moveDown(distance) - move para baixo no sistema local
    int node_moveDown(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);
        float distance = (float)args[0].asNumber();
        node->translate(0, -distance, 0, Ogre::Node::TS_LOCAL);
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
        if (argCount < 4) return 0;
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);

        Ogre::Quaternion quat(
            (float)args[0].asNumber(),  // w
            (float)args[1].asNumber(),  // x
            (float)args[2].asNumber(),  // y
            (float)args[3].asNumber()   // z
        );

        node->setOrientation(quat);
        return 0;
    }

    // ========== FUNÇÕES DE DIREÇÃO ==========

    // setDirection(x, y, z, [relativeTo], [localDirectionVector]) - define a direção do nó
    int node_setDirection(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3) return 0;
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);

        Ogre::Vector3 direction(
            (float)args[0].asNumber(),
            (float)args[1].asNumber(),
            (float)args[2].asNumber()
        );

        // Argumentos opcionais: relativeTo (default: TS_PARENT) e localDirectionVector (default: -Z)
        Ogre::Node::TransformSpace relativeTo = Ogre::Node::TS_PARENT;
        Ogre::Vector3 localDirection = Ogre::Vector3::NEGATIVE_UNIT_Z;

        if (argCount >= 4)
        {
            relativeTo = IntToSpace((int)args[3].asNumber());
        }

        if (argCount >= 7)
        {
            localDirection = Ogre::Vector3(
                (float)args[4].asNumber(),
                (float)args[5].asNumber(),
                (float)args[6].asNumber()
            );
        }

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

    // ========== ROTAÇÃO AVANÇADA ==========

    // rotateAround(pivotX, pivotY, pivotZ, axisX, axisY, axisZ, angleDegrees)
    int node_rotateAround(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 7) return 0;
        Ogre::SceneNode *node = static_cast<Ogre::SceneNode *>(data);

        Ogre::Vector3 pivot(
            (float)args[0].asNumber(),
            (float)args[1].asNumber(),
            (float)args[2].asNumber()
        );

        Ogre::Vector3 axis(
            (float)args[3].asNumber(),
            (float)args[4].asNumber(),
            (float)args[5].asNumber()
        );

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

    void registerAll(Interpreter &vm)
    {
        NativeClassDef *node = vm.registerNativeClass(
            "SceneNode",
            nullptr,  // No constructor - SceneNodes are created via scene.getRoot() or node.createChild()
            nullptr,  // No destructor - Ogre owns SceneNodes
            8,        // properties: posX, posY, posZ, scaleX, scaleY, scaleZ, position, scale
            false
        );

        // Individual position properties (X, Y, Z)
        vm.addNativeProperty(node, "posX", node_getPosX, node_setPosX);
        vm.addNativeProperty(node, "posY", node_getPosY, node_setPosY);
        vm.addNativeProperty(node, "posZ", node_getPosZ, node_setPosZ);

        // Individual scale properties (X, Y, Z)
        vm.addNativeProperty(node, "scaleX", node_getScaleX, node_setScaleX);
        vm.addNativeProperty(node, "scaleY", node_getScaleY, node_setScaleY);
        vm.addNativeProperty(node, "scaleZ", node_getScaleZ, node_setScaleZ);

        // Vector3 properties (full position and scale)
        vm.addNativeProperty(node, "position", node_getPosition, node_setPosition);
        vm.addNativeProperty(node, "scale", node_getScale, node_setScale);

        // Methods
        vm.addNativeMethod(node, "setPosition", node_method_setPosition);
        vm.addNativeMethod(node, "translate", node_translate);
        vm.addNativeMethod(node, "moveRelative", node_moveRelative);
        vm.addNativeMethod(node, "setScale", node_method_setScale);
        vm.addNativeMethod(node, "lookAt", node_lookAt);
        vm.addNativeMethod(node, "attachObject", node_attachObject);
        vm.addNativeMethod(node, "createChild", node_createChild);
        vm.addNativeMethod(node, "rotate", node_rotate);
        vm.addNativeMethod(node, "pitch", node_pitch);
        vm.addNativeMethod(node, "yaw", node_yaw);
        vm.addNativeMethod(node, "roll", node_roll);
        vm.addNativeMethod(node, "setVisible", node_setVisible);

        vm.addNativeMethod(node, "getWorldPosition", node_getWorldPosition);
        vm.addNativeMethod(node, "getWorldScale", node_getWorldScale);
        vm.addNativeMethod(node, "resetOrientation", node_resetOrientation);

        // Movimento direcional
        vm.addNativeMethod(node, "moveForward", node_moveForward);
        vm.addNativeMethod(node, "moveBackward", node_moveBackward);
        vm.addNativeMethod(node, "moveLeft", node_moveLeft);
        vm.addNativeMethod(node, "moveRight", node_moveRight);
        vm.addNativeMethod(node, "moveUp", node_moveUp);
        vm.addNativeMethod(node, "moveDown", node_moveDown);

        // Orientação com Quaternion
        vm.addNativeMethod(node, "getOrientation", node_getOrientation);
        vm.addNativeMethod(node, "setOrientation", node_setOrientation);

        // Direção
        vm.addNativeMethod(node, "setDirection", node_setDirection);
        vm.addNativeMethod(node, "getDirection", node_getDirection);

        // Rotação avançada
        vm.addNativeMethod(node, "rotateAround", node_rotateAround);

        Info("SceneNode bindings registered");
    }

} // namespace OgreSceneNodeBindings
