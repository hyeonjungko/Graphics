# Lighting, Texture Mapping & Raytracing
## Running the Program

  ```
  make clean
  make Scenegraphs
  ./Scenegraphs -i scenegraphmodels/{FILENAME}.txt
  ```
## Features

### OpenGL Render
* object rendering from XML
* object transformations including translation, scale, and rotation
* real-time perspective updates during program runs with keyboard and mouse inputs
* texture mapping from ASCII PPM format images
* point lights and spotlight implementation

### Raytracer
* ACSII PPM image output of raytracer upon `c` keyboard press
* implements all OpenGL capabilities + shadows + reflections
* shadows allow multiple lights
* reflections prevent infinite bouncing of the ray
* example raytracer images in `./raytracer_outputs`
