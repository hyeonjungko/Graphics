# Lighting and Texture Mapping
## Running the Program

  ```
  make clean
  make Scenegraphs
  ./Scenegraphs -i scenegraphmodels/scene.txt
  ```
## Implemented & Nonimplemented Features

:white_check_mark: At least 2 objects with texture images

:white_check_mark: Mipmapping enabled

:white_check_mark: Image credits provided

:white_check_mark: Texture matches the provided image

:white_check_mark: Two stationary lights, spotlight and point light

:white_check_mark: Light Implementation: Each node can have multiple lights 

:white_check_mark: Light Implementation: Lights are transformed correctly (eyeball code that gets lights in scene graph) 

:white_check_mark: Light Implementation: All lights are sent to shader before drawing any object.

:white_check_mark: Spotlight can be seen

:white_check_mark: Spotlight implementation: 1. Cosine of spot angle sent to shader. 2. Shader calculates dot product of -L and D, math is correct

:white_check_mark: XML support for lighting and texturing

:x: Objects are colored appropriately

## Texture Image Credits
`textures/pexels-anni-roenkae-2832432.ppm`: Photo by Anni Roenkae: https://www.pexels.com/photo/photo-of-acrylic-paint-2832432/

`textures/abstract.ppm`: Image by <a href="https://www.freepik.com/free-photo/abstract-pixelated-background_29795083.htm#query=pixel%20texture&position=8&from_view=search&track=ais">Freepik</a>


