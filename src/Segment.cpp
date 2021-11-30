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
	printf("SEG INITIALIZED: (slope=%.2f, intercept=%d)\n", slope, intercept);	
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
	printf("ENDPOINT_R: [%d, %d]\t ENDPOINT_L [%d, %d]\n", rightEndpoint[0], rightEndpoint[1], leftEndpoint[0], leftEndpoint[1]);
	if(rightEndpoint[1]==leftEndpoint[1]){
		printf("Slope = zero\n");
		slope = 0;
	} else {
		printf("Slope = %d/%d\n", rightEndpoint[1]-leftEndpoint[1], rightEndpoint[0]-leftEndpoint[0]);
		slope = double(rightEndpoint[1]-leftEndpoint[1]) / double(rightEndpoint[0]-leftEndpoint[0]);
	}

	intercept = leftEndpoint[1]-int((slope*leftEndpoint[0]));
}

int Segment::getYonSeg(int x){
	return int(slope*x+intercept);
}

int Segment::getSegID(){
	return id;
}

void Segment::setSegID(int i){
	id = i;
	// printf("SegID = %d\n", id);
}