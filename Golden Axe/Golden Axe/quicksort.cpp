#include "quicksort.h"

using namespace std;

int divide(vector<Module*> &v, int i, int j) {

	Module* temp;
	int x = v[i]->GetScreenHeight();
	int left = i;
	int right;

	for (right = i + 1; right < j; right++)
	{
		if (v[right]->GetScreenHeight() < x)
		{
			left++;
			temp = v[left];
			v[left] = v[right];
			v[right] = temp;
		}

	}

	temp = v[left];
	v[left] = v[i];
	v[i] = temp;

	return left;
}

void quicksort(vector<Module*> &v, int i, int j) {

	int pivot;

	if (i < j) {
		pivot = divide(v, i, j);

		quicksort(v, i, pivot);
		quicksort(v, pivot+1, j);
	}
}
