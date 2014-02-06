/**********************************************
File		:	SampleTree.h
Author		:	Mingcheng Chen
Last Update	:	February 6th, 2014
***********************************************/

#ifdef __SAMPLE_TREE_H
#define __SAMPLE_TREE_H

#include "Geometry.h"

class SampleTree {
private:
    struct Node {
        Vector lowerPoint, upperPoint;
        int population;
        int leftIndex, rightIndex;
        int *sampleList;
    };

    Node *nodes;
    Vector *samples;
    int numOfSamples;
    int maxNodePopulation;
    int maxTreeDepth;
    SplittingMethod method;

    void GetSplittingPositionByIntersectionRate(int fr, int to, int *sortArray, const Vector &lowerPoint, const Vector &upperPoint);
    void RecursiveBuild(const Node &currNode, int &cnt, int fr, int to, int *sortArray, int *assistArray);

public:
    enum {MEAN, MEADIUM, VOLUMN, INTERSECTION} SplittingMethod;

    SampleTree() {
        this->nodes = NULL;
        this->samples = NULL;
        this->maxTreeDepth = -1; // Marked as parameters are not set
    }

    ~SampleTree() {
        if (this->nodes) {
            for (int i = (numOfSamples << 1) - 2; i >= 0; i--)
                if (this->nodes[i].sampleList)
                    delete [] nodes[i].sampleList;
            delete [] this->nodes;
        }
        if (this->samples) delete [] this->samples;
    }

    void SetParameters(int maxNodePopulation, int maxTreeDepth, SplittingMethod method) {
        this->maxNodePopulation = maxNodePopulation;
        this->maxTreeDepth = maxTreeDepth;
        this->method = method;
    }

    void Build(const Vector *samples, int numOfSamples);
};

#endif
