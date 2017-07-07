local vec3 = require("xmath.vec3")

local aabb = { }

local methods = { }

local meta = {
    __index = methods
}

function methods:center()
    return (self.max + self.min) * 0.5
end

function methods:extents()
    return (self.max - self.min) * 0.5
end

function aabb.new(min, max)
    local val = {
        min = min or vec3.zero(),
        max = max or vec3.zero()
    }
    setmetatable(val, meta)
    return val
end

return aabb