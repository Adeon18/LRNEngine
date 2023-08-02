# Ostap Trush

## HW1: Simple raytracer with moving Sphere
- The red sphere moves along XY
- The plane to which a shadow is projected is angled at 45 def Z and 45 deg Y
- The render caps out at 60 FPS

## HW2: Expanding Raytracer
Additions:
- Made glm lib a submodule
- A moving and rotating camera implemented via quaternions
	- Move via `WASD`
	- Rotate with mouse `LMB`, roll with `E, Q`
- Meshes and fast mesh intersection with OcTree
- Blinn-Phong lighting model with support for one Directional Light and multiple PointLights and Spotlights. Added light colors too.
- Material support with some predefined materials
- Object dragging parallel to camera via `RMB` on object and mouse move
- Render is now done concurrently via a ParallelExecutor

## HW3: HelloTringle/HelloShaderToyTringle
Additions:
- Made a config file that allows to choose between the DX engine and the RayTracer by setting a `#define DX_ENGINE` in `engine/include/config`.
- Initialized DX Context
- Window Resizing Works
- Initialized the Rendering Pipeline
- VS and PS both with constant buffers for each of them
- There is a `#define VORONOI` in the `engine/render/Graphics/shaders/PS.hlsl` that defines what to run: **HelloTrinagle** or **HelloShaderToyTriangle**.
- The shaders are found always by absolute, no matter the working directory. Can run everywhere!

## HW4: Instanced rendering, Model Loading and more
How to run:
1. Get all the submodules(spdlog, assimp)
2. Make assimp work:
	- Easy way: `vcpkg install assimp` and add vckkg `installed` directory to `engine` and `project` properties in visual studio
	- Another way (For me, did not work very stable):
		- Disable `BUILD_SHARED_LIBS` in `lib/assimp/CMakeLists.txt` and build it, should link and work, but only `x86`.

Additions:
- Some model assets to play with
- Added a quaternion camera
- Added a depth-reversed depth buffer
- Created a model manager to import models from files using assimp
- Added a MeshSystem for instenced rendering of meshes
- Able to render multiple cubes/models with different colors/shaders
- Added mesh instance dragging
- Connected hologram shaders that are position independant and applied them to models.

## HW5: Tesselation, Hull/Domain/Geometry shaders, Degug Shaders
How to run:
- Same as HW4

Additions:
- Connected 3 more shaders to pipeline: Hull, Domain and Geometry.
- Added tesselation
- Added debug face normal visualization, press `N`
- Added wireframe visualization, press `F`
- Rewrote the pipeline system

## HW6: Textures and stuff
How to run:
- Same as previous HW
- Added full texture support
- Added cubes to scene with 2 different textures
- Added a skybox using a fullscreen triangle technique

## HW7
- Added lighting and 3 different casters(directional, point and spot lights)
- The pointlight are visualized by spheres that can be dragged(better to take sphere by the bottom, cuz dragging lags a bit othervise)
- The spotlight has a texture mask and can be deattached from camera by pressing `F`. The wireframe visualization moved to `H`.
- All the lights support different colors and intensities.
- I render the scene in the HDR backbuffer now.
- Added PostProcess with:
	- EV100 Exposure control(Adjust value via `+/-` keys)
	- Aces Tonemapping
	- Gamma correction that defaults to `2.2`
- The scene consists of: directional light and flashlight, 2 white pointlighs with distance 50 and
default intensity and 1 pointlight with distance 20 and twice the intensity.

## HW8: PBR
- Replaced Blinn-Phong with PBR: used Lambert Diffuse and Cook-Torrance specular
- Used solid angles instead of quadratic attenuation
- Added normal mapping
- Used PBR textures for metallness and roughness

## HW9: IBL
- Added ImGUI integration
- Added 2 modes of program operation: go to `engine/include/config.hpp`, set `BAKE_CUBEMAPS` to `1`, and the engine will start, generate cubemaps into `assets/Textures/Skyboxes`
directory and shut down. Set `BAKE_CUBEMAPS` to `0` after generation to run engine with IBL. Recommended to run the engine after the cubemap generation.
The `BAKE_CUBEMAPS` mode also prints the sum over the hemisphere into a `logs/engine.log` file.
- ReflectionCapture class can precompute Diffuse Irradinace, Pre-Filter Specular cubemaps and generate the 2D BRDF Integration LUT.
- Added horizon attenuation and the Carpentier's method for finding the closest representative point, to make better SphereLight lighting.
- The cubemaps are compressed into `BC6` and the 2D LUT - in `BC5`.

## HW10: Shadow Mapping
- Added pointlight, directional and spotlight shadows
- Added PCF to directional light
- Used comparison samplers
- Experimented with rasterizer depthbias
- Configured bias to have **less shadow flickering** instead of more precise shadows.

## HW11: Blending and Particles
- Added a model spawn effect on key `N`. In Imgui you can choose the model and animation speed.
- Added a smoke emitter, works on key `H`.
- Particles have animation and some velocity, ti add animation I used Motion Vectors

## HW12: Deferred render and Decals and FXAA
- Rewrote my renderer to deferred renderer. Particles are rendered afterwards via forward render
- Added Nvidia FXAA 3.11 with ability to tweak parameters in IMGUI
- Added a decal system. On `G` key pressed a decal is spawned **WHERE THE CURSOR IS**. A decal has a random rotation and a random color from
a 10-color-pool. Normal maps for decals are supported.

## HW13: GPU Particles
- Added a possibility to remove meshes by clicking `Del` while having a cursor on a mesh.
- Added GPU particles with depthbuffer collision and random color for particle.
- Rendering via `DrawIndexedInstancedIndirect`.
- Particles use ringBuffer to successfully respawn.
- Particles have diffuse only reflections.

### HW14: UE Fog of War
- Added a wandering sphere with destroy and respawn logic and decal spawn logic
- Added fog of war effect
- Pushed only ThirdPerson folder where I created all BPs and Materials, did not push started content