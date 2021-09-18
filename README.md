# Computer_graphics_project
This project is a C++ program created using OpenGL, GLFW, GLM and Glad.

The project contains:
* **Models**: The one ring
* **Textures**: mordor_skybox, lava.jpg, middle_earth.jpg ,stone.jpg
* **Lighting**: directional, spot, and point light implemented using Blinn-Phong lighting model
* **Shaders**: lightshow and skybox shaders
* **Skybox cubemap**: (large)cube that encompases the entire scene, contains 6 images
* **Face culling**: if a side on an object cannot be seen, don't render it
* **By pressing**:
  * **B**: activate/deactivate Blinn-Phong lighting model
  * **Arrow keys**: move the ring forwards, backwards, left and right
  * **M**: move the ring up
  * **N**: move the ring down
  * **V**: switch spot light on/off