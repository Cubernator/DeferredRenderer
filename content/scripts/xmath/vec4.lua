local struct = require("xmath.struct")

local vec4 = {}

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

function meta.__add(lhs, rhs)
    return vec4.new(
        lhs[1] + rhs[1],
        lhs[2] + rhs[2],
        lhs[3] + rhs[3],
        lhs[4] + rhs[4]
    )
end

function meta.__sub(lhs, rhs)
    return vec4.new(
        lhs[1] - rhs[1],
        lhs[2] - rhs[2],
        lhs[3] - rhs[3],
        lhs[4] - rhs[4]
    )
end

function meta.__mul(lhs, rhs)
    return vec4.new(
        lhs[1] * rhs,
        lhs[2] * rhs,
        lhs[3] * rhs,
        lhs[4] * rhs
    )
end

function meta.__div(lhs, rhs)
    return vec4.new(
        lhs[1] / rhs,
        lhs[2] / rhs,
        lhs[3] / rhs,
        lhs[4] / rhs
    )
end

function meta.__unm(val)
    return vec4.new(
        -val[1],
        -val[2],
        -val[3],
        -val[4]
    )
end

function meta.__len(val)
    return 4
end

function meta.__eq(lhs, rhs)
    return (lhs[1] == rhs[1]) and (lhs[2] == rhs[2]) and (lhs[3] == rhs[3]) and (lhs[4] == rhs[4])
end

function meta.__copy(val)
    return vec4.copy(val)
end

function vec4.new(x, y, z, w)
    local v = { x or 0, y or 0, z or 0, w or 0 }
    setmetatable(v, meta)
    return v
end

function vec4.copy(val)
    return vec4.new(val[1], val[2], val[3], val[4])
end

function vec4.point(x, y, z)
    return vec4.new(x, y, z, 1)
end

function vec4.dir(x, y, z)
    return vec4.new(x, y, z, 0)
end

return vec4