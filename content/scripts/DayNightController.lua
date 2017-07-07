local color = require("xmath.color")

DayNightController = Behaviour:new{
    dayTime = true,
    ambient = true,
    daySkyColor = color.black(),
    nightSkyColor = color.black(),
    dayLightColor = color.black(),
    nightLightColor = color.black(),
    dayLightIntensity = 0,
    nightLightIntensity = 0
}

function DayNightController:init()
    self.light = self:entity():getComponent("Light")
    self:applyValues()
end

function DayNightController:update()
    if Input.getKeyPressed("n") then
        self:setDayTime(not self.dayTime)
    end

    if Input.getKeyPressed("b") then
        self:setAmbient(not self.ambient)
    end
end

function DayNightController:setDayTime(val)
    if val ~= self.dayTime then
        self.dayTime = val
        self:applyValues()
    end
end

function DayNightController:setAmbient(val)
    if val ~= self.ambient then
        self.ambient = val
        self:applyValues()
    end
end

function DayNightController:applyValues()
    local scene = self:entity():parentScene()

    if scene then
        local skyColor = self.ambient and (self.dayTime and self.daySkyColor or self.nightSkyColor) or color.black()
        scene:setBackColor(skyColor)
        scene:setAmbientLight(skyColor)
    end

    if self.light then
        self.light:setColor(self.dayTime and self.dayLightColor or self.nightLightColor)
        self.light:setIntensity(self.dayTime and self.dayLightIntensity or self.nightLightIntensity)
    end
end