## 260919785-theodorepeters-competition.png

![260919785-theodorepeters-competition](https://user-images.githubusercontent.com/25112030/213032474-b9c3bd5e-9071-4d9c-bd57-85adabd5453e.png)

Novel scene for competition submission -- for the competition I upscaled it to 1024x1024 which took some like 15 minutes (I took a shower and when I returned it was done) to render so I would not recommend doing that

Right now the json file is at 256x256 which took about ~60 seconds to run in release mode on my machine

This file is using the dragon.obj mesh which was included in assignment 3 -- it is exactly the same file, so I will not upload it because it is like 72Mb and even when zipped it was still some 17Mb so I think it would be better to avoid uploading that one to mycourses...

Just put the dragon from L03 in resources/objs and it will work.

### This file demonstrates the following features:

Fresnel reflections, specified by setting an optional "mirror" value to each of the materials to dictate how much mirrored reflections of each colour should contribute

Multithreading, right now the scene is spawning 8 threads; each thread renders 1/N of the rows of the image

BVH, meshes are processed into a BVH with median space partitioning when they are loaded

Enviornment maps, There is an enviornment map loaded as a .ppm file -- I used ImageMagick to convert it from a PNG I wasn't really interested in doing complicated image processing in cpp so I just used a .ppm instead, other image file formats will not work.

## competition_downscale.png

Exactly the same scene as the competition submission, but downscaled to 256x256 -- what the 260919785-theodorepeters-competition.png file specified currently

## Other testing files:

All of these files ran pretty much instantly in release mode on my machine

## aacp.png

Antialiasing, output of AACheckerPlane.json

## boxrgb.png

AABB intersection, output of BoxRGBLights.json

## boxstacks.png

Hierarchical (which is mispelt as "hierachical" in the source files) shapes

## plane2.png

Output of Plane2.json

## plane.png

Output of Plane.json

## sphere.png

Output of Sphere.json

## torus.png

Output of TorusMesh.json

## twosphere.png

Output of TwoSpheresPlane.json

## reflectionspheres.png

Demonstrates reflections, Output of ReflectionSpheres.json
