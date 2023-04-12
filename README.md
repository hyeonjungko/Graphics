# Lighting, Texture Mapping & Raytracing
## Running the Program

  ```
  make clean
  make Scenegraphs
  ./Scenegraphs -i scenegraphmodels/{FILENAME}.txt
  ```
## Implemented & Nonimplemented Features

:white_check_mark: Ray is created in the correct coordinate system

:white_check_mark: Ray is transformed correctly before intersection

:white_check_mark: Point of intersection is correctly determined and transformed for both objects

:white_check_mark: Normals are correctly calculated and transformed for both objects

:white_check_mark: Diffuse shading matches opengl rendering

:white_check_mark: Specular shading matches opengl rendering

:white_check_mark: Spot lights work, raytraced result matches opengl rendering

:white_check_mark: Image(`./raytracer_outputs/raytrace-sphere-and-box-and-spotlight-output.ppm`) showing the final rendering (the best output) of your ray tracer (at least 800x800)

:white_check_mark: Image(`./raytracer_outputs/raytrace-sphere-and-box-and-spotlight-output.png`) showing the opengl rendering of the same scene and the same camera position as your final raytraced image

:white_check_mark: Image(`./raytracer_outputs/raytrace-sphere.ppm`) showing the ray tracing for spheres

:white_check_mark: Image(`./raytracer_outputs/raytrace-box.ppm`) showing the ray tracing for boxes

:white_check_mark: Spot lights work, raytraced result matches opengl rendering

:white_check_mark: Program compiles and runs without errors






