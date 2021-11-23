// By Adam Caulfield (ac7717)
// November 3 2021

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

    std::vector<int> getLeftEndpoint();
    void setLeftEndpoint(int, int);

    std::vector<int> getRightEndpoint();
    void setRightEndpoint(int, int);

    int getSegID();
    void setSegID(int);

    void initDual();

    int getYonSeg(int);

private:
    int slope;
    int intercept;
    int nodeListSize = 0;
    int id;
    std::vector<int> rightEndpoint;
    std::vector<int> leftEndpoint;
};

#endif /*PRIVPOINTLOC_SEGMENT_H*/