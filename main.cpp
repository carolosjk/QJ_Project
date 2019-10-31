#include <iostream>
#include "Utils.h"

using namespace std;

int main(int argc, char **argv)
{

    char* file = "t4.txt";
    Tuple a = getMatrixSize(file);
    //cout << a.getKey() << " " << a.getPayload() << endl;
    Matrix* matrix = new Matrix(a.getPayload(),a.getKey());
    if(matrix->setMatrix(file))
        cout << "success" << endl;
    //matrix->printMatrix();
    Relation* R1,*R2;
    R1 = matrix->getRelation(1);
    R2 = matrix->getRelation(2);
    cout << "success" << endl;
//    R1->print();
//    R2->print();
    Radixsort(R1,0,R1->getNumTuples()-1);
    Radixsort(R2,0,R2->getNumTuples()-1);
    R1->isSorted();
    R2->isSorted();
    SortMergeJoin(R1,R2);
    return 0;
}
