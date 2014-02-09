/*
Program: Point Sampler
Author: Mingcheng Chen
*/

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <climits>

#include <algorithm>

double GetRandomDouble() {
    return (double)rand() / INT_MAX;
}

double GetRandomDouble(double lower, double upper) {
    return (upper - lower) * GetRandomDouble() + lower;
}

int main(int argc, char **argv) {
    if (argc != 8) {
        printf("sampler <minX> <maxX> <minY> <maxY> <minZ> <maxZ> <numOfSamples>\n");
        exit(-1);
    }

    double minX, maxX, minY, maxY, minZ, maxZ;
    int numOfSamples;

    sscanf(argv[1], "%lf", &minX);
    sscanf(argv[2], "%lf", &maxX);
    sscanf(argv[3], "%lf", &minY);
    sscanf(argv[4], "%lf", &maxY);
    sscanf(argv[5], "%lf", &minZ);
    sscanf(argv[6], "%lf", &maxZ);
    sscanf(argv[7], "%d", &numOfSamples);

    FILE *fout = fopen("samplers.txt", "w");

    fprintf(fout, "%d\n", numOfSamples);
    for (int i = 0; i < numOfSamples; i++)
        fprintf(fout, "%lf %lf %lf\n", GetRandomDouble(minX, maxX), GetRandomDouble(minY, maxY), GetRandomDouble(minZ, maxZ));

    fclose(fout); 
    return 0;
}
