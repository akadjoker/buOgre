# Guia Completo de Bindings para buLanguage

Um guia genérico para criar **bindings (ligações) entre C++ e buLanguage** para qualquer projeto, não apenas Ogre.

## 📋 Índice

1. [Conceitos Fundamentais](#conceitos-fundamentais)
2. [Arquitetura de Bindings](#arquitetura-de-bindings)
3. [Tipos de Dados e Conversões](#tipos-de-dados-e-conversões)
4. [Criando um Binding Simples](#criando-um-binding-simples)
5. [Exemplo Completo: Classe MathVector](#exemplo-completo-classe-mathvector)
6. [Exemplo Completo: Classe Database](#exemplo-completo-classe-database)
7. [Padrões Avançados](#padrões-avançados)
8. [Debugging e Testes](#debugging-e-testes)

---

## 🎯 Conceitos Fundamentais

### O que é um Binding?

Um **binding** é um "tradutor" que permite código buLanguage (script) chamar código C++ (nativo).

```
buLanguage Script         C++ Nativo
     ↓                        ↑
var obj = MyClass(42)  →  new MyClass(42)
obj.getValue()         →  obj->getValue()
obj.setValue(100)      →  obj->setValue(100)
```

### Por que Bindings?

- **Desempenho**: C++ é muito mais rápido que scripts
- **Acesso a Sistema**: Operações de baixo nível, I/O, etc.
- **Bibliotecas Externas**: Integrar libs C++
- **Game Logic**: Física, gráficos, IA em C++

---

## 🏗️ Arquitetura de Bindings

### Estrutura Básica

```
Projeto/
├── src/
│   ├── my_class.hpp       # Classe C++ nativa
│   ├── my_class.cpp       # Implementação
│   ├── my_class_bindings.hpp  # Declaração dos bindings
│   └── my_class_bindings.cpp  # Implementação dos bindings
├── main.cpp               # Setup dos bindings
└── scripts/
    └── main.bu            # Script buLanguage
```

### Fluxo de Execução

```
Script buLanguage
    ↓
Interpreter (VM)
    ↓
Native Binding Function
    ↓
C++ Native Code
    ↓
Retorna resultado
    ↓
Script buLanguage
```

---

## 🔄 Tipos de Dados e Conversões

### Tabela de Conversão Completa

| buLanguage | C++ Get | C++ Create | C++ Get Type |
|---|---|---|---|
| **Number** | `args[i].asNumber()` → `double` | `vm->makeFloat(42.0)` | Sempre `double` |
| **Boolean** | `args[i].asBool()` → `bool` | `vm->makeBool(true)` | `bool` |
| **String** | `args[i].asStringChars()` → `const char*` | `vm->makeString("text")` | `const char*` |
| **Pointer** | `args[i].asPointer()` → `void*` | `vm->pushPointer(ptr)` | Raw C pointer |
| **NativeClass** | `args[i].asNativeClassInstance()` | `vm->makeNativeClassInstance()` | Custom object |
| **Array** | Não suportado diretamente | Não suportado | Usar loops |

### Exemplos de Conversão

```cpp
// ✅ Number (double)
double value = args[0].asNumber();        // Get
vm->pushDouble(42.0);                      // Push
return 1;

// ✅ Number (int)
int value = (int)args[0].asNumber();      // Get
vm->pushInt(42);                           // Push
return 1;

// ✅ Boolean
bool enabled = args[0].asBool();           // Get
vm->pushBool(true);                        // Push
return 1;

// ✅ String
const char* text = args[0].asStringChars();// Get
vm->pushString("hello");                   // Push
return 1;

// ✅ Pointer (raw C pointer)
MyClass* obj = (MyClass*)args[0].asPointer();
vm->pushPointer((void*)newObj);
return 1;

// ✅ NativeClass Instance
NativeClassInstance* inst = args[0].asNativeClassInstance();
MyClass* obj = (MyClass*)inst->userData;

// ✅ Retornar um objeto NativeClass
NativeClassDef *classDef = nullptr;
vm->tryGetNativeClassDef("MyClass", &classDef);
Value objValue = vm->makeNativeClassInstance(false);
NativeClassInstance *instance = objValue.asNativeClassInstance();
instance->klass = classDef;
instance->userData = (void *)myObject;
vm->push(objValue);
return 1;
```

---

## 🛠️ Criando um Binding Simples

### Passo 1: Classe C++ Nativa

**arquivo: `src/calculator.hpp`**

```cpp
#pragma once

class Calculator
{
public:
    Calculator();
    ~Calculator();
    
    double add(double a, double b);
    double subtract(double a, double b);
    double multiply(double a, double b);
    double divide(double a, double b);
    
    void reset();
    double getLastResult();
    
private:
    double lastResult;
};
```

**arquivo: `src/calculator.cpp`**

```cpp
#include "calculator.hpp"

Calculator::Calculator() : lastResult(0.0) {}
Calculator::~Calculator() {}

double Calculator::add(double a, double b)
{
    lastResult = a + b;
    return lastResult;
}

double Calculator::subtract(double a, double b)
{
    lastResult = a - b;
    return lastResult;
}

double Calculator::multiply(double a, double b)
{
    lastResult = a * b;
    return lastResult;
}

double Calculator::divide(double a, double b)
{
    if (b == 0.0)
        return 0.0;  // ou lançar erro
    lastResult = a / b;
    return lastResult;
}

void Calculator::reset()
{
    lastResult = 0.0;
}

double Calculator::getLastResult()
{
    return lastResult;
}
```

### Passo 2: Arquivo de Bindings

**arquivo: `src/calculator_bindings.hpp`**

```cpp
#pragma once

#include "interpreter.hpp"

namespace CalculatorBindings
{
    void registerAll(Interpreter &vm);
}
```

**arquivo: `src/calculator_bindings.cpp`**

```cpp
#include "calculator_bindings.hpp"
#include "calculator.hpp"

namespace CalculatorBindings
{
    // ============ CONSTRUCTOR ============
    void *calculator_ctor(Interpreter *vm, int argCount, Value *args)
    {
        Calculator *calc = new Calculator();
        Info("Calculator created");
        return calc;
    }

    // ============ DESTRUCTOR ============
    void calculator_dtor(Interpreter *vm, void *data)
    {
        delete static_cast<Calculator *>(data);
        Info("Calculator destroyed");
    }

    // ============ METHODS ============

    // add(a, b) -> number
    int calculator_add(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 2)
            return 0;
        
        Calculator *calc = static_cast<Calculator *>(data);
        double a = args[0].asNumber();
        double b = args[1].asNumber();
        double result = calc->add(a, b);
        
        vm->pushDouble(result);
        return 1;
    }

    // subtract(a, b) -> number
    int calculator_subtract(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 2)
            return 0;
        
        Calculator *calc = static_cast<Calculator *>(data);
        double a = args[0].asNumber();
        double b = args[1].asNumber();
        double result = calc->subtract(a, b);
        
        vm->pushDouble(result);
        return 1;
    }

    // multiply(a, b) -> number
    int calculator_multiply(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 2)
            return 0;
        
        Calculator *calc = static_cast<Calculator *>(data);
        double a = args[0].asNumber();
        double b = args[1].asNumber();
        double result = calc->multiply(a, b);
        
        vm->pushDouble(result);
        return 1;
    }

    // divide(a, b) -> number
    int calculator_divide(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 2)
            return 0;
        
        Calculator *calc = static_cast<Calculator *>(data);
        double a = args[0].asNumber();
        double b = args[1].asNumber();
        double result = calc->divide(a, b);
        
        vm->pushDouble(result);
        return 1;
    }

    // reset() -> void
    int calculator_reset(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Calculator *calc = static_cast<Calculator *>(data);
        calc->reset();
        return 0;  // Sem retorno
    }

    // getLastResult() -> number
    int calculator_getLastResult(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Calculator *calc = static_cast<Calculator *>(data);
        double result = calc->getLastResult();
        
        vm->pushDouble(result);
        return 1;
    }

    // ============ REGISTER ============
    void registerAll(Interpreter &vm)
    {
        NativeClassDef *sc = vm.registerNativeClass(
            "Calculator",        // Nome
            calculator_ctor,     // Constructor
            calculator_dtor,     // Destructor
            1,                   // Número de properties (0 neste caso)
            false                // Ogre/Sistema gerencia memória
        );

        // Registar métodos
        vm.addNativeMethod(sc, "add", calculator_add);
        vm.addNativeMethod(sc, "subtract", calculator_subtract);
        vm.addNativeMethod(sc, "multiply", calculator_multiply);
        vm.addNativeMethod(sc, "divide", calculator_divide);
        vm.addNativeMethod(sc, "reset", calculator_reset);
        vm.addNativeMethod(sc, "getLastResult", calculator_getLastResult);

        Info("Calculator bindings registered");
    }
}
```

### Passo 3: Registar no Main

**arquivo: `main.cpp`**

```cpp
#include "interpreter.hpp"
#include "calculator_bindings.hpp"

int main()
{
    Interpreter vm;
    
    // Registar bindings
    CalculatorBindings::registerAll(vm);
    
    // Carregar e executar script
    vm.loadScript("scripts/main.bu");
    
    return 0;
}
```

### Passo 4: Usar no Script

**arquivo: `scripts/main.bu`**

```bulang
// Criar calculadora
var calc = Calculator()

// Usar métodos
var result1 = calc.add(10, 5)
print(result1)  // Output: 15

var result2 = calc.multiply(4, 3)
print(result2)  // Output: 12

var result3 = calc.divide(20, 4)
print(result3)  // Output: 5

var last = calc.getLastResult()
print(last)     // Output: 5

calc.reset()
```

---

## 📚 Exemplo Completo: Classe MathVector

Classe para operações com vetores 3D.

**arquivo: `src/mathvector.hpp`**

```cpp
#pragma once

class MathVector
{
public:
    MathVector();
    MathVector(double x, double y, double z);
    
    // Getters
    double getX() const { return x; }
    double getY() const { return y; }
    double getZ() const { return z; }
    
    // Setters
    void setX(double val) { x = val; }
    void setY(double val) { y = val; }
    void setZ(double val) { z = val; }
    
    // Operações
    double length();
    void normalize();
    double dot(const MathVector &other);
    MathVector cross(const MathVector &other);
    void add(const MathVector &other);
    void scale(double factor);
    
    // Debug
    const char* toString();

private:
    double x, y, z;
    char buffer[64];
};
```

**arquivo: `src/mathvector_bindings.cpp`**

```cpp
#include "mathvector_bindings.hpp"
#include "mathvector.hpp"
#include <cmath>

namespace MathVectorBindings
{
    // ============ CONSTRUCTOR ============
    void *mathvector_ctor(Interpreter *vm, int argCount, Value *args)
    {
        MathVector *vec = nullptr;
        
        if (argCount >= 3)
        {
            double x = args[0].asNumber();
            double y = args[1].asNumber();
            double z = args[2].asNumber();
            vec = new MathVector(x, y, z);
        }
        else if (argCount == 1)
        {
            double val = args[0].asNumber();
            vec = new MathVector(val, val, val);
        }
        else
        {
            vec = new MathVector();
        }
        
        Info("MathVector created: (%.2f, %.2f, %.2f)", vec->getX(), vec->getY(), vec->getZ());
        return vec;
    }

    void mathvector_dtor(Interpreter *vm, void *data)
    {
        delete static_cast<MathVector *>(data);
    }

    // ============ PROPERTIES (Getters) ============
    Value mathvector_getX(Interpreter *vm, void *data)
    {
        return vm->makeFloat(static_cast<MathVector *>(data)->getX());
    }

    Value mathvector_getY(Interpreter *vm, void *data)
    {
        return vm->makeFloat(static_cast<MathVector *>(data)->getY());
    }

    Value mathvector_getZ(Interpreter *vm, void *data)
    {
        return vm->makeFloat(static_cast<MathVector *>(data)->getZ());
    }

    // ============ PROPERTIES (Setters) ============
    void mathvector_setX(Interpreter *vm, void *data, Value value)
    {
        static_cast<MathVector *>(data)->setX(value.asNumber());
    }

    void mathvector_setY(Interpreter *vm, void *data, Value value)
    {
        static_cast<MathVector *>(data)->setY(value.asNumber());
    }

    void mathvector_setZ(Interpreter *vm, void *data, Value value)
    {
        static_cast<MathVector *>(data)->setZ(value.asNumber());
    }

    // ============ METHODS ============

    int mathvector_length(Interpreter *vm, void *data, int argCount, Value *args)
    {
        MathVector *vec = static_cast<MathVector *>(data);
        double len = vec->length();
        vm->pushDouble(len);
        return 1;
    }

    int mathvector_normalize(Interpreter *vm, void *data, int argCount, Value *args)
    {
        MathVector *vec = static_cast<MathVector *>(data);
        vec->normalize();
        return 0;
    }

    int mathvector_dot(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            Error("dot(): requires 1 argument");
            return 0;
        }

        MathVector *self = static_cast<MathVector *>(data);
        
        // Obter outro vetor
        NativeClassInstance *otherInst = args[0].asNativeClassInstance();
        if (!otherInst)
        {
            Error("dot(): argument must be a MathVector");
            return 0;
        }

        MathVector *other = static_cast<MathVector *>(otherInst->userData);
        double result = self->dot(*other);
        
        vm->pushDouble(result);
        return 1;
    }

    int mathvector_add(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            Error("add(): requires 1 argument");
            return 0;
        }

        MathVector *self = static_cast<MathVector *>(data);
        NativeClassInstance *otherInst = args[0].asNativeClassInstance();
        
        if (!otherInst)
        {
            Error("add(): argument must be a MathVector");
            return 0;
        }

        MathVector *other = static_cast<MathVector *>(otherInst->userData);
        self->add(*other);
        return 0;
    }

    int mathvector_scale(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
        {
            Error("scale(): requires 1 argument");
            return 0;
        }

        MathVector *vec = static_cast<MathVector *>(data);
        double factor = args[0].asNumber();
        vec->scale(factor);
        return 0;
    }

    // ============ REGISTER ============
    void registerAll(Interpreter &vm)
    {
        NativeClassDef *sc = vm.registerNativeClass(
            "MathVector",
            mathvector_ctor,
            mathvector_dtor,
            3,  // 3 properties: x, y, z
            false
        );

        // Properties
        vm.addNativeProperty(sc, "x", mathvector_getX, mathvector_setX);
        vm.addNativeProperty(sc, "y", mathvector_getY, mathvector_setY);
        vm.addNativeProperty(sc, "z", mathvector_getZ, mathvector_setZ);

        // Methods
        vm.addNativeMethod(sc, "length", mathvector_length);
        vm.addNativeMethod(sc, "normalize", mathvector_normalize);
        vm.addNativeMethod(sc, "dot", mathvector_dot);
        vm.addNativeMethod(sc, "add", mathvector_add);
        vm.addNativeMethod(sc, "scale", mathvector_scale);

        Info("MathVector bindings registered");
    }
}
```

**uso no script:**

```bulang
// Criar vetores
var v1 = MathVector(1, 2, 3)
var v2 = MathVector(4, 5, 6)

// Acessar properties
print(v1.x)  // 1
print(v1.y)  // 2

// Modificar properties
v1.x = 10

// Chamar métodos
var len = v1.length()
var dotProduct = v1.dot(v2)
v1.scale(2.0)
v1.add(v2)
```

---

## 💾 Exemplo Completo: Classe Database

Sistema simples de base de dados com bindings.

**arquivo: `src/database.hpp`**

```cpp
#pragma once
#include <map>
#include <string>

class Database
{
public:
    Database(const char* name);
    ~Database();
    
    void setValue(const char* key, double value);
    double getValue(const char* key);
    bool hasKey(const char* key);
    void removeKey(const char* key);
    void clear();
    int getKeyCount();
    
    const char* getName() const { return name.c_str(); }
    
private:
    std::string name;
    std::map<std::string, double> data;
};
```

**arquivo: `src/database_bindings.cpp`**

```cpp
#include "database_bindings.hpp"
#include "database.hpp"

namespace DatabaseBindings
{
    // Constructor
    void *database_ctor(Interpreter *vm, int argCount, Value *args)
    {
        const char *name = "DefaultDB";
        if (argCount >= 1)
            name = args[0].asStringChars();
        
        Database *db = new Database(name);
        Info("Database '%s' created", name);
        return db;
    }

    void database_dtor(Interpreter *vm, void *data)
    {
        Database *db = static_cast<Database *>(data);
        Info("Database '%s' destroyed", db->getName());
        delete db;
    }

    // setValue(key, value)
    int database_setValue(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 2)
            return 0;
        
        Database *db = static_cast<Database *>(data);
        const char *key = args[0].asStringChars();
        double value = args[1].asNumber();
        
        db->setValue(key, value);
        Info("DB: Set '%s' = %.2f", key, value);
        return 0;
    }

    // getValue(key) -> number
    int database_getValue(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
            return 0;
        
        Database *db = static_cast<Database *>(data);
        const char *key = args[0].asStringChars();
        double value = db->getValue(key);
        
        vm->pushDouble(value);
        return 1;
    }

    // hasKey(key) -> boolean
    int database_hasKey(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
            return 0;
        
        Database *db = static_cast<Database *>(data);
        const char *key = args[0].asStringChars();
        bool exists = db->hasKey(key);
        
        vm->pushBool(exists);
        return 1;
    }

    // removeKey(key)
    int database_removeKey(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1)
            return 0;
        
        Database *db = static_cast<Database *>(data);
        const char *key = args[0].asStringChars();
        db->removeKey(key);
        
        Info("DB: Removed '%s'", key);
        return 0;
    }

    // clear()
    int database_clear(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Database *db = static_cast<Database *>(data);
        db->clear();
        Info("DB: Cleared all entries");
        return 0;
    }

    // getKeyCount() -> number
    int database_getKeyCount(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Database *db = static_cast<Database *>(data);
        int count = db->getKeyCount();
        vm->pushInt(count);
        return 1;
    }

    // Register
    void registerAll(Interpreter &vm)
    {
        NativeClassDef *sc = vm.registerNativeClass(
            "Database",
            database_ctor,
            database_dtor,
            1,
            false
        );

        vm.addNativeMethod(sc, "setValue", database_setValue);
        vm.addNativeMethod(sc, "getValue", database_getValue);
        vm.addNativeMethod(sc, "hasKey", database_hasKey);
        vm.addNativeMethod(sc, "removeKey", database_removeKey);
        vm.addNativeMethod(sc, "clear", database_clear);
        vm.addNativeMethod(sc, "getKeyCount", database_getKeyCount);

        Info("Database bindings registered");
    }
}
```

**uso no script:**

```bulang
var db = Database("game_data")

db.setValue("playerHealth", 100.0)
db.setValue("playerMana", 50.0)
db.setValue("playerExp", 1500.0)

if (db.hasKey("playerHealth"))
{
    var health = db.getValue("playerHealth")
    print("Health: " + health)
}

var count = db.getKeyCount()
print("Total keys: " + count)

db.removeKey("playerMana")
db.clear()
```

---

## 🚀 Padrões Avançados

### 1. Retornar Objetos Criados em C++

```cpp
// Criar novo objeto em C++ e retornar
int myClass_create(Interpreter *vm, void *data, int argCount, Value *args)
{
    // Criar objeto C++
    MyNewClass *newObj = new MyNewClass();
    
    // Obter classe do VM
    NativeClassDef *classDef = nullptr;
    if (!vm->tryGetNativeClassDef("MyNewClass", &classDef))
    {
        Error("MyNewClass not found");
        return 0;
    }
    
    // Criar wrapper buLanguage
    Value objValue = vm->makeNativeClassInstance(false);
    NativeClassInstance *instance = objValue.asNativeClassInstance();
    instance->klass = classDef;
    instance->userData = (void *)newObj;
    
    vm->push(objValue);
    return 1;  // Retorna 1 valor
}
```

### 2. Funções Globais (não métodos de classe)

```cpp
// Função global acessível do script
int global_print_message(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1)
        return 0;
    
    const char *message = args[0].asStringChars();
    printf("Message: %s\n", message);
    return 0;
}

// Registar no main.cpp
vm->registerNative("printMessage", global_print_message, 1);
```

**uso:**

```bulang
printMessage("Hello World")
```

### 3. Múltiplos Retornos

Sim, **buLanguage SUPORTA múltiplos retornos**! Basta fazer múltiplos `vm->push()` e retornar o número certo.

```cpp
int myClass_getValues(Interpreter *vm, void *data, int argCount, Value *args)
{
    MyClass *obj = static_cast<MyClass *>(data);
    
    // Push 3 valores na ordem
    vm->pushDouble(obj->getX());
    vm->pushDouble(obj->getY());
    vm->pushDouble(obj->getZ());
    
    return 3;  // Retorna 3 valores!
}
```

**uso no script:**

```bulang
var obj = MyClass()

// Múltiplos retornos com sintaxe de parêntesis
var (x, y, z) = obj.getValues()  // Captura 3 valores
print(x)  // Primeiro valor
print(y)  // Segundo valor
print(z)  // Terceiro valor
```

### Exemplo Completo: Retornos Múltiplos

```cpp
// Retornar X, Y, Z de um ponto
int point_getCoordinates(Interpreter *vm, void *data, int argCount, Value *args)
{
    Point *pt = static_cast<Point *>(data);
    
    vm->pushDouble(pt->x);
    vm->pushDouble(pt->y);
    vm->pushDouble(pt->z);
    
    return 3;  // Retorna 3 valores
}

// Retornar R, G, B, A de uma cor
int color_getRGBA(Interpreter *vm, void *data, int argCount, Value *args)
{
    Color *col = static_cast<Color *>(data);
    
    vm->pushDouble(col->r);
    vm->pushDouble(col->g);
    vm->pushDouble(col->b);
    vm->pushDouble(col->a);
    
    return 4;  // Retorna 4 valores
}

// Retornar múltiplos tipos
int math_divmod(Interpreter *vm, void *data, int argCount, Value *args)
{
    if (argCount < 2)
        return 0;
    
    int a = (int)args[0].asNumber();
    int b = (int)args[1].asNumber();
    
    if (b == 0)
        return 0;
    
    int quotient = a / b;
    int remainder = a % b;
    
    vm->pushInt(quotient);               // Quociente
    vm->pushInt(remainder);              // Resto
    
    return 2;  // Retorna 2 valores
}
```

### 4. Validação de Argumentos Robusta

```cpp
int myClass_process(Interpreter *vm, void *data, int argCount, Value *args)
{
    if (argCount < 2)
    {
        Error("process(): requires 2 arguments, got %d", argCount);
        return 0;
    }
    
    if (!args[0].isNumber())
    {
        Error("process(): first argument must be a number");
        return 0;
    }
    
    if (!args[1].isString())
    {
        Error("process(): second argument must be a string");
        return 0;
    }
    
    double value = args[0].asNumber();
    const char *text = args[1].asStringChars();
    
    // ... processar
    
    return 0;
}
```

---

## 🐛 Debugging e Testes

### Logging Útil

```cpp
Info("MyClass created with value: %d", someValue);
Error("Invalid argument: %s", argName);
Warning("Deprecated method called");
```

### Verificar Tipos (se disponível)

```cpp
// Exemplos de como verificar tipos
if (args[0].isNumber())  { /* ... */ }
if (args[0].isBool())    { /* ... */ }
if (args[0].isString())  { /* ... */ }
if (args[0].isNil())     { /* ... */ }
```

### Script de Teste

```bulang
// test.bu - Testar bindings

print("=== Testing Calculator ===")
var calc = Calculator()

var r1 = calc.add(10, 5)
assert(r1 == 15, "add failed")

var r2 = calc.multiply(4, 3)
assert(r2 == 12, "multiply failed")

var r3 = calc.divide(20, 4)
assert(r3 == 5, "divide failed")

print("All tests passed!")

def assert(condition, message)
{
    if (!condition)
    {
        print("FAIL: " + message)
    }
    else
    {
        print("PASS: " + message)
    }
}
```

---

## 📋 Checklist de Implementação

- [ ] Classe C++ implementada (`.hpp` e `.cpp`)
- [ ] Arquivo de bindings criado (`.hpp` e `.cpp`)
- [ ] Constructor binding implementado
- [ ] Destructor binding implementado
- [ ] Properties bindings (getter/setter) se necessário
- [ ] Methods bindings implementados
- [ ] `registerAll()` função implementada
- [ ] Bindings registados em `bindings.hpp`
- [ ] Bindings registados em `main.cpp`
- [ ] CMakeLists.txt atualizado com novos ficheiros
- [ ] Script de teste criado
- [ ] Testes executados com sucesso
- [ ] Documentação atualizada

---

## 🎓 Resumo dos Passos

1. **Criar classe C++** com lógica nativa
2. **Criar bindings** com funções para cada método/property
3. **Registar** os bindings na VM
4. **Usar** do script buLanguage como se fosse classe nativa

**É sempre o mesmo padrão:**
```
Classe C++ → Bindings → Registar no VM → Usar no Script
```

---

**Última atualização:** Fevereiro 2026
**Linguagem Script:** buLanguage
**Versão de Referência:** 1.0
