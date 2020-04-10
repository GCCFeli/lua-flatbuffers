--[[ Generated with https://github.com/TypeScriptToLua/TypeScriptToLua ]]
require("lualib_bundle");
__TS__SourceMapTraceBack(debug.getinfo(1).short_src, {["5"] = 9,["6"] = 9,["7"] = 9,["9"] = 13,["10"] = 18,["11"] = 23,["12"] = 28,["13"] = 33,["14"] = 38,["15"] = 43,["16"] = 9,["17"] = 58,["18"] = 58,["19"] = 58,["21"] = 62,["22"] = 67,["23"] = 72,["24"] = 77,["25"] = 58,["26"] = 92,["27"] = 92,["28"] = 92,["30"] = 96,["31"] = 101,["32"] = 106,["33"] = 111,["34"] = 116,["35"] = 92,["36"] = 131,["37"] = 131,["38"] = 131,["40"] = 132,["41"] = 133,["42"] = 134,["43"] = 131,["44"] = 149,["45"] = 149,["46"] = 149,["48"] = 150,["49"] = 151,["50"] = 152,["51"] = 153,["52"] = 154,["53"] = 155,["54"] = 156,["55"] = 157,["56"] = 158,["57"] = 159,["58"] = 160,["59"] = 161,["60"] = 162,["61"] = 163,["62"] = 164,["63"] = 165,["64"] = 166,["65"] = 167,["66"] = 168,["67"] = 169,["68"] = 170,["69"] = 172,["70"] = 173,["71"] = 174,["72"] = 175,["73"] = 180,["74"] = 185,["75"] = 190,["76"] = 191,["77"] = 192,["78"] = 197,["79"] = 202,["80"] = 203,["81"] = 149,["82"] = 205,["83"] = 206,["84"] = 207,["85"] = 208,["86"] = 209,["87"] = 210,["88"] = 211,["89"] = 212,["90"] = 213,["91"] = 214,["92"] = 205,["93"] = 230,["94"] = 230,["95"] = 230,["97"] = 231,["98"] = 232,["99"] = 233,["100"] = 234,["101"] = 235,["102"] = 236,["103"] = 237,["104"] = 238,["105"] = 239,["106"] = 240,["107"] = 241,["108"] = 246,["109"] = 251,["110"] = 252,["111"] = 230,["112"] = 254,["113"] = 255,["114"] = 256,["115"] = 257,["116"] = 254,["117"] = 273,["118"] = 273,["119"] = 273,["121"] = 274,["122"] = 275,["123"] = 276,["124"] = 277,["125"] = 278,["126"] = 279,["127"] = 280,["128"] = 281,["129"] = 282,["130"] = 283,["131"] = 284,["132"] = 285,["133"] = 286,["134"] = 291,["135"] = 296,["136"] = 301,["137"] = 306,["138"] = 311,["139"] = 316,["140"] = 321,["141"] = 326,["142"] = 331,["143"] = 336,["144"] = 337,["145"] = 338,["146"] = 343,["147"] = 273,["148"] = 358,["149"] = 358,["150"] = 358,["152"] = 359,["153"] = 360,["154"] = 361,["155"] = 362,["156"] = 363,["157"] = 364,["158"] = 365,["159"] = 366,["160"] = 367,["161"] = 368,["162"] = 369,["163"] = 370,["164"] = 371,["165"] = 372,["166"] = 373,["167"] = 374,["168"] = 358,["169"] = 389,["170"] = 389,["171"] = 389,["173"] = 390,["174"] = 391,["175"] = 392,["176"] = 389,["177"] = 407,["178"] = 407,["179"] = 407,["181"] = 408,["182"] = 409,["183"] = 410,["184"] = 411,["185"] = 412,["186"] = 407});
local ____exports = {}
____exports.MarkedDataT = __TS__Class()
local MarkedDataT = ____exports.MarkedDataT
MarkedDataT.name = "MarkedDataT"
function MarkedDataT.prototype.____constructor(self)
    self.markedByAthleteEntity = -1
    self.skillMoveStartDirection = vector2.zero
    self.prevExpectedPosition = vector2.zero
    self.nextExpectedPosition = vector2.zero
    self.expectedPosition = vector2.zero
    self.expectedTotalRemainingTime = 0
    self.animationPhase = -1
end
____exports.MarkDataT = __TS__Class()
local MarkDataT = ____exports.MarkDataT
MarkDataT.name = "MarkDataT"
function MarkDataT.prototype.____constructor(self)
    self.targetAthleteEntity = -1
    self.targetPosition = vector2.zero
    self.strategy = 0
    self.isNotToMakeDecision = false
end
____exports.CatchDataT = __TS__Class()
local CatchDataT = ____exports.CatchDataT
CatchDataT.name = "CatchDataT"
function CatchDataT.prototype.____constructor(self)
    self.moveTargetPosition = Vector3.zero
    self.moveTargetTime = 0
    self.moveTargetDirection = 0
    self.catchMovePriority = 0
    self.isCatchAnimationChosen = false
end
____exports.HeroMomentDataT = __TS__Class()
local HeroMomentDataT = ____exports.HeroMomentDataT
HeroMomentDataT.name = "HeroMomentDataT"
function HeroMomentDataT.prototype.____constructor(self)
    self.enterTime = 0
    self.exitTime = 0
    self.athleteEntity = -1
end
____exports.AthleteDataT = __TS__Class()
local AthleteDataT = ____exports.AthleteDataT
AthleteDataT.name = "AthleteDataT"
function AthleteDataT.prototype.____constructor(self)
    self.position = vector2.zero
    self.bodyDirection = vector2.zero
    self.velocity = vector2.zero
    self.averageVelocity = vector2.zero
    self.rotationVelocity = 0
    self.averageRotationVelocity = 0
    self.acceleration = 0
    self.averageAcceleration = 0
    self.entity = 0
    self.teamEntity = 0
    self.maxSpeed = 10
    self.hasBall = false
    self.lastHasBall = false
    self.predictedPosition = vector2.zero
    self.targetPositionX = 0
    self.targetPositionY = 0
    self.index = 0
    self.characterId = 0
    self.roleId = 0
    self.tactics = __TS__New(____exports.TacticsConfigT)
    self.isAtDefenseTargetPosition = false
    self.defenseMentality = 0
    self.defenseStandLine = 0
    self.marked = __TS__New(____exports.MarkedDataT)
    self.mark = __TS__New(____exports.MarkDataT)
    self.needApplyUserInput = false
    self.preferredFoot = 1
    self.isBeforeDetach = false
    self.currentScenarioData = nil
    self.tackleType = 0
    self.tackleTargetAthleteEntity = -1
    self.tackleTargetConfrontDecision = 0
    self.catch = nil
end
function AthleteDataT.prototype.ResetPartFields(self)
    self.marked = __TS__New(____exports.MarkedDataT)
    self.mark = __TS__New(____exports.MarkDataT)
    self.needApplyUserInput = false
    self.isBeforeDetach = false
    self.currentScenarioData = nil
    self.tackleType = 0
    self.tackleTargetAthleteEntity = -1
    self.tackleTargetConfrontDecision = 0
    self.catch = nil
end
____exports.TeamDataT = __TS__Class()
local TeamDataT = ____exports.TeamDataT
TeamDataT.name = "TeamDataT"
function TeamDataT.prototype.____constructor(self)
    self.entity = 0
    self.formationId = 0
    self.defenseLinePosition = 0
    self.defenseStrategy = 0
    self.attackMentality = 0
    self.athleteEntities = {}
    self.role = 0
    self.field = -1
    self.side = 0
    self.score = 0
    self.otherTeamEntity = -1
    self.markAthleteEntity = -1
    self.isInBallRivalry = false
    self.heroMoment = __TS__New(____exports.HeroMomentDataT)
end
function TeamDataT.prototype.ResetPartFields(self)
    self.markAthleteEntity = -1
    self.isInBallRivalry = false
    self.heroMoment = __TS__New(____exports.HeroMomentDataT)
end
____exports.MatchDataT = __TS__Class()
local MatchDataT = ____exports.MatchDataT
MatchDataT.name = "MatchDataT"
function MatchDataT.prototype.____constructor(self)
    self.formationId = 0
    self.attackTeamEntity = -1
    self.defenceTeamEntity = -1
    self.withBallAthleteEntity = -1
    self.ballEntity = -1
    self.matchEvent = 0
    self.deadBallCountdownTime = 0
    self.yAtkDCLine = 0
    self.yDefDCLine = 0
    self.AtkToDefDistance = 0
    self.yDefFCLine = 0
    self.yBallAtkLerp = 0
    self.yBallDefLerp = 0
    self.inMatch = false
    self.matchState = 1
    self.matchStage = -1
    self.matchTime = 0
    self.matchMinutes = 0
    self.hasExtraTimeStage = false
    self.hasPenaltyShootoutStage = false
    self.isSpecialStart = false
    self.stoppageMinutes = {}
    self.stageDeltaMinutes = {}
    self.ballPosition = vector2.zero
    self.ballVelocity = vector2.zero
    self.expectedBallPosition = vector2.zero
end
____exports.TacticsConfigT = __TS__Class()
local TacticsConfigT = ____exports.TacticsConfigT
TacticsConfigT.name = "TacticsConfigT"
function TacticsConfigT.prototype.____constructor(self)
    self.xAtkDefault = 0
    self.xAtkOffset = 0
    self.yAtkShrinkLine = 0
    self.xAtkShrinkCoefficient = 0
    self.xAtkCompress = 0
    self.formationPercentageAtkMin = 0
    self.formationPercentageAtkMax = 0
    self.atkMoveFurtherUp = 0
    self.xDefDefault = 0
    self.xDefOffset = 0
    self.yDefShrinkLine = 0
    self.xDefShrinkCoefficient = 0
    self.xDefCompress = 0
    self.formationPercentageDefMin = 0
    self.formationPercentageDefMax = 0
    self.defMoveFurtherUp = 0
end
____exports.ScriptRecordT = __TS__Class()
local ScriptRecordT = ____exports.ScriptRecordT
ScriptRecordT.name = "ScriptRecordT"
function ScriptRecordT.prototype.____constructor(self)
    self.athleteData = {}
    self.teamData = {}
    self.matchData = nil
end
____exports.ScenarioDataT = __TS__Class()
local ScenarioDataT = ____exports.ScenarioDataT
ScenarioDataT.name = "ScenarioDataT"
function ScenarioDataT.prototype.____constructor(self)
    self.name = 0
    self.phase = 0
    self.enterPosition = vector2.zero
    self.skillMoveDisplacement = vector2.zero
    self.hasPerformedSkillMove = false
end
return ____exports
