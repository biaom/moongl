#!/usr/bin/env lua
package.path = package.path..";../?.lua"
local glfw = require("moonglfw")
local gl = require("moongl")
local glmath = require("moonglmath")
local new_torus = require("common.torus")

local vec3, vec4 = glmath.vec3, glmath.vec4
local mat3, mat4 = glmath.mat3, glmath.mat4
local pi, rad = math.pi, math.rad

local TITLE = "Chapter 3 - Phong reflection model (per-vertex)"
local W, H = 500, 500

-- GLFW/GL initializations
glfw.version_hint(4, 6, 'core')
glfw.window_hint('opengl forward compat', true)
local window = glfw.create_window(W, H, TITLE)
glfw.make_context_current(window)
gl.init()

local angle, speed = 0, pi/3 -- rad, rad/s
local animate = false

glfw.set_key_callback(window, function(window, key, scancode, action)
   if key == 'escape' and action == 'press' then
      glfw.set_window_should_close(window, true)
   elseif key == 'space' and action == 'press' then
      animate = not animate
   end
end)

local projection
local function resize(window, w, h)
   W, H = w, h
   gl.viewport(0, 0, w, h)
   projection = glmath.perspective(rad(70.0), w/h, 0.3, 100.0)
end

glfw.set_window_size_callback(window, resize)

-- Parse command line options
local USAGE = "\nUsage: "..arg[0].." <option>\n"..[[
Options:
        -1   Use phong.vert and phong.frag
        -2   Use function.vert and function.frag
]]

local option = arg[1]
local vertex_shader, fragment_shader

if not option then
   print(USAGE)
   os.exit(true)
elseif option == '-1' then
   vertex_shader = "shaders/phong.vert"
   fragment_shader = "shaders/phong.frag"
elseif option == '-2' then
   vertex_shader = "shaders/function.vert"
   fragment_shader = "shaders/function.frag"
else
   print("Invalid option")
   print(USAGE)
   os.exit(true)
end

-- Create the shader program
local prog, vsh, fsh = gl.make_program('vertex', vertex_shader, 'fragment', fragment_shader)
gl.delete_shaders(vsh, fsh)
gl.use_program(prog)

-- Get the locations of the uniform variables
local uniforms = {
    "Material.Kd", 
    "Material.Ka",
    "Material.Ks",
    "Material.Shininess",
    "Light.Position",
    "Light.Ld",
    "Light.La",
    "Light.Ls",
    "ModelViewMatrix",
    "NormalMatrix",
    "MVP",
}
local loc = {}
for _,name in ipairs(uniforms) do loc[name] = gl.get_uniform_location(prog, name) end

-- Initialize the uniform variables
local view = glmath.look_at(vec3(0.0,0.0,2.0), vec3(0.0,0.0,0.0), vec3(0.0,1.0,0.0))
resize(window, W, H) -- creates projection
local world_light = vec4(5.0,5.0,2.0,1.0)

local function set_matrices(model)
   local mv = view * model
   local normal_mv = mat3(mv):inv():transpose()
   gl.uniform_matrix4f(loc["ModelViewMatrix"], true, mv)
   gl.uniform_matrix3f(loc["NormalMatrix"], true, normal_mv)
   gl.uniform_matrix4f(loc["MVP"], true, projection * mv)
end

gl.uniformf(loc["Material.Kd"], 0.9, 0.5, 0.3)
gl.uniformf(loc["Light.Ld"], 1.0, 1.0, 1.0)
gl.uniformf(loc["Light.Position"], view * world_light)
gl.uniformf(loc["Material.Ka"], 0.9, 0.5, 0.3)
gl.uniformf(loc["Light.La"], 0.4, 0.4, 0.4)
gl.uniformf(loc["Material.Ks"], 0.8, 0.8, 0.8)
gl.uniformf(loc["Light.Ls"], 1.0, 1.0, 1.0)
gl.uniformf(loc["Material.Shininess"], 100.0)

-- Generate the torus mesh
local torus = new_torus(0.7, 0.3, 50, 50)

-- Event loop -----------------------------------------------------------------

gl.enable('depth test')
gl.clear_color(0.5, 0.5, 0.5, 1.0)

print("Press space to toggle animation on/off")

local t0 = glfw.now()

while not glfw.window_should_close(window) do
   glfw.poll_events()

   -- Update
   local t = glfw.now()
   local dt = t - t0
   t0 = t
   if animate then
      angle = angle + speed*dt
      if angle >= 2*pi then angle = angle - 2*pi end
   end

   local model = glmath.rotate(angle, 0, 1, 0)*
                 glmath.rotate(rad(-35), 1, 0, 0)*
                 glmath.rotate(rad(35), 0, 1, 0)
   set_matrices(model)

   -- Render
   gl.clear('color', 'depth')
   torus:render()

   glfw.swap_buffers(window)
end

