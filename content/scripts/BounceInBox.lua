local aabb = require("xmath.aabb")
local vec3 = require("xmath.vec3")

BounceInBox = Behaviour:new{
    area = aabb.new(),
    velocity = vec3.zero()
}

function BounceInBox:init()
    self.transform = self:entity():transform()
end

function BounceInBox:update()
    local pos = self.transform:position()

    pos = pos + (self.velocity * Engine.frameTime())

    for i = 1, 3 do
        if (pos[i] < self.area.min[i]) or (pos[i] > self.area.max[i]) then
            self.velocity[i] = -self.velocity[i]
        end
    end

    self.transform:setPosition(pos)
end