#ifndef __QUICKSORT_H__
#define __QUICKSORT_H__

#include <vector>
#include "Module.h"

int divide(std::vector<Module*> &v, int i, int j);
void quicksort(std::vector<Module*> &v, int i, int j);

#endif