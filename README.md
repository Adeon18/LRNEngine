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
