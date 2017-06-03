require("Behaviour")

RandomMovement = ScriptBehaviour:new{
    minSpeed = 0.1,
    maxSpeed = 20.0
}

function RandomMovement:start()
    self.transform = self.getEntity().getTransform()
end

function RandomMovement:update() end