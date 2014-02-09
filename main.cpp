/**********************************************
File		:	main.cpp
Author		:	Mingcheng Chen
Last Update	:	February 6th, 2014
***********************************************/

#include "Utility.h"
#include "Geometry.h"
#include "SampleTree.h"

#include <vtkUnstructuredGridReader.h>
#include <vtkSmartPointer.h>

#include <cstdio>
#include <cstring>

#include <algorithm>

const char *configurationFile = "SampleTree.conf";

Configuration conf;
Tetrahedron *cells;
Vector *samples;
SampleTree sampleTree;
int numOfCells, numOfSamples;

void DisposeAll() {
    delete [] cells;
    delete [] samples;
}

void LoadConfiguration() {
    printf("LoadConfiguration()\n");

    conf.LoadFile(configurationFile);

    printf("Done.\n\n");
}

void LoadGrid(const char *filename) {
    printf("LoadGrid()\n");

    FILE *fin = fopen(filename, "r");
    if (fin == NULL)
        Error("Grid file \"%s\" cannot be opened.", filename);

    const char *suffix = GetSuffix(filename);

    if (!strcmp(suffix, "vtk")) {
        vtkSmartPointer<vtkUnstructuredGridReader> reader = vtkSmartPointer<vtkUnstructuredGridReader>::New();
        reader->SetFileName(filename);
        reader->Update();

        TetrahedralGrid tetGrid(reader->GetOutput());

        numOfCells = tetGrid.GetNumOfCells();
        cells = new Tetrahedron [numOfCells];
        for (int i = 0; i < numOfCells; i++) {
            Tetrahedron tet = tetGrid.GetTetrahedron(i);
            for (int j = 0; j < 4; j++)
                cells[i].SetVertex(j, tet.GetVertex(j));
        }
    } else if (!strcmp(suffix, "vtu")) {
        Error("not implemented for vtu files");
    } else {
        fscanf(fin, "%d", &numOfCells);
        cells = new Tetrahedron [numOfCells];
        for (int i = 0; i < numOfCells; i++)
            for (int j = 0; j < 4; j++) {
                double x, y, z;
                fscanf(fin, "%lf %lf %lf", &x, &y, &z);
                cells[i].SetVertex(j, Vector(x, y, z));
            }
    }

    fclose(fin);

    printf("numOfCells = %d\n", numOfCells);
    printf("Done.\n\n");
}

void LoadSamples(const char *filename) {
    printf("LoadSamples()\n");

    FILE *fin = fopen(filename, "r");
    if (fin == NULL)
        Error("Sample file \"%s\" cannot be opened.", filename);

    fscanf(fin, "%d", &numOfSamples);
    samples = new Vector [numOfSamples];
    for (int i = 0; i < numOfSamples; i++) {
        double x, y, z;
        fscanf(fin, "%lf %lf %lf", &x, &y, &z);
        samples[i] = Vector(x, y, z);
    }

    fclose(fin);

    printf("numOfSamples = %d\n", numOfSamples);
    printf("Done.\n\n");
}

void BuildSampleTree(const Vector *samples, int numOfSamples) {
    printf("BuildSampleTree()\n");

    sampleTree.SetParameters(conf.GetMaxNodePopulation(), conf.GetMaxTreeDepth(), SampleTree::INTERSECTION);
    sampleTree.Build(samples, numOfSamples);
    printf("sampleTree.GetNumOfNodes() = %d\n", sampleTree.GetNumOfNodes());

    sampleTree.Output();

    printf("Done.\n\n");
}

int main(int argc, char **argv) {
    if (argc != 3)
        Error("locator <sample> <unstructured grid>");
    char *sampleFile = argv[1];
    char *gridFile = argv[2];

    LoadConfiguration();
    LoadGrid(gridFile);
    LoadSamples(sampleFile);
    BuildSampleTree(samples, numOfSamples);

    DisposeAll();

    return 0;
}
