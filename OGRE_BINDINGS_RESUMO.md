# Resumo dos Bindings Ogre para buLanguage

## 📋 Índice
1. [Arquitetura Geral](#arquitetura-geral)
2. [Padrão de Implementação](#padrão-de-implementação)
3. [Classes/Namespaces Disponíveis](#classesnamespaces-disponíveis)
4. [Exemplos Práticos](#exemplos-práticos)
5. [Guia de Criação de Novos Bindings](#guia-de-criação-de-novos-bindings)

---

## 🏗️ Arquitetura Geral

Os bindings do Ogre fornecem uma interface entre o **buLanguage** (linguagem de script) e a **engine Ogre3D** (C++).

### Estrutura de Arquivos
```
main/src/
├── bindings.hpp           # Header com declarações de namespaces
├── bindings.cpp           # Implementação dos bindings base
├── scene.cpp              # SceneManager bindings
├── camera.cpp             # Camera bindings
├── light.cpp              # Light bindings
├── entity.cpp             # Entity bindings
├── node.cpp               # SceneNode bindings
├── material.cpp           # Material bindings
├── mesh.cpp               # Mesh bindings
├── animation.cpp          # AnimationState bindings
├── particles.cpp          # ParticleSystem bindings
├── billboards.cpp         # BillboardSet bindings
└── [outros].cpp           # Mais bindings especializados
```

### Fluxo de Comunicação
```
buScript (Script)
    ↓
Interpreter VM (C++)
    ↓
Native Bindings (C++)
    ↓
Ogre3D Engine (C++)
```

---

## 🔧 Padrão de Implementação

### 1️⃣ **Estrutura Básica de um Binding**

Cada namespace contém:

```cpp
namespace OgreXXXBindings
{
    // Constructor (cria o objeto)
    void *xxx_ctor(Interpreter *vm, int argCount, Value *args)
    {
        // Criar e retornar objeto Ogre
        return new OgreObject(...);
    }

    // Destructor (limpa o objeto)
    void xxx_dtor(Interpreter *vm, void *data)
    {
        // Limpar dados
    }

    // Property Getters (retornam valor)
    Value xxx_getProperty(Interpreter *vm, void *data)
    {
        OgreObject *obj = static_cast<OgreObject *>(data);
        return vm->makeFloat(obj->getValue()); // ou makeBool, makeString, etc.
    }

    // Property Setters (modificam valor)
    void xxx_setProperty(Interpreter *vm, void *data, Value value)
    {
        OgreObject *obj = static_cast<OgreObject *>(data);
        obj->setValue(value.asNumber()); // ou asBool, asStringChars, etc.
    }

    // Methods (funções/ações)
    int xxx_method(Interpreter *vm, void *data, int argCount, Value *args)
    {
        OgreObject *obj = static_cast<OgreObject *>(data);
        // Fazer algo
        vm->push(...); // Push resultado se necessário
        return 1; // Retornar número de valores no stack
    }

    // Registrar tudo no VM
    void registerAll(Interpreter &vm)
    {
        NativeClassDef *sc = vm.registerNativeClass(
            "ClassName",      // Nome da classe
            xxx_ctor,         // Constructor
            xxx_dtor,         // Destructor
            2,                // Número de properties (informação para o VM)
            false             // Se Ogre gerencia memória (false = Ogre gerencia)
        );

        // Adicionar properties
        vm.addNativeProperty(sc, "property", xxx_getProperty, xxx_setProperty);

        // Adicionar métodos
        vm.addNativeMethod(sc, "method", xxx_method);
    }
}
```

### 2️⃣ **Convertendo Tipos entre buScript e C++**

| buScript | C++ Getter | C++ Setter |
|----------|-----------|-----------|
| Number | `args[i].asNumber()` | `vm->makeFloat(value)` |
| Boolean | `args[i].asBool()` | `vm->makeBool(value)` |
| String | `args[i].asStringChars()` | `vm->makeString(value)` |
| Vector3 | `static_cast<Ogre::Vector3*>(args[i].asPointer())` | `vm->pushPointer(ptr)` |
| Objeto Nativo | `static_cast<Classe*>(args[i].asNativeClassInstance()->userData)` | `vm->makeNativeClassInstance(...)` |

### ⚠️ **O 4º argumento do `registerNativeClass`**

O **4º argumento** (`argCount`) é o **número de properties** que a classe terá. Exemplos reais:

```cpp
// Light tem 1 property (visible)
registerNativeClass("Light", light_ctor, light_dtor, 1, false);

// Entity tem 2 properties (castShadows, visible)
registerNativeClass("Entity", entity_ctor, entity_dtor, 2, false);

// Se tiver 3 properties
registerNativeClass("MyClass", ctor, dtor, 3, false);
```

Este número é informativo para o VM e ajuda na alocação eficiente de memória.

### 3️⃣ **Retornando Valores**

```cpp
// Para retornar um valor simples:
vm->push(vm->makeFloat(42.0f));
return 1;  // 1 valor no stack

vm->push(vm->makeBool(true));
return 1;

vm->push(vm->makeString("texto"));
return 1;

// Para retornar objeto Ogre (ex: Vector3):
Ogre::Vector3 *vec = new Ogre::Vector3(1, 2, 3);
vm->pushPointer((void *)vec);
return 1;

// Para retornar objeto NativeClass (ex: SceneNode):
NativeClassDef *nodeClass = nullptr;
vm->tryGetNativeClassDef("SceneNode", &nodeClass);
Value nodeValue = vm->makeNativeClassInstance(false);
NativeClassInstance *instance = nodeValue.asNativeClassInstance();
instance->klass = nodeClass;
instance->userData = (void *)node;
vm->push(nodeValue);
return 1;

// Para não retornar nada:
return 0;
```

---

## 📚 Classes/Namespaces Disponíveis

### **Vector3**
```cpp
namespace OgreVector3Bindings
// Construtores:
// Vector3()           - (0,0,0)
// Vector3(x, y, z)   - com valores
// Vector3(val)       - (val, val, val)

// Properties:
x, y, z

// Methods:
length()              → float
normalise()           → void
dot(other)            → float
cross(other)          → Vector3*
add(other)            → void (modifica self)
scale(value)          → void (multiplica por valor)
```

### **Quaternion**
```cpp
namespace OgreQuaternionBindings
// Representação de rotação (x, y, z, w)
// Métodos para operações de rotação
```

### **SceneManager (Scene)**
```cpp
namespace OgreSceneBindings
// Construtores:
// Scene()

// Methods:
getRoot()                                    → SceneNode
createEntity(meshName)                       → Entity
createLight(name, type?)                     → Light
createCamera(name)                           → Camera
createCube(name, size, material, group?)     → ManualObject
createSphere(...)                            → ManualObject
createCylinder(...)                          → ManualObject
createCone(...)                              → ManualObject
createTorus(...)                             → ManualObject
createQuad(...)                              → ManualObject
createPlane(...)                             → ManualObject
createCapsule(...)                           → ManualObject
createParticleSystem(name, template)        → ParticleSystem
createRibbonTrail(name)                     → RibbonTrail
createBillboardSet(name, poolSize)          → BillboardSet

// Shadow Configuration:
setShadowTechnique(type)                    → void
setShadowTextureSize(size)                  → void
setShadowTextureCount(count)                → void
setShadowFarDistance(distance)              → void
setShadowDirLightTextureOffset(offset)      → void
setShadowTextureSelfShadow(enabled)         → void
setShadowCasterRenderBackFaces(enabled)     → void
setShadowColour(r, g, b)                    → void
setShadowTexturePixelFormat(format)         → void
setShadowTextureCountPerLightType(type, cnt)→ void

// Lighting:
setAmbientLight(r, g, b)                    → void

// Sky & Fog:
setSkyBox(enabled, material, distance, first) → void
setSkyDome(enabled, material, curvature, tiling, dist, first) → void
setSkyPlane(enabled, material, scale, tiling, first, bow)     → void
setSkyPlaneCustom(enabled, nx, ny, nz, dist, material, scale, tiling, first, bow) → void
setFog(mode, r, g, b, density, start, end)  → void
disableFog()                                 → void

// Shadow Types (setShadowTechnique):
// 0 = NONE
// 1 = STENCIL_ADDITIVE
// 2 = STENCIL_MODULATIVE
// 3 = TEXTURE_ADDITIVE
// 4 = TEXTURE_MODULATIVE
```

### **SceneNode**
```cpp
namespace OgreSceneNodeBindings
// Properties (Position):
posX, posY, posZ        (get/set)

// Properties (Scale):
scaleX, scaleY, scaleZ  (get/set)

// Properties (Rotation):
rotX, rotY, rotZ, rotW  (get/set - Quaternion)

// Methods:
setPosition(x, y, z)                        → void
getPosition()                               → Vector3*
setScale(x, y, z)                           → void
getScale()                                  → Vector3*
createChildSceneNode(name?, posX?, posY?, posZ?) → SceneNode
attachObject(entity)                        → void
detachObject(entity)                        → void
rotate(axisX, axisY, axisZ, angle, space?) → void
translate(x, y, z, space?)                  → void
lookAt(x, y, z, space?)                     → void
yaw(angle, space?)                          → void
pitch(angle, space?)                        → void
roll(angle, space?)                         → void
getParent()                                 → SceneNode
getChildCount()                             → int
getChild(index)                             → SceneNode

// Transform Space (para rotate, translate, lookAt):
// 0 = TS_LOCAL (relativo ao nó)
// 1 = TS_PARENT (relativo ao nó pai)
// 2 = TS_WORLD (relativo ao mundo)
```

### **Entity**
```cpp
namespace OgreEntityBindings
// Properties:
castShadows         (get/set bool)
visible             (get/set bool)

// Methods:
setMaterialName(name)                       → void
setCastShadows(enabled)                     → void
getAnimationState(name)                     → AnimationState
```

### **Light**
```cpp
namespace OgreLightBindings
// Properties:
visible             (get/set bool)

// Methods:
setDiffuse(r, g, b)                        → void
setSpecular(r, g, b)                       → void
setAttenuation(range, constant, linear, quad) → void
setPowerScale(power)                       → void
setDirection(x, y, z)                      → void
setRange(range)                            → void
setSpotlightRange(inner, outer, falloff)  → void
setSpotlightInnerAngle(angle)              → void
setSpotlightOuterAngle(angle)              → void
```

### **Camera**
```cpp
namespace OgreCameraBindings
// Properties:
fov         (em graus)
nearClip    (distância mínima)
farClip     (distância máxima)

// Methods:
setAutoAspectRatio(enabled)                 → void
setFOV(degrees)                             → void
setNearClip(distance)                       → void
setFarClip(distance)                        → void
getParentNode()                             → SceneNode
lookAt(x, y, z)                            → void
```

### **AnimationState**
```cpp
namespace OgreAnimationStateBindings
// Properties:
enabled             (get/set bool)
looped              (get/set bool)
timePosition        (get/set float)

// Methods:
setEnabled(bool)                            → void
setLooped(bool)                             → void
setTimePosition(time)                       → void
addTime(delta)                              → void
```

### **Outras Classes**
- **Material**: Configuração de materiais
- **Mesh**: Carregamento e manipulação de meshes
- **ParticleSystem**: Sistemas de partículas
- **BillboardSet**: Billboards (quadrados orientados para câmera)
- **RibbonTrail**: Trilhas de fita acompanhando objetos
- **ManualObject**: Meshes criados proceduralmente

---

## 💡 Exemplos Práticos

### **Exemplo 1: Criar uma Cena Simples**

```cpp
// C++ - Registrar os bindings
OgreSceneBindings::registerAll(vm);
OgreSceneNodeBindings::registerAll(vm);
OgreEntityBindings::registerAll(vm);
OgreCameraBindings::registerAll(vm);
OgreLightBindings::registerAll(vm);

// buLanguage (sintaxe correta com . e não :)
var scene = CreateScene()
var root = scene.getRoot()

// Criar entidade
var entity = scene.createEntity("Robot.mesh")
var node = root.createChild()
node.attachObject(entity)
node.setPosition(0, 0, 0)
node.setScale(2, 2, 2)

// Criar câmara
var camera = scene.createCamera("MainCamera")
var cameraNode = root.createChild()
cameraNode.attachObject(camera)
cameraNode.setPosition(0, 5, 10)
cameraNode.lookAt(0, 0, 0)

// Criar luz
var light = scene.createLight("Sun", 1)  // 1 = DIRECTIONAL
var lightNode = root.createChild()
lightNode.attachObject(light)
lightNode.setDirection(0.3, -0.75, 0.5)
light.setDiffuse(1.0, 0.95, 0.9)

// Configurar sombras
scene.setShadowTechnique(4)  // TEXTURE_MODULATIVE
scene.setShadowTextureSize(2048)
scene.setShadowTextureCount(3)
scene.setShadowFarDistance(300.0)
```

### **Exemplo 2: Animar uma Entidade**

```bulang
var entity = scene.createEntity("Ninja.mesh")
var node = root.createChild()
node.attachObject(entity)

// Obter estado de animação
var animState = entity.getAnimationState("Attack1")
animState.setEnabled(true)
animState.setLooped(false)

// Atualizar animação a cada frame (dentro do loop)
var dt = getDeltaTime()  // 16ms por frame
animState.addTime(dt)
```

### **Exemplo 3: Usar Vector3 para Operações**

```bulang
var vec1 = Vector3(1, 2, 3)
var vec2 = Vector3(4, 5, 6)

// Produto escalar
var dot = vec1.dot(vec2)

// Produto cruzado
var cross = vec1.cross(vec2)

// Normalizar
vec1.normalise()

// Adicionar
vec1.add(vec2)

// Escalar
vec1.scale(2.0)

// Obter comprimento
var length = vec1.length()
```

### **Exemplo 4: Manipular Posição e Rotação**

```bulang
var node = root.createChild()

// Modificar posição
node.posX = 10
node.posY = 5
node.posZ = -5

// ou usar método
node.setPosition(10, 5, -5)

// Modificar escala
node.scaleX = 2
node.scaleY = 2
node.scaleZ = 2

// Rotacionar (em graus)
// Space: 0=LOCAL, 1=PARENT, 2=WORLD
node.rotate(0, 1, 0, 45, 2)  // Rotar 45° em Y, mundo

// Transladar
node.translate(5, 0, 0, 0)   // Mover 5 unidades em X, local

// Olhar para ponto
node.lookAt(0, 0, 0, 2)      // Olhar para origem, mundo
```

---

## 🛠️ Guia de Criação de Novos Bindings

### **Passo 1: Criar o Arquivo**
Criar novo arquivo em `main/src/nova_classe.cpp`

### **Passo 2: Estrutura Base**
```cpp
#include "bindings.hpp"

namespace OgreNovaClasseBindings
{
    // 1. Constructor
    void *novaclasse_ctor(Interpreter *vm, int argCount, Value *args)
    {
        // Validar argumentos
        if (argCount < 1)
        {
            Error("NovaClasse constructor requires X argument");
            return nullptr;
        }

        // Criar objeto Ogre
        OgreNovaClasse *obj = new OgreNovaClasse(...);
        
        Info("NovaClasse created");
        return obj;
    }

    // 2. Destructor
    void novaclasse_dtor(Interpreter *vm, void *data)
    {
        // Se Ogre gerencia: não deletar
        // Se manual: delete static_cast<OgreNovaClasse*>(data);
    }

    // 3. Properties (Getters)
    Value novaclasse_getProperty(Interpreter *vm, void *data)
    {
        OgreNovaClasse *obj = static_cast<OgreNovaClasse *>(data);
        return vm->makeFloat(obj->getProperty());
    }

    // 4. Properties (Setters)
    void novaclasse_setProperty(Interpreter *vm, void *data, Value value)
    {
        OgreNovaClasse *obj = static_cast<OgreNovaClasse *>(data);
        obj->setProperty((float)value.asNumber());
    }

    // 5. Methods
    int novaclasse_method(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
            return 0;
        
        OgreNovaClasse *obj = static_cast<OgreNovaClasse *>(data);
        float param = (float)args[0].asNumber();
        
        obj->doSomething(param);
        return 0;  // ou push resultado e return 1
    }

    // 6. Registrar no VM
    void registerAll(Interpreter &vm)
    {
        NativeClassDef *sc = vm.registerNativeClass(
            "NovaClasse",
            novaclasse_ctor,
            novaclasse_dtor,
            0,
            false
        );

        // Properties
        vm.addNativeProperty(sc, "property", novaclasse_getProperty, novaclasse_setProperty);

        // Methods
        vm.addNativeMethod(sc, "method", novaclasse_method);
    }
}
```

### **Passo 3: Registrar em bindings.hpp**
```cpp
namespace OgreNovaClasseBindings
{
    void registerAll(Interpreter &vm);
}
```

### **Passo 4: Registrar em main.cpp**
```cpp
OgreNovaClasseBindings::registerAll(vm);
```

### **Passo 5: Adicionar ao CMakeLists.txt**
```cmake
target_sources(main PRIVATE
    src/nova_classe.cpp
)
```

---

## 📌 Dicas Importantes

1. **Gerenciamento de Memória**: Ogre gerencia maioria dos objetos. Não deletar!
2. **Conversão de Tipos**: Use casts seguros `static_cast`
3. **Validação**: Sempre validar argumentos e verificar nullptrs
4. **argCount em registerNativeClass**: Este é o **número de properties**, não de argumentos do construtor
5. **Transform Space**: 
   - 0 = LOCAL (relativo ao objeto)
   - 1 = PARENT (relativo ao pai)
   - 2 = WORLD (global)
6. **Ângulos**: Muitos métodos usam Radianos ou Graus (verificar Ogre::Degree vs Ogre::Radian)
7. **Retornos**: Objetos Ogre alocados com `new` devem ser deletados se criados manualmente
8. **Debugging**: Usar `Info()` e `Error()` para logging

---

## 📖 Referência Rápida

| Operação | Função |
|----------|--------|
| Criar entidade | `scene->createEntity(meshName)` |
| Posicionar | `node->setPosition(x, y, z)` |
| Escalar | `node->setScale(x, y, z)` |
| Rotacionar | `node->rotate(axisX, axisY, axisZ, angle, space)` |
| Animar | `entity->getAnimationState(name)->addTime(dt)` |
| Criar luz | `scene->createLight(name, type)` |
| Sombra | `scene->setShadowTechnique(type)` |
| Câmara | `scene->createCamera(name)` |
| Olhar para | `node->lookAt(x, y, z, space)` |
| Vector ops | `vec->dot(other)`, `vec->cross(other)`, `vec->normalise()` |

---

**Último Update:** Fevereiro 2026
**Versão Ogre:** 14.5
**Linguagem Script:** buLanguage
