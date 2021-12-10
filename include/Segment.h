/**
* Authored by Adam Caulfield (ac7717). 
* November 3 2021.
*/

#ifndef PRIVPOINTLOC_SEGMENT_H
#define PRIVPOINTLOC_SEGMENT_H

#include <iostream>
#include <fstream>
#include <vector>
#include "Node.h"

class Node;

class Segment {
public:
    // Segment();
    Segment(int , int , int , int );

    /**
       * Get the left endpoint of the segment
       * @param void
       * @return std::vector<int>
    */
    std::vector<int> getLeftEndpoint();
    

    /**
       * Set the left endpoint of the segment
       * @param std::vector<int>
       * @return void
    */
    void setLeftEndpoint(int, int);


    /**
       * Get the right endpoint of the segment
       * @param void
       * @return std::vector<int>
    */
    std::vector<int> getRightEndpoint();

    /**
       * Set the right endpoint of the segment
       * @param std::vector<int>
       * @return void
    */
    void setRightEndpoint(int, int);

    /**
       * Get the segment ID
       * @param void
       * @return int
    */
    int getSegID();

    /**
       * Set the segment ID
       * @param int
       * @return void
    */
    void setSegID(int);

    /**
       * Initialize the dual coordinates (slope, intercept)
       * @param void
       * @return void
    */
    void initDual();

    /**
       * Given the slope and intercept of this segment, return the Y value for a given X
       * @param int x
       * @return int y
    */
    int getYonSeg(int);

    /**
       * Get the segment intercept
       * @param void
       * @return int
    */
    int getIntercept();

    /**
       * Get the slope denominator (dx)
       * @param void
       * @return int
    */
    int getDx();

    /**
       * Get the slope numerator (dy)
       * @param void
       * @return int
    */
    int getDy();

private:
    double slope;
    int intercept, dx, dy;
    int nodeListSize = 0;
    int id;
    std::vector<int> rightEndpoint;
    std::vector<int> leftEndpoint;
};

#endif /*PRIVPOINTLOC_SEGMENT_H*/