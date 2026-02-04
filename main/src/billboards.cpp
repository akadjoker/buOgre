#include "bindings.hpp"
#include <OgreBillboardSet.h>
#include <OgreBillboard.h>

// ============== OGRE BILLBOARD BINDINGS ==============

namespace OgreBillboardBindings
{
  

    // ========== BILLBOARD SET METHODS ==========

    // setMaterialName(materialName)
    int bbset_setMaterialName(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::BillboardSet *bbSet = static_cast<Ogre::BillboardSet *>(data);
        if (bbSet) bbSet->setMaterialName(args[0].asStringChars());
        return 0;
    }

    // createBillboard(x, y, z, color) -> returns index
    int bbset_createBillboard(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3)
        {
            Error("createBillboard: requires position (x, y, z)");
            return 0;
        }

        Ogre::BillboardSet *bbSet = static_cast<Ogre::BillboardSet *>(data);
        if (!bbSet) return 0;

        Ogre::Vector3 position(
            (float)args[0].asNumber(),
            (float)args[1].asNumber(),
            (float)args[2].asNumber()
        );

        Ogre::ColourValue color = Ogre::ColourValue::White;

        if (argCount >= 7)
        {
            color.r = (float)args[3].asNumber();
            color.g = (float)args[4].asNumber();
            color.b = (float)args[5].asNumber();
            color.a = (float)args[6].asNumber();
        }

        Ogre::Billboard *billboard = bbSet->createBillboard(position, color);

        // Return billboard index (for later reference)
        // Note: In a real implementation, we'd track billboards better
        vm->pushInt(bbSet->getNumBillboards() - 1);
        return 1;
    }

    // setBillboardPosition(index, x, y, z)
    int bbset_setBillboardPosition(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 4) return 0;

        Ogre::BillboardSet *bbSet = static_cast<Ogre::BillboardSet *>(data);
        if (!bbSet) return 0;

        int index = (int)args[0].asNumber();

        if (index >= 0 && index < (int)bbSet->getNumBillboards())
        {
            Ogre::Billboard *billboard = bbSet->getBillboard((unsigned int)index);
            billboard->setPosition(
                (float)args[1].asNumber(),
                (float)args[2].asNumber(),
                (float)args[3].asNumber()
            );
        }

        return 0;
    }

    // setBillboardColour(index, r, g, b, a)
    int bbset_setBillboardColour(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 5) return 0;

        Ogre::BillboardSet *bbSet = static_cast<Ogre::BillboardSet *>(data);
        if (!bbSet) return 0;

        int index = (int)args[0].asNumber();

        if (index >= 0 && index < (int)bbSet->getNumBillboards())
        {
            Ogre::Billboard *billboard = bbSet->getBillboard((unsigned int)index);
            billboard->setColour(Ogre::ColourValue(
                (float)args[1].asNumber(),
                (float)args[2].asNumber(),
                (float)args[3].asNumber(),
                (float)args[4].asNumber()
            ));
        }

        return 0;
    }

    // setBillboardDimensions(width, height)
    int bbset_setBillboardDimensions(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 2) return 0;

        Ogre::BillboardSet *bbSet = static_cast<Ogre::BillboardSet *>(data);
        if (bbSet)
        {
            bbSet->setDefaultDimensions(
                (float)args[0].asNumber(),
                (float)args[1].asNumber()
            );
        }
        return 0;
    }

    // clear()
    int bbset_clear(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::BillboardSet *bbSet = static_cast<Ogre::BillboardSet *>(data);
        if (bbSet) bbSet->clear();
        return 0;
    }

    // setBillboardType(type)
    // 0 = POINT (camera facing), 1 = ORIENTED_COMMON, 2 = ORIENTED_SELF, 3 = PERPENDICULAR_COMMON, 4 = PERPENDICULAR_SELF
    int bbset_setBillboardType(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::BillboardSet *bbSet = static_cast<Ogre::BillboardSet *>(data);
        if (!bbSet) return 0;

        int type = (int)args[0].asNumber();

        Ogre::BillboardType bbType = Ogre::BBT_POINT;
        switch (type)
        {
            case 0: bbType = Ogre::BBT_POINT; break;
            case 1: bbType = Ogre::BBT_ORIENTED_COMMON; break;
            case 2: bbType = Ogre::BBT_ORIENTED_SELF; break;
            case 3: bbType = Ogre::BBT_PERPENDICULAR_COMMON; break;
            case 4: bbType = Ogre::BBT_PERPENDICULAR_SELF; break;
            default: bbType = Ogre::BBT_POINT; break;
        }

        bbSet->setBillboardType(bbType);
        return 0;
    }

    // setCommonDirection(x, y, z)
    int bbset_setCommonDirection(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 3) return 0;

        Ogre::BillboardSet *bbSet = static_cast<Ogre::BillboardSet *>(data);
        if (bbSet)
        {
            bbSet->setCommonDirection(Ogre::Vector3(
                (float)args[0].asNumber(),
                (float)args[1].asNumber(),
                (float)args[2].asNumber()
            ));
        }
        return 0;
    }

    // getNumBillboards() -> int
    int bbset_getNumBillboards(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Ogre::BillboardSet *bbSet = static_cast<Ogre::BillboardSet *>(data);
        vm->pushInt(bbSet ? bbSet->getNumBillboards() : 0);
        return 1;
    }

    // setCastShadows(bool)
    int bbset_setCastShadows(Interpreter *vm, void *data, int argCount, Value *args)
    {
        if (argCount < 1) return 0;

        Ogre::BillboardSet *bbSet = static_cast<Ogre::BillboardSet *>(data);
        if (bbSet) bbSet->setCastShadows(args[0].asBool());
        return 0;
    }

    void registerAll(Interpreter &vm)
    {
        // Register BillboardSet class
        NativeClassDef *bbSet = vm.registerNativeClass(
            "BillboardSet",
            nullptr,
            nullptr,
            0,
            false
        );

        // Methods
        vm.addNativeMethod(bbSet, "setMaterialName", bbset_setMaterialName);
        vm.addNativeMethod(bbSet, "createBillboard", bbset_createBillboard);
        vm.addNativeMethod(bbSet, "setBillboardPosition", bbset_setBillboardPosition);
        vm.addNativeMethod(bbSet, "setBillboardColour", bbset_setBillboardColour);
        vm.addNativeMethod(bbSet, "setBillboardDimensions", bbset_setBillboardDimensions);
        vm.addNativeMethod(bbSet, "clear", bbset_clear);
        vm.addNativeMethod(bbSet, "setBillboardType", bbset_setBillboardType);
        vm.addNativeMethod(bbSet, "setCommonDirection", bbset_setCommonDirection);
        vm.addNativeMethod(bbSet, "getNumBillboards", bbset_getNumBillboards);
        vm.addNativeMethod(bbSet, "setCastShadows", bbset_setCastShadows);

 

        Info("Billboard bindings registered");
    }

} // namespace OgreBillboardBindings
