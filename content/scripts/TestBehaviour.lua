require("Behaviour")

TestBehaviour = Behaviour:new()

function TestBehaviour:start()
    local e = self:getentity()
    local name = e:getname()
    print(name)
end