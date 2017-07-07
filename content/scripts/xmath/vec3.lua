local struct = require("xmath.struct")

local vec3 = {}

local fields = {
    x = 1,
    y = 2,
    z = 3
}

local meta = {
    __index = struct.getIndexMM(fields),
    __newindex = struct.getNewIndexMM(fields)
}

function meta.__add(lhs, rhs)
    return vec3.new(
        lhs[1] + rhs[1],
        lhs[2] + rhs[2],
        lhs[3] + rhs[3]
    )
end

function meta.__sub(lhs, rhs)
    return vec3.new(
        lhs[1] - rhs[1],
        lhs[2] - rhs[2],
        lhs[3] - rhs[3]
    )
end

function meta.__mul(lhs, rhs)
    return vec3.new(
        lhs[1] * rhs,
        lhs[2] * rhs,
        lhs[3] * rhs
    )
end

function meta.__div(lhs, rhs)
    return vec3.new(
        lhs[1] / rhs,
        lhs[2] / rhs,
        lhs[3] / rhs
    )
end

function meta.__unm(val)
    return vec3.new(
        -val[1],
        -val[2],
        -val[3]
    )
end

function meta.__len(val)
    return 3
end

function meta.__eq(lhs, rhs)
    return (lhs[1] == rhs[1]) and (lhs[2] == rhs[2]) and (lhs[3] == rhs[3])
end

function meta.__copy(val)
    return vec3.copy(val)
end

function vec3.new(x, y, z)
    local v = { x or 0, y or 0, z or 0 }
    setmetatable(v, meta)
    return v
end

function vec3.copy(val)
    return vec3.new(val[1], val[2], val[3])
end

function vec3.length2(val)
    return vec3.dot(val, val)
end

function vec3.length(val)
    return math.sqrt(vec3.length2(val))
end

function vec3.normalized(val)
    return val * (1 / vec3.length(val))
end

function vec3.dot(lhs, rhs)
    return lhs[1] * rhs[1] + lhs[2] * rhs[2] + lhs[3] * rhs[3]
end

function vec3.cross(lhs, rhs)
    return vec3.new(
        lhs[2] * rhs[3] - rhs[2] * lhs[3],
		lhs[3] * rhs[1] - rhs[3] * lhs[1],
		lhs[1] * rhs[2] - rhs[1] * lhs[2]
    )
end

function vec3.zero()
    return vec3.new(0, 0, 0)
end

function vec3.one()
    return vec3.new(1, 1, 1)
end

function vec3.right()
    return vec3.new(1, 0, 0)
end

function vec3.up()
    return vec3.new(0, 1, 0)
end

function vec3.forward()
    return vec3.new(0, 0, 1)
end


return vec3