#include "Segment.h"
#include "Node.h"
// Segment::Segment(){
// 	int xl=0;
// 	int yl=0;
// 	int xr=0;
// 	int yr=0;
// 	leftEndpoint = {xl, yl};
// 	rightEndpoint = {xr, yr};
// 	slope = 0;
// 	intercept = 0;
// }

Segment::Segment(int xl, int yl, int xr, int yr){
	leftEndpoint = {xl, yl};
	rightEndpoint = {xr, yr};
	this->initDual();
}

std::vector<int> Segment::getLeftEndpoint(){
	return leftEndpoint;
}

void Segment::setLeftEndpoint(int xl, int yl){
	leftEndpoint = {xl, yl};
}

std::vector<int> Segment::getRightEndpoint(){
	return rightEndpoint;
}

void Segment::setRightEndpoint(int xr, int yr){
	rightEndpoint = {xr, yr};
}

void Segment::initDual(){
	slope = int((rightEndpoint[1]-leftEndpoint[1])/rightEndpoint[0]-leftEndpoint[0]);
	intercept = leftEndpoint[1]-slope*leftEndpoint[0];
}

int Segment::getYonSeg(int x){
	return slope*x+intercept;
}

int Segment::getSegID(){
	return id;
}

void Segment::setSegID(int i){
	id = i;
	// printf("SegID = %d\n", id);
}