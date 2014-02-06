/**********************************************
File		:	SampleTree.cpp
Author		:	Mingcheng Chen
Last Update	:	February 6th, 2014
***********************************************/

#include "SampleTree.h"
#include "Utility.h"

#include <cstring>

#include <algorithm>

void SampleTree::RecursiveRuild(const Node &currNode, int &cnt, int fr, int to, int *sortArray, int *assistArray) {
    currNode.population = to - fr + 1;
    currNode.lowerPoint = Vector(samples[sortArray[fr]].GetX(),
                                 samples[sortArray[this->numOfSamples + fr]].GetY(),
                                 samples[sortArray[(this->numOfSamples << 1) + fr]].GetZ());
    currNode.upperPoint = Vector(samples[sortArray[to]].GetX(),
                                 samples[sortArray[this->numOfSamples + to]].GetY(),
                                 samples[sortArray[(this->numOfSamples << 1) + to]].GetZ());
    if (currNode.population <= this->maxNodePopulation) {
        currNode.leftIndex = currNode.rightIndex = -1;
        currNode.sampleList = new int [currNode.population];
        for (int i = fr; i <= to; i++)
            currNode.sampleList[i] = sortArray[i];
        return;
    }
    currNode.sampleList = NULL;
    currNode.leftIndex = ++cnt;
    currNode.rightIndex = ++cnt;

    double spanLength[] = {currNode.upperPoint.GetX() - currNode.lowerPoint.GetX(),
                           currNode.upperPoint.GetY() - currNode.lowerPoint.GetY(),
                           currNode.upperPoint.GetZ() - currNode.lowerPoint.GetZ()};
    int dim = 0;
    for (int i = 1; i < 3; i++)
        if (spanLength[i] > spanLength[dim])
            dim = i;

    double splittingPosition;
    switch (this->method) {
        case INTERSECTION:
            splittingPosition = this->GetSplittingPositionByIntersectionRate(fr, to, sortArray, currNode.lowerPoint, currNode.upperPoint);
            break;
    }
}

static struct SampleComparator {
    int dimension; // 0: X, 1: Y, 2: Z
    const Vector *samples;
    bool operator() (int a, int b) const {
        switch (this->dimension) {
            case 0: return samples[a].GetX() < samples[b].GetX();
            case 1: return samples[a].GetY() < samples[b].GetY();
            case 2: return samples[a].GetZ() < samples[b].GetZ();
        }
        Error("unknown dimension for comparison in sorting");
    }
};

void SampleTree::Build(const Vector *samples, int numOfSamples) {
    if (this->maxTreeDepth == -1)
        Error("Parameters are not set for building a sample tree.");

    this->numOfSamples = numOfSamples;
    this->samples = new Vector [numOfSamples];
    memcpy(this->samples, samples, sizeof(Vector) * numOfSamples);

    int *sortArray = new int [numOfSamples * 3];
    int *assistArray = new int [numOfSamples];

    SampleComparator comparator;
    comparator.samples = samples;
    for (int dim = 0; dim < 3; dim++) {
        int *arr = sortArray + dim * numOfSamples;
        for (int j = 0; j < numOfSamples; j++)
            arr[j] = j;
        comparator.dimension = dim;
        std::sort(arr, arr + numOfSamples, comparator);
    }

    int numOfNodes = numOfSamples * 2 - 1;
    this->nodes = new SampleTree::Node [numOfNodes];

    int cnt = 0;
    this->RecursiveBuild(nodes[0], cnt, 0, numOfSamples - 1,
                         sortArray, assistArray, samples);

    delete [] assistArray;
    delete [] sortArray;
}
