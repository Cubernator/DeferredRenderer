local vec4 = require("xmath.vec4")

local color = { }

function color.new(r, g, b, a)
    return vec4.new(r or 0, g or 0, b or 0, a or 1)
end

function color.fromHSB(hue, saturation, brightness, alpha)
    local s = saturation or 1
    local v = brightness or 1
    local a = alpha or 1
    if s == 0 then
        return color.new(v, v, v, a)
    else
        local sector = hue // 60
        local frac = (hue / 60) - sector
        local o = v * (1 - s)
        local p = v * (1 - s * frac)
        local q = v * (1 - s * (1 - frac))

        if sector == 0 then
            return color.new(v, q, o, a)
        elseif sector == 1 then
            return color.new(p, v, o, a)
        elseif sector == 2 then
            return color.new(o, v, q, a)
        elseif sector == 3 then
            return color.new(o, p, v, a)
        elseif sector == 4 then
            return color.new(q, o, v, a)
        elseif sector == 5 then
            return color.new(v, o, p, a)
        end
    end
end

function color.fromHex(hexString)
    -- TODO
end

function color.transparent()
    return color.new(0, 0, 0, 0)
end

function color.black()
    return color.new(0, 0, 0)
end

function color.white()
    return color.new(1, 1, 1)
end

function color.red()
    return color.new(1, 0, 0)
end

function color.green()
    return color.new(0, 1, 0)
end

function color.blue()
    return color.new(0, 0, 1)
end

function color.yellow()
    return color.new(1, 1, 0)
end

function color.cyan()
    return color.new(0, 1, 1)
end

function color.magenta()
    return color.new(1, 0, 1)
end

return color