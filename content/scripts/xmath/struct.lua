local struct = {}

function struct.getIndexMM(fields)
    return function (vec, key)
        return rawget(vec, fields[key])
    end
end

function struct.getNewIndexMM(fields)
    return function (vec, key)
        return rawset(vec, fields[key], val)
    end
end

return struct