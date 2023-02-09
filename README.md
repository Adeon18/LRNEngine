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
