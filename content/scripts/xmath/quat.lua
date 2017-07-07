local struct = require("xmath.struct")
local vec3 = require("xmath.vec3")

local quat = {}

local fields = {
    x = 1,
    y = 2,
    z = 3,
    w = 4
}

local meta = {
    __index = struct.getIndexMM(fields),
    __newindex = struct.getNewIndexMM(fields)
}

function meta.__mul(lhs, rhs)
    return quat.new(
        lhs[4] * rhs[1] + lhs[1] * rhs[4] + lhs[2] * rhs[3] - lhs[3] * rhs[2],
        lhs[4] * rhs[2] + lhs[2] * rhs[4] + lhs[3] * rhs[1] - lhs[1] * rhs[3],
        lhs[4] * rhs[3] + lhs[3] * rhs[4] + lhs[1] * rhs[2] - lhs[2] * rhs[1],
        lhs[4] * rhs[4] - lhs[1] * rhs[1] - lhs[2] * rhs[2] - lhs[3] * rhs[3]
    )
end

function quat.new(x, y, z, w)
    local q = { x, y, z, w }
    setmetatable(q, meta)
    return q
end

function quat.fromAngleAxis(angle, axis)
    local s = math.sin(angle * 0.5)
    return quat.new(
        axis[1] * s,
        axis[2] * s,
        axis[3] * s,
        math.cos(angle * 0.5)
    )
end

function quat.fromEulerAngles(pitch, yaw, roll)
    -- TODO
end

function quat.rotate(q, v)
    local qv = vec3.new(q[1], q[2], q[3])
    local uv = vec3.cross(qv, v)
    local uuv = vec3.cross(qv, uv)
    return v + ((uv * q[4]) + uuv) * 2
end

return quat