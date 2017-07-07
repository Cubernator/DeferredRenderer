Test = Behaviour:new()

function Test:update()
    if Input.getKeyPressed("l") then
        if self.light then
            self.light:setEnabled(not self.light:isEnabled())
        else
            print("light not found!")
        end
    end
end