/**********************************************
File		:	SampleTree.cpp
Author		:	Mingcheng Chen
Last Update	:	February 6th, 2014
***********************************************/

#include "SampleTree.h"
#include "Geometry.h"
#include "Utility.h"

#include <cstring>

#include <algorithm>

static double GetVectorComponent(const Vector &a, int dim) {
    switch (dim) {
        case 0: return a.GetX(); break;
        case 1: return a.GetY(); break;
        case 2: return a.GetZ(); break;
    }
    Error("invalid dimension");
}

double SampleTree::GetSplittingPositionByIntersectionRate(int fr, int to, int *sortArray, int dim, const Vector &lowerPoint, const Vector &upperPoint, double *accumulativeTop, double *accumulativeBottom) {
    accumulativeTop[to] = accumulativeBottom[to] = GetVectorComponent(this->samples[sortArray[to]], (dim + 1) % 3);
    accumulativeTop[to + this->numOfSamples] = accumulativeBottom[to + this->numOfSamples] = GetVectorComponent(this->samples[sortArray[to]], (dim + 2) % 3);
    for (int i = to - 1; i >= fr; i--) {
        double v1 = GetVectorComponent(this->samples[sortArray[i]], (dim + 1) % 3);
        double v2 = GetVectorComponent(this->samples[sortArray[i]], (dim + 2) % 3);

        accumulativeTop[i] = std::max(accumulativeTop[i + 1], v1);
        accumulativeBottom[i] = std::min(accumulativeBottom[i + 1], v1);

        accumulativeTop[i + this->numOfSamples] = std::max(accumulativeTop[i + 1 + this->numOfSamples], v2);
        accumulativeBottom[i + this->numOfSamples] = std::min(accumulativeBottom[i + 1 + this->numOfSamples], v2);
    }

    int lowerBound, upperBound;
    double top[2], bottom[2];
    top[0] = bottom[0] = GetVectorComponent(samples[sortArray[fr]], (dim + 1) % 3);
    top[1] = bottom[1] = GetVectorComponent(samples[sortArray[fr]], (dim + 2) % 3);

    double bestExpectation = -1;
    int bestUpperBound;

    for (lowerBound = fr; lowerBound < to; lowerBound = upperBound) {
        double pos = GetVectorComponent(this->samples[sortArray[lowerBound]], dim);
        for (upperBound = lowerBound; upperBound <= to && GetVectorComponent(this->samples[sortArray[upperBound]], dim) == pos; upperBound++) {
            double v1 = GetVectorComponent(this->samples[sortArray[upperBound]], (dim + 1) % 3);
            double v2 = GetVectorComponent(this->samples[sortArray[upperBound]], (dim + 2) % 3);

            top[0] = std::max(top[0], v1);
            bottom[0] = std::min(bottom[0], v1);

            top[1] = std::max(top[1], v2);
            bottom[1] = std::max(bottom[1], v2);
        }
        if (upperBound > to) break;

        double p1 = Sqr(GetVectorComponent(upperPoint, dim) - GetVectorComponent(lowerPoint, dim)) - 
                    Sqr(GetVectorComponent(this->samples[sortArray[fr]], dim) - GetVectorComponent(lowerPoint, dim)) -
                    Sqr(GetVectorComponent(upperPoint, dim) - GetVectorComponent(this->samples[sortArray[upperBound - 1]], dim));

        double p2 = Sqr(GetVectorComponent(upperPoint, (dim + 1) % 3) - GetVectorComponent(lowerPoint, (dim + 1) % 3)) -
                    Sqr(bottom[0] - GetVectorComponent(lowerPoint, (dim + 1) % 3)) -
                    Sqr(GetVectorComponent(upperPoint, (dim + 1) % 3) - top[0]);

        double p3 = Sqr(GetVectorComponent(upperPoint, (dim + 2) % 3) - GetVectorComponent(lowerPoint, (dim + 2) % 3)) -
                    Sqr(bottom[1] - GetVectorComponent(lowerPoint, (dim + 2) % 3)) -
                    Sqr(GetVectorComponent(upperPoint, (dim + 2) % 3) - top[1]);

        double expectation1 = p1 * p2 * p3 * (upperBound - fr);

        p1 = Sqr(GetVectorComponent(upperPoint, dim) - GetVectorComponent(lowerPoint, dim)) -
             Sqr(GetVectorComponent(this->samples[sortArray[upperBound]], dim) - GetVectorComponent(lowerPoint, dim)) -
             Sqr(GetVectorComponent(upperPoint, dim) - GetVectorComponent(this->samples[sortArray[to]], dim));

        p2 = Sqr(GetVectorComponent(upperPoint, (dim + 1) % 3) - GetVectorComponent(lowerPoint, (dim + 1) % 3)) -
                 Sqr(accumulativeBottom[upperBound] - GetVectorComponent(lowerPoint, (dim + 1) % 3)) -
                 Sqr(GetVectorComponent(upperPoint, (dim + 1) % 3) - accumulativeTop[upperBound]);

        p3 = Sqr(GetVectorComponent(upperPoint, (dim + 2) % 3) - GetVectorComponent(lowerPoint, (dim + 2) % 3)) -
                 Sqr(accumulativeBottom[upperBound + this->numOfSamples] - GetVectorComponent(lowerPoint, (dim + 2) % 3)) -
                 Sqr(GetVectorComponent(upperPoint, (dim + 2) % 3) - accumulativeTop[upperBound + this->numOfSamples]);

        double expectation2 = p1 * p2 * p3 * (to - upperBound + 1);

        double expectation = expectation1 + expectation2;
        if (bestExpectation < 0 || expectation < bestExpectation) {
            bestExpectation = expectation;
            bestUpperBound = upperBound;
        }
    }

    return GetVectorComponent(this->samples[sortArray[bestUpperBound - 1]], dim);
}

void SampleTree::RecursiveBuildByIntersectionRate(SampleTree::Node &currNode, int &cnt, int fr, int to, int *sortArray, int *assistArray, double *accumulativeTop, double *accumulativeBottom) {
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
            currNode.sampleList[i - fr] = sortArray[i];
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

    double splittingPosition = this->GetSplittingPositionByIntersectionRate(fr, to, sortArray + dim * this->numOfSamples, dim,
                                                                            currNode.lowerPoint, currNode.upperPoint,
                                                                            accumulativeTop, accumulativeBottom);

    int l1, l2;
    for (int d = 0; d < 3; d++) {
        l1 = l2 = 0;
        int *arr = sortArray + d * this->numOfSamples;
        for (int i = fr; i <= to; i++)
            if (GetVectorComponent(this->samples[arr[i]], dim) <= splittingPosition)
                arr[fr + l1++] = arr[i];
            else
                assistArray[fr + l2++] = arr[i];
        for (int i = 0; i < l2; i++)
            arr[fr + l1++] = assistArray[fr + i];
    }

    this->RecursiveBuildByIntersectionRate(nodes[currNode.leftIndex], cnt, fr, to - l2, sortArray, assistArray, accumulativeTop, accumulativeBottom);
    this->RecursiveBuildByIntersectionRate(nodes[currNode.rightIndex], cnt, to - l2 + 1, to, sortArray, assistArray, accumulativeTop, accumulativeBottom);
}

struct SampleComparator {
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
    switch (this->method) {
        case SampleTree::INTERSECTION: {
            double *accumulativeTop = new double [numOfSamples * 2];
            double *accumulativeBottom = new double [numOfSamples * 2];

            this->RecursiveBuildByIntersectionRate(nodes[0], cnt, 0, numOfSamples - 1,
                                                   sortArray, assistArray, accumulativeTop, accumulativeBottom);

            delete [] accumulativeTop;
            delete [] accumulativeBottom;
        } break;
    }

    this->numOfNodes = cnt + 1;

    delete [] assistArray;
    delete [] sortArray;
}

static void Indent(int indent) {
    for (int i = 0; i < indent; i++)
        printf(" ");
}

void SampleTree::RecursiveOutput(int currNode, int indent) const {
    Indent(indent);

    for (int i = 0; i < 3; i++) {
        if (i) printf(" x ");
        printf("[%lf, %lf]", GetVectorComponent(this->nodes[currNode].lowerPoint, i),
                             GetVectorComponent(this->nodes[currNode].upperPoint, i));
    }
    printf(", population = %d\n", this->nodes[currNode].population);

    if (this->nodes[currNode].sampleList) {
        Indent(indent);
        printf("Leaf:");
        for (int i = 0; i < this->nodes[currNode].population; i++) {
            Vector point = this->samples[this->nodes[currNode].sampleList[i]];
            printf(" (%lf, %lf, %lf)", point.GetX(), point.GetY(), point.GetZ());
        }
        printf("\n");
    } else {
        Indent(indent);
        printf("Non-leaf:\n");
        this->RecursiveOutput(this->nodes[currNode].leftIndex, indent + 4);
        this->RecursiveOutput(this->nodes[currNode].rightIndex, indent + 4);
    }
}

void SampleTree::Output() const {
    printf("Output SampleTree\n");

    this->RecursiveOutput(0, 0);

    printf("#END#\n");
}
