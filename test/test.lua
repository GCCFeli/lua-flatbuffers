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
local offset = ScenarioData.Pack(builder, data)
builder:Finish(offset)
local output = builder:Output()

local ba = flatbuffersnative.new_binaryarray(output)
local sd = ScenarioData.GetRootAsScenarioData(ba, 0)

print(sd:Name())
print(sd:Phase())
print(sd:EnterPosition():X())
print(sd:EnterPosition():Y())
