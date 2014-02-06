/**********************************************
File		:	main.cpp
Author		:	Mingcheng Chen
Last Update	:	February 6th, 2014
***********************************************/

#include "Utility.h"
#include "Geometry.h"

#include <cstdio>

#include <algorithm>

Tetrahedron *cells;
Vector *samples;
int numOfCells, numOfSamples;

void DisposeAll() {
    delete [] cells;
    delete [] samples;
}

void LoadGrid(const char *filename) {
    FILE *fin = fopen(filename, "r");
    if (fin == NULL)
        Error("Grid file \"%s\" cannot be opened.", filename);

    fscanf("%d", &numOfCells);
    cells = new Tetrahedron [numOfCells];
    for (int i = 0; i < numOfCells; i++)
        for (int j = 0; j < 4; j++) {
            double x, y, z;
            fscanf("%lf %lf %lf", &x, &y, &z);
            cells[i].SetVertex(j, Vector(x, y, z));
        }

    fclose(fin);
}

void LoadSamples(const char *filename) {
    FILE *fin = fopen(filename, "r");
    if (fin == NULL)
        Error("Sample file \"%s\" cannot be opened.", filename);

    fscanf("%d", &numOfSamples);
    samples = new Vector [numOfSamples];
    for (int i = 0; i < numOfSamples; i++) {
        double x, y, z;
        fscanf("%lf %lf %lf", &x, &y, &z);
        samples[i] = Vector(x, y, z);
    }

    fclose(fin);
}

int main(int argc, char **argv) {
    if (argc != 3)
        Error("locator <sample> <unstructured grid>");
    char *sampleFile = argv[1];
    char *gridFile = argv[2];

    LoadGrid(gridFile);
    LoadSamples(sampleFile);
    BuildSampleTree(samples, numOfSamples);

    DisposeAll();

    return 0;
}
