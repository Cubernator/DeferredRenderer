local aabb = require("xmath.aabb")
local color = require("xmath.color")
local random = require("xmath.random")
local vec3 = require("xmath.vec3")

LightingTest = Behaviour:new{
    lightCount = 0,
    lightArea = aabb.new(),
    minSaturation = 0,
    maxSaturation = 1,
    minIntensity = 0,
    maxIntensity = 1,
    rangeFactor = 1,
    minSpeed = 0,
    maxSpeed = 1,
    testDuration = 1,
}

function LightingTest:init()
    self.lightEnabled = false
    self.lightsMoving = false
    self.testing = false
    self.lights = { }

    self:addLights(self.lightCount)
end

function LightingTest:update()
    if self.testing then
        local ft = Engine.frameTime()
        self.accFrameTime = self.accFrameTime + ft
        self.testSamples = self.testSamples + 1

        local time = Engine.time()

        if time >= self.testTimer then
            local avgFrameTime = self.accFrameTime / self.testSamples
            local avgFPS = self.testSamples / (time - (self.testTimer - self.testDuration))

            table.insert(self.ftResults, avgFrameTime)
            table.insert(self.fpsResults, avgFPS)
            table.insert(self.lightsPerObj, Graphics.avgLightsPerObj())

            print(string.format("%u lights: %f ms (%f FPS)", self.testLight, avgFrameTime, avgFPS))

            self.testSamples = 0
            self.accFrameTime = 0
            if self.testLights then
                if self.testLight <= #self.lights then
                    self.lights[self.testLight]:setActive(true)
                    self.testTimer = time + self.testDuration
                    self.testLight = self.testLight + 1
                else
                    self.testing = false
                end
            else
                self.testing = false
            end

            if not self.testing then
                print("done!")
            end
        end
    end

    if Input.getKeyPressed("f1") then
        self:startTest(false)
    end

    if Input.getKeyPressed("f2") then
        self:startTest(true)
    end

    if Input.getKeyPressed("enter") then
        self:saveResults()
    end

    if Input.getKeyPressed("comma") then
        self:addLights(1)
    end

    if Input.getKeyPressed("period") then
        self:addLights(10)
    end

    if Input.getKeyPressed("m") then
        self:setLightsEnabled(not self.lightsEnabled)
    end

    if Input.getKeyPressed("h") then
        self:setLightsMoving(not self.lightsMoving)
    end
end

function LightingTest:addLights(count)
    for i = 1, count do
        local entity = Entity.create("light_test")
        entity:setActive(self.lightsEnabled)

        local trans = entity:transform()
        trans:setPosition(random.uniform_vec3(self.lightArea.min, self.lightArea.max))

        local light = entity:addComponent("Light")
        light:setType("point")

        -- generate color from random hue and saturation (but always full brightness)
        local c = color.fromHSB(
            random.uniform_float(0, 360),
            random.uniform_float(self.minSaturation, self.maxSaturation),
            1
        )        
        light:setColor(c)

        local intensity = random.uniform_float(self.minIntensity, self.maxIntensity)
        light:setIntensity(intensity)

        local range = math.sqrt(intensity) * self.rangeFactor
        light:setRange(range)

        local bib = entity:addBehaviour("BounceInBox")
        bib:setEnabled(self.lightsMoving)
        bib.area = self.lightArea
        -- TODO: use actual uniform direction
        bib.velocity = random.uniform_dir() * random.uniform_float(self.minSpeed, self.maxSpeed)

        table.insert(self.lights, entity)
    end

    print(string.format("added %i lights, %i total", count, #self.lights))
end

function LightingTest:setLightsEnabled(val)
    if val ~= self.lightsEnabled then
        self.lightsEnabled = val
        for i, e in ipairs(self.lights) do
            e:setActive(val)
        end
    end
end

function LightingTest:setLightsMoving(val)
    if val ~= self.lightsMoving then
        self.lightsMoving = val
        for i, e in ipairs(self.lights) do
            e:getBehaviour("BounceInBox"):setEnabled(val)
        end
    end
end

function LightingTest:startTest(lights)
    self.testing = true

    self.testTimer = Engine.time() + self.testDuration
    self.testLights = lights
    self.testLight = 1
    self.testSamples = 0
    self.accFrameTime = 0

    if lights then
        self:setLightsEnabled(false)
    end

    self.ftResults = { }
    self.fpsResults = { }
    self.lightsPerObj = { }
end

function LightingTest:saveResults()
    do
        local file = io.open("test.dat", "w")
        if file then
            file:write("lights frameTime fps")
            for i, res in ipairs(self.ftResults) do
                file:write(string.format("%u %f %f", i, res, self.fpsResults[i]))
            end
            file:close()
        end
    end
    do
        local file = io.open("test2.dat", "w")
        if file then
            file:write("lights lpo")
            for i, res in ipairs(self.lightsPerObj) do
                file:write(string.format("%u %f", i, res))
            end
            file:close()
        end
    end
end
