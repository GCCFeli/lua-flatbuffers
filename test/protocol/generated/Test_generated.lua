--[[ Generated with https://github.com/TypeScriptToLua/TypeScriptToLua ]]
require("lualib_bundle");
__TS__SourceMapTraceBack(debug.getinfo(1).short_src, {["5"] = 8,["6"] = 8,["7"] = 8,["9"] = 9,["10"] = 8,["11"] = 23,["12"] = 23,["13"] = 23,["15"] = 24,["16"] = 25,["17"] = 23,["18"] = 41,["19"] = 41,["20"] = 41,["22"] = 45,["23"] = 46,["24"] = 47,["25"] = 48,["26"] = 49,["27"] = 50,["28"] = 51,["29"] = 52,["30"] = 53,["31"] = 54,["32"] = 55,["33"] = 56,["34"] = 57,["35"] = 58,["36"] = 59,["37"] = 60,["38"] = 61,["39"] = 62,["40"] = 63,["41"] = 64,["42"] = 65,["43"] = 66,["44"] = 67,["45"] = 68,["46"] = 69,["47"] = 70,["48"] = 71,["49"] = 72,["50"] = 73,["51"] = 74,["52"] = 75,["53"] = 76,["54"] = 77,["55"] = 78,["56"] = 79,["57"] = 80,["58"] = 81,["59"] = 41,["60"] = 98,["61"] = 98,["62"] = 98,["64"] = 99,["65"] = 98,["66"] = 114,["67"] = 114,["68"] = 114,["70"] = 115,["71"] = 114});
local ____exports = {}
____exports.NestedStructT = __TS__Class()
local NestedStructT = ____exports.NestedStructT
NestedStructT.name = "NestedStructT"
function NestedStructT.prototype.____constructor(self)
    self.nestedStructField = 0
end
____exports.SomeStructT = __TS__Class()
local SomeStructT = ____exports.SomeStructT
SomeStructT.name = "SomeStructT"
function SomeStructT.prototype.____constructor(self)
    self.someField = 0
    self.nestedStruct = __TS__New(____exports.NestedStructT)
end
____exports.TestDataT = __TS__Class()
local TestDataT = ____exports.TestDataT
TestDataT.name = "TestDataT"
function TestDataT.prototype.____constructor(self)
    self.entity = -1
    self.teamEntity = 0
    self.hasBall = false
    self.lastHasBall = false
    self.index = 0
    self.characterId = 0
    self.roleId = 0
    self.isAtDefenseTargetPosition = false
    self.markedByAthleteId = 0
    self.someLong = 0
    self.someFix64 = 0
    self.someStruct = __TS__New(____exports.SomeStructT)
    self.someArray = {}
    self.someVector2 = vector2.zero
    self.someObj = nil
    self.someOtherArray = {}
    self.someEmptyObj = nil
    self.someNumberArray = {}
    self.someVector2Array = {}
    self.optionalNumber = nil
    self.optionalNumber1 = nil
    self.optionalNumber2 = nil
    self.optionalStruct = nil
    self.optionalStruct1 = nil
    self.intArray = {}
    self.str = ""
    self.arrayOfArray = {}
    self.bool1 = false
    self.bool2 = true
    self.bool3 = false
    self.bool4 = true
    self.optionalInt = nil
    self.optionalInt1 = nil
    self.optionalInt2 = nil
    self.optionalBool = nil
    self.optionalBool1 = nil
    self.optionalBool2 = nil
end
____exports.SomethingElseT = __TS__Class()
local SomethingElseT = ____exports.SomethingElseT
SomethingElseT.name = "SomethingElseT"
function SomethingElseT.prototype.____constructor(self)
    self.someField = 0
end
____exports.TableWithArrayT = __TS__Class()
local TableWithArrayT = ____exports.TableWithArrayT
TableWithArrayT.name = "TableWithArrayT"
function TableWithArrayT.prototype.____constructor(self)
    self.someArray = {}
end
return ____exports
