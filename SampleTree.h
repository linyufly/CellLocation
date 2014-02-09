/**********************************************
File		:	SampleTree.h
Author		:	Mingcheng Chen
Last Update	:	February 6th, 2014
***********************************************/

#ifndef __SAMPLE_TREE_H
#define __SAMPLE_TREE_H

#include "Geometry.h"

class SampleTree {
public:
    enum SplittingMethod {MEAN, MEDIUM, VOLUMN, INTERSECTION};

    SampleTree() {
        this->nodes = NULL;
        this->samples = NULL;
        this->maxTreeDepth = -1; // Marked as parameters are not set
    }

    ~SampleTree() {
        if (this->nodes) {
            for (int i = 0; i < this->numOfNodes; i++)
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

    int GetNumOfNodes() const {
        return this->numOfNodes;
    }

    void Build(const Vector *samples, int numOfSamples);

private:
    struct Node {
        Vector lowerPoint, upperPoint;
        int population;
        int leftIndex, rightIndex;
        int *sampleList;
    };

    SampleTree::Node *nodes;
    Vector *samples;
    int numOfSamples;
    int numOfNodes;
    int maxNodePopulation;
    int maxTreeDepth;
    SplittingMethod method;

    double GetSplittingPositionByIntersectionRate(int fr, int to, int *sortArray, int dim, const Vector &lowerPoint, const Vector &upperPoint, double *accumulativeTop, double *accumulativeBottom);
    void RecursiveBuildByIntersectionRate(SampleTree::Node &currNode, int &cnt, int fr, int to, int *sortArray, int *assistArray, double *accumulativeTop, double *accumulativeBottom);
};

#endif
