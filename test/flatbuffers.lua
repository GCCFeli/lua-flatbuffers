local m = {}

m.Builder = require("flatbuffers.builder").New
m.N = flatbuffersnative.N
m.view = {}
m.view.New = flatbuffersnative.new_view

m.Protocol = {
    Vector2Fix64 = {},
    Vector3Fix64 = {}
}

m.Protocol.Vector2Fix64.Pack = function(builder, _o)
    local _x = _o.x
    local _y = _o.y

    return require("protocol.generated.Native_serializer").Vector2Fix64.CreateVector2Fix64(builder, _x, _y)
end

m.Protocol.Vector2Fix64.UnPack = function(_self)
    return vector2.new(_self:X(), _self:Y())
end

m.Protocol.Vector3Fix64.Pack = function(builder, _o)
    local _x = _o.x
    local _y = _o.y
    local _z = _o.z

    return require("protocol.generated.Native_serializer").Vector3Fix64.CreateVector3Fix64(builder, _x, _y, _z)
end

m.Protocol.Vector3Fix64.UnPack = function(_self)
    return Vector3Fix64.new(_self:X(), _self:Y(), _self:Z())
end

return m
