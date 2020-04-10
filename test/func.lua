function exports.printf(fmt, ...)
    print(string.format(fmt, ...))
end

function exports.dump(object, label, maxNest)
    local result = vardump(object, label, maxNest)
    local str = table.concat(result, "\n")

    local traceback = debug.traceback("", 2)
    print(str .. "\n" .. (traceback or "dump from out of lua-state"))
    return str
end

function exports.vardump(object, label, maxNest)
    local lookupTable = {}
    local indexed = {}
    local result = {}

    local function _v(v)
        if type(v) == "string" then
            local str = string.gsub(v, "\\", "\\\\")
            str = string.gsub(str, '"', '\\"')
            return '"' .. str .. '"'
        end
        if type(v) == "number" or type(v) == "boolean" then
            return tostring(v)
        end
        if type(v) == "userdata" then
            return tostring(v)
        end
        if type(v) == "vector2" then
            return "vector2.new" .. tostring(v)
        end
        return "'" .. tostring(v) .. "'"
    end

    local _vardump  -- local function _vardump
    local function _k(k, indent, nest)
        if type(k) == "string" then
            return k
        end
        if type(k) == "number" or type(k) == "boolean" then
            return string.format("[%s]", tostring(k))
        end
        if type(k) == "table" then
            if lookupTable[k] then
                local line = lookupTable[k]
                local rv = string.format("['*%s*']", tostring(line))
                if not indexed[line] then
                    result[line] = result[line] .. string.format(" --*%s*", line)
                    indexed[line] = line
                end
                return rv
            else
                local line = #result + 1
                local rv = string.format("['*%s*']", tostring(line))
                local kkey = string.format("['*%s*:']", tostring(line))
                _vardump(k, kkey, indent, nest)
                if nest == 1 then
                    result[#result] = result[#result] .. ","
                end
                --lookupTable[k] = line
                indexed[line] = line
                return rv
            end
        end
        return string.format("['%s']", tostring(k))
    end

    function _vardump(object, label, indent, nest, isArr)
        if (label == nil) then
            label = "var"
        end
        local postfix = ""
        if nest > 1 then
            postfix = ","
        end
        local reallabel = indent
        if not isArr then
            local key = _k(label, indent, nest)
            reallabel = string.format("%s%s = ", indent, key)
        end
        if type(object) ~= "table" then
            result[#result + 1] = string.format("%s%s%s", reallabel, _v(object), postfix)
        elseif lookupTable[object] then
            local line = lookupTable[object]
            result[#result + 1] = string.format("%s'*%s*'%s", reallabel, line, postfix)
            if not indexed[line] then
                result[line] = result[line] .. string.format(" --*%s*", line)
                indexed[line] = line
            end
        else
            local line = #result + 1
            lookupTable[object] = line

            if maxNest and nest + 1 > maxNest then
                result[line] = string.format("%s%s", reallabel, tostring(object))
            else
                result[line] = string.format("%s{", reallabel)

                local indent2 = indent .. "    "
                local keys = {}
                local values = {}
                for k, v in pairs(object) do
                    keys[#keys + 1] = k
                    values[k] = v
                end
                local isObjArr = #object == #keys
                table.sort(
                    keys,
                    function(a, b)
                        if type(a) == "number" and type(b) == "number" then
                            return a < b
                        else
                            return tostring(a) < tostring(b)
                        end
                    end
                )

                for i, k in ipairs(keys) do
                    _vardump(values[k], k, indent2, nest + 1, isObjArr)
                end
                result[#result + 1] = string.format("%s}%s", indent, postfix)
            end
        end
    end
    _vardump(object, label, "", 1)

    return result
end

--[[--

Creating a copy of an table with fully replicated properties.

**Usage:**

    -- Creating a reference of an table:
    local t1 = {a = 1, b = 2}
    local t2 = t1
    t2.b = 3    -- t1 = {a = 1, b = 3} <-- t1.b changed

    -- Createing a copy of an table:
    local t1 = {a = 1, b = 2}
    local t2 = clone(t1)
    t2.b = 3    -- t1 = {a = 1, b = 2} <-- t1.b no change


@param mixed object
@return mixed

]]
function exports.clone(object)
    local lookup_table = {}
    local function _copy(object)
        if type(object) ~= "table" then
            return object
        elseif lookup_table[object] then
            return lookup_table[object]
        end
        local new_table = {}
        lookup_table[object] = new_table
        for key, value in pairs(object) do
            new_table[_copy(key)] = _copy(value)
        end
        return setmetatable(new_table, getmetatable(object))
    end
    return _copy(object)
end

function exports.shallowClone(object)
    if type(object) ~= "table" then
        return object
    end
    local new_table = {}
    for key, value in pairs(object) do
        new_table[key] = value
    end
    return setmetatable(new_table, getmetatable(object))
end

--[[--

hecks if the given key or index exists in the table.

@param table arr
@param mixed key
@return boolean

]]
function exports.isset(arr, key)
    return type(arr) == "table" and arr[key] ~= nil
end

--[[--

Rounds a float.

@param number num
@return number(integer)

]]
function math.round(num)
    return math.floor(num + 0.5)
end

function math.roundWithMinStep(number, precision)
    local ret = math.round(number / precision) * precision
    return ret < precision and precision or ret
end

--[[--

Count all elements in an table.

@param table t
@return number(integer)

]]
function table.nums(t)
    local count = 0
    for k, v in pairs(t) do
        count = count + 1
    end
    return count
end

--[[--

Return all the keys or a subset of the keys of an table.

**Usage:**

    local t = {a = 1, b = 2, c = 3}
    local keys = table.keys(t)
    -- keys = {"a", "b", "c"}


@param table t
@return table

]]
function table.keys(t)
    local keys = {}
    for k, v in pairs(t) do
        keys[#keys + 1] = k
    end
    return keys
end

--[[--

Return all the values of an table.

**Usage:**

    local t = {a = "1", b = "2", c = "3"}
    local values = table.values(t)
    -- values = {1, 2, 3}


@param table t
@return table

]]
function table.values(t)
    local values = {}
    for k, v in pairs(t) do
        values[#values + 1] = v
    end
    return values
end

--[[--

Merge tables.

**Usage:**

    local dest = {a = 1, b = 2}
    local src  = {c = 3, d = 4}
    table.merge(dest, src)
    -- dest = {a = 1, b = 2, c = 3, d = 4}


@param table dest
@param table src

]]
function table.merge(dest, src)
    for k, v in pairs(src) do
        dest[k] = v
    end
end

--[[--

Merge arraies.

**Usage:**

    local dest = {1, 2}
    local src  = {3, 4}
    table.imerge(dest, src)
    -- dest = {1, 2, 3, 4}


@param table dest
@param table src

]]
function table.imerge(dest, src)
    for i, v in ipairs(src) do
        table.insert(dest, v)
    end
end

function table.any(t, predicate)
    for k, v in pairs(t) do
        if predicate(k, v) then
            return true
        end
    end
    return false
end

function table.iany(t, predicate)
    for i, v in ipairs(t) do
        if predicate(v) then
            return true
        end
    end
    return false
end

function table.clone(src)
    local unpack = unpack or table.unpack
    return {unpack(src)}
end

-- math.nan = 0/0
-- math.max_int32 = 0x7FFFFFFF
-- math.min_int32 = -math.max_int32 - 1
-- math.eps = 1e-6

-- function math.isNan(num)
--     return num ~= num
-- end

local mathmin = math.min
local mathmax = math.max

--[Comment]
--return a < b ? { min = a, max = b } : { min = b, max = a }
function math.range(a, b)
    return a < b and {min = a, max = b} or {min = b, max = a}
end

--[Comment]
--return x >= min and x <= max
function math.isInRange(x, min, max)
    return math.cmpf(x, min) >= 0 and math.cmpf(x, max) <= 0
end

--[Comment]
--return a random real number in range [min, max)
function math.randomInRange(min, max)
    return min + math.random() * (max - min)
end

-- return a random point (x, y) within a circle of radius 1
function math.randomInsideUnitCircle()
    local r = math.sqrt(math.random())
    local angle = math.random() * 2 * math.pi
    local x = r * math.cos(angle)
    local y = r * math.sin(angle)
    return x, y
end

function table.isEmpty(table)
    if table == nil then
        return true
    end
    if type(table) ~= "table" then
        return false
    end
    local empty = true
    for k, v in pairs(table) do
        empty = false
        break
    end
    return empty
end

function table.shuffleArray(t)
    for i = #t, 1, -1 do
        local j = math.random(i)
        t[i], t[j] = t[j], t[i]
    end
end

function table.multiply(table, multiplier)
    for k, v in pairs(table) do
        table[k] = v * multiplier
    end
end

function table.isArrayInclude(array, value)
    for i, v in ipairs(array) do
        if value == v then
            return true
        end
    end
    return false
end

function exports.ascendingPairs(tbl)
    local keys = {}
    local idx = 0
    for k, v in pairs(tbl) do --get all keys by ascending order
        if idx == 0 then
            keys[1] = k
        else
            local inserted = false
            for j = idx, 1, -1 do -- insertion sort
                if math.cmpf(keys[j], k) > 0 then
                    keys[j + 1] = keys[j]
                else
                    keys[j + 1] = k
                    inserted = true
                    break
                end
            end
            if not inserted then
                keys[1] = k
            end
        end
        idx = idx + 1
    end

    local i = 0
    return function()
        --define iterator
        i = i + 1
        if keys[i] then
            return keys[i], tbl[keys[i]]
        end
    end
end

--越靠近对方底线的球员排在前面
function exports.attackAthletesPairs(athletes, sign)
    local keys = {}
    local idx = 0
    for k, v in pairs(athletes) do --get all keys by ascending order
        if idx == 0 then
            keys[1] = k
        else
            local inserted = false
            for j = idx, 1, -1 do -- insertion sort
                if math.cmpf((athletes[keys[j]].position.y - v.position.y) * sign, 0) > 0 then
                    keys[j + 1] = keys[j]
                else
                    keys[j + 1] = k
                    inserted = true
                    break
                end
            end
            if not inserted then
                keys[1] = k
            end
        end
        idx = idx + 1
    end

    local i = 0
    return function()
        --define iterator
        i = i + 1
        if keys[i] then
            return keys[i], athletes[keys[i]]
        end
    end
end
