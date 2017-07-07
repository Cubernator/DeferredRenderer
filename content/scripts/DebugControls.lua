DebugControls = Behaviour:new{
    imgEffectsEnabled = true
}

function DebugControls:init()
    local cam = Camera.main()
    if cam then
        self.imgEffects = cam:entity():getComponents("ImageEffect")
    end
end

function DebugControls:update()
    if Input.getKeyPressed("escape") then
        Engine.stop()
    end

    if Input.getKeyPressed("t") then
        print("Triangle count: "..Graphics.triangleCount())
    end

    if Input.getKeyPressed("p") then
        local d = not Graphics.isDeferredEnabled()
        Graphics.setDeferredEnabled(d)
        print("Rendering path: "..(d and "Deferred" or "Forward"))
    end

    if Input.getKeyPressed("o") then
        local vfc = not Graphics.isVFCEnabled()
        Graphics.setVFCEnabled(vfc)
        print("View frustum culling "..getEnabledStatus(vfc))
    end

    if Input.getKeyPressed("g") then
        local om = Graphics.outputMode()
        -- TODO: expose enums to lua
        om = (om + 1) % 6
        Graphics.setOutputMode(om)
        print("Debug output: "..getOutputName(om))
    end

    if Input.getKeyPressed("i") then
        self:setImgEffectsEnabled(not self.imgEffectsEnabled)
        print("Image Effects "..getEnabledStatus(self.imgEffectsEnabled))
    end

    if Input.getKey("u") then
        io.write("\rFPS: "..Engine.fps())
    end

    if Input.getKeyPressed("r") then
        Engine.loadFirstScene()
    end

    if Input.getKeyPressed("v") then
        local vs = not Engine.vSync()
        Engine.setVSync(vs)
        print("VSync "..getEnabledStatus(vs))
    end
end

function DebugControls:setImgEffectsEnabled(val)
    if (val ~= self.imgEffectsEnabled) then
        self.imgEffectsEnabled = val

        for i, e in ipairs(self.imgEffects) do
            e:setEnabled(self.imgEffectsEnabled)
        end
    end
end

omNames = { "Default", "Diffuse Color", "Specular Color", "Smoothness", "Normals", "Depth" }

function getOutputName(mode)
    return omNames[mode+1]
end

function getEnabledStatus(enabled)
    return enabled and "Enabled" or "Disabled"
end