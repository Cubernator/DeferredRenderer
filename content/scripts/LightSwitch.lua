LightSwitch = Behaviour:new{
    key = -1
}

function LightSwitch:init()
    self.light = self:entity():getComponent("Light")
end

function LightSwitch:update()
    if self.light and Input.getKeyPressed(self.key) then
        self.light:setEnabled(not self.light:isEnabled())
    end
end