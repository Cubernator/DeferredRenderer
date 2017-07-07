local struct = require("xmath.struct")

local vec2 = {}

local fields = {
    x = 1,
    y = 2
}

local meta = {
    __index = struct.getIndexMM(fields),
    __newindex = struct.getNewIndexMM(fields)
}

function meta.__add(lhs, rhs)
    return vec2.new(
        lhs[1] + rhs[1],
        lhs[2] + rhs[2]
    )
end

function meta.__sub(lhs, rhs)
    return vec2.new(
        lhs[1] - rhs[1],
        lhs[2] - rhs[2]
    )
end

function meta.__mul(lhs, rhs)
    return vec2.new(
        lhs[1] * rhs,
        lhs[2] * rhs
    )
end

function meta.__div(lhs, rhs)
    return vec2.new(
        lhs[1] / rhs,
        lhs[2] / rhs
    )
end

function meta.__unm(val)
    return vec2.new(
        -val[1],
        -val[2]
    )
end

function meta.__len(val)
    return 2
end

function meta.__eq(lhs, rhs)
    return (lhs[1] == rhs[1]) and (lhs[2] == rhs[2])
end

function meta.__copy(val)
    return vec2.copy(val)
end

function vec2.new(x, y)
    local v = { x or 0, y or 0 }
    setmetatable(v, meta)
    return v
end

function vec2.copy(val)
    return vec2.new(val[1], val[2])
end

function vec2.length2(val)
    return vec2.dot(val, val)
end

function vec2.length(val)
    return math.sqrt(vec2.length2(val))
end

function vec2.normalized(val)
    return val * (1 / vec2.length(val))
end

function vec2.dot(lhs, rhs)
    return lhs[1] * rhs[1] + lhs[2] * rhs[2]
end

function vec2.zero()
    return vec2.new(0, 0)
end

function vec2.one()
    return vec2.new(1, 1)
end

function vec2.right()
    return vec2.new(1, 0)
end

function vec2.up()
    return vec2.new(0, 1)
end

return vec2