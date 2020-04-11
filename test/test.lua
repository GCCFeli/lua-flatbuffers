local __g = _G

-- export global variable
_G.exports = {}
setmetatable(
    exports,
    {
        __newindex = function(_, name, value)
            rawset(__g, name, value)
        end,
        __index = function(_, name)
            return rawget(__g, name)
        end
    }
)

require("func")

exports.vector2 = vector2_native

local Protocol = require("protocol.Protocol").Protocol
local data = __TS__New(Protocol.ScenarioDataT)

data.name = 123

dump(data)

local flatbuffers = require("flatbuffers")
local ScenarioData = require("protocol.generated.ScriptComponents_serializer").ScenarioData

local builder = flatbuffers.Builder(1024)
print("---1")
local offset = ScenarioData.Pack(builder, data)
print("---2")
builder:Finish(offset)
print("---3")
local output = builder:Output()
print("---4")

local ba = flatbuffersnative.new_binaryarray(output)
print("---5")
print(ba)
local sd = ScenarioData.GetRootAsScenarioData(ba, 0)

print("haah")
print(sd.view)

local n = sd:Name()

print(n)
