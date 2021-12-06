# Private Point Location

## Description
This is a project done as a part of the CSCI-716 Computational Geometry course at RIT. In this project, I investigate a topic which combines Planar Point Location and Homomorphic Encryption (HE). 

Through this project, I would like to explore the question: is it possible to conduct Planar Point Location in a Trapezoidal Map given the coordinates of the point are encrypted? Homomorphic Encryption (HE) is an encryption scheme which allows for arithmetic over data without requiring decryption. Because of this, I aim in this project to combine HE with Trapezoidal Maps to conduct Planar Point Location.

## Dependencies
All dependencies can be installed via the bootstrap.sh script. 

## Compile

First, run `./bootstrap.sh`

Call `cmake .` A common error comes from needing to change the path in `CMakeCache.txt` to the path of your current directory

Call `make`

## How to Run

### Segments file
First, the file `ac7717.txt` contains the format for which the segments are specified. The segments currently in the text file and resulting trapezoidal map are graphed at https://people.rit.edu/ac7717/csci716.html

The first line is one number, specifying the total number of segments

The second line is the bounds of the plane `0 0 MAX_X MAX_Y`

Next, segments are specified by `vertex1_X vertex1_Y vertex2_X vertex2_Y`.

### Plaintext mode

Plaintext mode only runs the plaintext component: constructing the trapezoid maps, printing the DAG, and printing all paths to trapezoids.

To run this mode, simply call `./bin/privPointLoc ptxt`

### Private Point Location

The full program encrypts a point specified by the user, and completes private point location via traversing the DAG representing the trapezoidal map.

First, consider the point which should be located. Given its coordinates are `X` and `Y`, execute this command to run the program: `./bin/privPointLoc main 8 X Y`

## Contact
Name: Adam Caulfield

Email: ac7717@rit.edu