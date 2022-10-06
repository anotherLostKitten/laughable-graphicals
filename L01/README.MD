## L01

Theodore Peters 260919785

`./L01 ../resources/` will run the program with the Bollywood .bvh file.

To run with a different file, specify a 3rd argument:

`./L01 ../resources/ 1` will run with the Cyrus .bvh;

`./L01 ../resources/ 2` will run with the OptiTrack .bvh.

Anything else will default to the Bollywood file.

Limbs are being generated based on the assumption that in the reference pose in the .bvh files, all the limbs will be more or less colinear with axes. This is the case for the provided files; so this method works, but if the provided files had been in an a-pose something more complicated would have to be done to get the thin looking limbs.

Axis colors are being assigned by clamping position vectors in the vertex shader, as the XYZ values of each basis vector corresponds to the RGB color value we want to assign it.
