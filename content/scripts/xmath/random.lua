local vec3 = require("xmath.vec3")

local random = { }

function random.uniform_float(min, max)
    return min + math.random() * (max - min)
end

function random.uniform_vec3(min, max)
    return vec3.new(
        random.uniform_float(min[1], max[1]),
        random.uniform_float(min[2], max[2]),
        random.uniform_float(min[3], max[3])
    )
end

function random.uniform_dir()
    return vec3.normalized(random.uniform_vec3(-vec3.one(), vec3.one()))
end

return random