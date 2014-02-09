/**********************************************
File		:	Utility.h
Author		:	Mingcheng Chen
Last Update	:	February 8th, 2014
***********************************************/

#ifndef __UTILITY_H
#define __UTILITY_H

#include <cstdio>

#include <string>

void Error(const char *format, ...);
double ParseDouble(const char *str);
int ParseInt(const char *str);
void ConsumeChar(char goal, FILE *fin);
double GetCurrentTimeInSeconds();

int Code(int x, int y, int z, int ny, int nz);
void Decode(int &x, int &y, int &z, int code, int ny, int nz);
const char *GetSuffix(const char *str);

class Configuration {
private:
    int maxNodePopulation;
    int maxTreeDepth;

public:
    void LoadFile(const char *fileName);

    int GetMaxNodePopulation() const {
        return this->maxNodePopulation;
    }

    int GetMaxTreeDepth() const {
        return this->maxTreeDepth;
    }
};

#endif
