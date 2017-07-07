local vec3 = require("xmath.vec3")
local quat = require("xmath.quat")

FlyCamera = Behaviour:new{
    normalSpeed = 1,
    fastSpeed = 5,
    sensitivityX = 1,
    sensitivityY = 1,
    angleX = 0,
    angleY = 0
}

function FlyCamera:init()
    self.transform = self:entity():transform()
    self.angleX = math.rad(self.angleX)
    self.angleY = math.rad(self.angleY)

    self:updateRotation()
end

function FlyCamera:update()
    local locked = Input.isCursorLocked()
    if Input.getMouseButtonPressed("left") then
        locked = not locked
        Input.setCursorLocked(locked)
    end

    if locked then
        local delta = Input.cursorDelta()

        self.angleX = self.angleX - delta.y * self.sensitivityY * 0.017
        self.angleY = self.angleY - delta.x * self.sensitivityX * 0.017

        local half_pi = math.pi * 0.5
        self.angleX = math.min(math.max(self.angleX, -half_pi), half_pi)
        self.angleY = math.fmod(self.angleY, math.pi * 2)

        local rot = self:updateRotation()

        local x = 0
        if Input.getKey("a") then x = x - 1 end
        if Input.getKey("d") then x = x + 1 end

        local z = 0
        if Input.getKey("w") then z = z - 1 end
        if Input.getKey("s") then z = z + 1 end

        local speed = Input.getKey("leftShift") and self.fastSpeed or self.normalSpeed
        local dir = quat.rotate(rot, vec3.new(x, 0, z))

        local pos = self.transform:position()
        pos = pos + dir * speed * Engine.frameTime()
        self.transform:setPosition(pos)
    end
end

function FlyCamera:updateRotation()
    local rotX = quat.fromAngleAxis(self.angleX, vec3.right())
    local rotY = quat.fromAngleAxis(self.angleY, vec3.up())
    local rot = rotY * rotX
    self.transform:setRotation(rot)
    return rot
end