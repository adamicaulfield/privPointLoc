# Private Point Location

## Description
This is a project done as a part of the CSCI-716 Computational Geometry course at RIT. In this project, I investigate a topic which combines algorithms from Computational Geometry and Homomorphic Encryption (HE). 

In computational geometry, there are algorithms which make planar point location effieicnt. In application, this requires a user to share their location information to a 3rd party service who owns an application. The 3rd party will conduct the algorithm to identify their region, and will make an evaluation based on their region (such as closest resturaunt, whether their region is effected by a storm warning, etc.). 

A user may be interested in preserving the privacy of their location when using apps. This scenario motivates the work here. This introduces a challenge because these algorithms typically rely on knowledge of the points coordinates and make comparisons to edges and vertices of the regions. Because of this, I aim to use HE to make the point private, and make the algorithms for point location homomorphic so that it can be executed on a point (user location) which remains private throughout.

## Dependencies
This is implemented in C++, and uses HELib for the HE algorithms in C++. It also uses CMake 3.15 to build the project.

## Contact
Name: Adam Caulfield

Email: ac7717@rit.edu