//
// Copyright SpiRITlab - The PRIVPOINTLOC project.
// https://github.com/SpiRITlab/PRIVPOINTLOC
//

#ifndef PRIVPOINTLOC_NODE_H
#define PRIVPOINTLOC_NODE_H

#include <iostream>
#include <fstream>

namespace privPointLoc {

    class Node {
    public:
        Node * getLeft();
        Node * getRight();
    private:
        int depth;
        int total;
        Node * left;
        Node * right;
    };

}

#endif //PRIVPOINTLOC_NODE_H
