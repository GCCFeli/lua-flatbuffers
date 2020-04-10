--[[ Generated with https://github.com/TypeScriptToLua/TypeScriptToLua ]]
require("lualib_bundle");
__TS__SourceMapTraceBack(debug.getinfo(1).short_src, {["5"] = 8,["6"] = 8,["7"] = 8,["9"] = 9,["10"] = 8,["11"] = 24,["12"] = 24,["13"] = 24,["15"] = 25,["16"] = 26,["17"] = 27,["18"] = 24,["19"] = 42,["20"] = 42,["21"] = 42,["23"] = 43,["24"] = 44,["25"] = 42,["26"] = 59,["27"] = 59,["28"] = 59,["30"] = 60,["31"] = 59,["32"] = 75,["33"] = 75,["34"] = 75,["36"] = 75,["37"] = 90,["38"] = 90,["39"] = 90,["41"] = 91,["42"] = 92,["43"] = 93,["44"] = 94,["45"] = 90,["46"] = 109,["47"] = 109,["48"] = 109,["50"] = 110,["51"] = 111,["52"] = 112,["53"] = 109});
local ____exports = {}
____exports.ManualDribbleT = __TS__Class()
local ManualDribbleT = ____exports.ManualDribbleT
ManualDribbleT.name = "ManualDribbleT"
function ManualDribbleT.prototype.____constructor(self)
    self.direction = 0
end
____exports.HeroMomentActionT = __TS__Class()
local HeroMomentActionT = ____exports.HeroMomentActionT
HeroMomentActionT.name = "HeroMomentActionT"
function HeroMomentActionT.prototype.____constructor(self)
    self.type = 0
    self.triggerEvent = 0
    self.dribble = nil
end
____exports.ActionT = __TS__Class()
local ActionT = ____exports.ActionT
ActionT.name = "ActionT"
function ActionT.prototype.____constructor(self)
    self.type = 0
    self.heroMomentAction = nil
end
____exports.AthleteSampleDataT = __TS__Class()
local AthleteSampleDataT = ____exports.AthleteSampleDataT
AthleteSampleDataT.name = "AthleteSampleDataT"
function AthleteSampleDataT.prototype.____constructor(self)
    self.action = __TS__New(____exports.ActionT)
end
____exports.TeamSampleDataT = __TS__Class()
local TeamSampleDataT = ____exports.TeamSampleDataT
TeamSampleDataT.name = "TeamSampleDataT"
function TeamSampleDataT.prototype.____constructor(self)
end
____exports.MatchSampleDataT = __TS__Class()
local MatchSampleDataT = ____exports.MatchSampleDataT
MatchSampleDataT.name = "MatchSampleDataT"
function MatchSampleDataT.prototype.____constructor(self)
    self.withBallAthleteId = -1
    self.matchMinutes = 0
    self.matchEvent = 0
    self.isReset = false
end
____exports.ScriptSampleDataT = __TS__Class()
local ScriptSampleDataT = ____exports.ScriptSampleDataT
ScriptSampleDataT.name = "ScriptSampleDataT"
function ScriptSampleDataT.prototype.____constructor(self)
    self.frame = 0
    self.athletes = {}
    self.teams = {}
end
return ____exports
