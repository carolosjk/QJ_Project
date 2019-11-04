//
// Created by wmsksf on 16/10/19.
//

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "Utils.h"
#include "LinkedList.h"
#include "Stack.h"

static uint64_t partition(Tuple* A, uint64_t p, uint64_t r)
{
    uint64_t pivot = A[r].getKey();
    int64_t i = p - 1;

    for (uint64_t j = p; j < r; j++)
        if (A[j].getKey() <= pivot)
        {
            i++;
            A[i].swap(&A[j]);
        }

    A[i + 1].swap(&A[r]);

    return i + 1;
}

void OptQuicksort(Tuple *A, uint64_t lo, uint64_t hi)
{

    if (hi == lo) return;
    Stack stack(hi-lo+1);


    stack.push(lo);
    stack.push(hi);

    while (!stack.isEmpty())
    {
        hi = stack.pop();
        lo = stack.pop();


        uint64_t pivot = partition(A, lo, hi);


//        >>>>>>>>>>>>>>>>>>>>>>>>
//        tail recursion to be added after testing of current code is successful
//        medianofthree might be added as well
//        >>>>>>>>>>>>>>>>>>>>>>>>


//        if elements on left side of pivot, push left side to stack
        if ( pivot > 0 and pivot - 1 > lo)
        {
            stack.push(lo);
            stack.push(pivot-1);
        }

//        if elements on right side of pivot, push right side to stack
        if ( pivot + 1 < hi)
        {
            stack.push(pivot+1);
            stack.push(hi);
        }
    }
}

//IN PROCESS...
void Radixsort(Relation *R, uint64_t start, uint64_t end, uint64_t current_byte, Relation* RR)
{
    if(end == 991918)
        std::cout << std::endl;
    if (current_byte == 56 && (end+1 - start) * sizeof(Tuple) < L1_CACHESIZE)
    {
//        Quicksort(R->getTuples(), start, end);
        OptQuicksort(R->getTuples(), start, end);

        return;
    }

    if (RR == nullptr)
    {
        RR = new Relation;
        RR->setNumTuples(R->getNumTuples());
        RR->initTuples();
    }

    uint64_t Hist[256] = {0};
    for (uint64_t i = start; i <= end; i++)
        Hist[(R->getTuples()[i].getKey() >> current_byte) & 0xff]++; // for byte 0 same as A[i] & 0xff

    uint64_t Psum[256] = {start};
    for (int i = 1; i < 256; i++)
        Psum[i] = Psum[i-1] + Hist[i-1];

    // utility in copying R to RR
    uint64_t tmp[256] = {0};
    for (uint64_t i = 0; i < 256; i++) tmp[i] = Psum[i];

    Tuple tuple;
    for (uint64_t i = start; i <= end; i++)
    {
        tuple = R->getTuples()[i];
        uint64_t byte = (tuple.getKey() >> current_byte) & 0xff;

        RR->setTupleVal(tmp[byte]++, tuple.getKey(), tuple.getPayload());
    }

    uint64_t nth_byte = current_byte/8; // switch R, RR after byte checked
    for (uint64_t i = 1; i < 256; i++)
    {
        if(Psum[i-1]-1 == end) break;
        if (!(Psum[i] - Psum[i-1])) continue;

        if ((Psum[i] - Psum[i-1]) * sizeof(Tuple) > L1_CACHESIZE)
        {
            if (!current_byte) {
//                Quicksort(RR->getTuples(), Psum[i - 1], Psum[i]-1);
                OptQuicksort(RR->getTuples(), Psum[i - 1], Psum[i]-1);

            }
            else
                Radixsort(RR, Psum[i - 1], Psum[i]-1, current_byte - 8, R);
        }
        else{
            if(Psum[i] > Psum[i-1])
//                Quicksort(RR->getTuples(), Psum[i-1], Psum[i]-1);
                OptQuicksort(RR->getTuples(), Psum[i-1], Psum[i]-1);

        }
    }

    if(end > Psum[255]) {
        if ((end - Psum[255]) * sizeof(Tuple) > L1_CACHESIZE) {
            if (!current_byte)
//            Quicksort(RR->getTuples(), Psum[255], end);
                OptQuicksort(RR->getTuples(), Psum[255], end);
            else
                Radixsort(RR, Psum[255], end, current_byte - 8, R);
        } else {
//            Quicksort(RR->getTuples(), Psum[255], end);
            OptQuicksort(RR->getTuples(), Psum[255], end);
        }
    }

//    uint64_t c = 0;
//    for (uint64_t i =start; i <end+1; i ++){
//        if(RR->getTuples()[i].getKey() == 18278284510384154391)
//            c++;
//    }
//    std::cout << c << std::endl;

   // if(nth_byte==6 or nth_byte==4 or nth_byte==2 or nth_byte==0) {
        R->copyTuplesVal(RR, start, end);
   // }


    if(nth_byte==7) {
        //R->copyTuplesVal(RR,start,end);
        delete RR;
    }
}

Tuple getMatrixSize(const char *fileName) {

    //Opening the input file
    FILE* fp;
    char buffer[SIZE + 1], lastchar = '\n';
    size_t bytes;
    int lines = 0;
    int columns = 0;
    fp = fopen(fileName,"r");     //Opening the file

    if (fp == NULL) {   //checking for error with fopen
        printf("here..\n");
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char c = getc(fp);
    char previous;
    while(c != '\n'){
        if(c == '|' or c == ',')
            columns++;
        previous = c;
        c = getc(fp);
    }
    if(previous <= '9' and previous >= '0')
        columns++;
    lines++;

    //A look to count the lines in our input file, which will also be the number of rows.
    while ((bytes = fread(buffer, 1, sizeof(buffer) - 1, fp))) {
        lastchar = buffer[bytes - 1];
        for (char *c = buffer; (c = (char*)(memchr(c, '\n', bytes - (c - buffer)))); c++) {
            lines++;
        }
    }
    if (lastchar != '\n')
        lines++;  /* Count the last line even if it lacks a newline */

    fclose(fp);

    Tuple tmp;
    tmp.setKey(columns);
    tmp.setPayload(lines);

    return tmp;
}

LinkedList* SortMergeJoin(Relation* relA, Relation* relB) {

    if(relA == nullptr or relB == nullptr){
        std::cout << "Relations can't be NULL!" << std::endl;
        return nullptr;
    }

    uint64_t sizeA = relA->getNumTuples();
    uint64_t sizeB = relB->getNumTuples();

   // relA->print();

//    uint64_t c = 0;
//    for (uint64_t i =0; i < sizeA; i ++){
//        if(relA->getTuples()[i].getKey() == 18278284510384154391)
//            c++;
//    }
//    std::cout << c << std::endl;

    Radixsort(relA,0,sizeA-1);
    Radixsort(relB,0,sizeB-1);


    Tuple* tupA = relA->getTuples();
    Tuple* tupB = relB->getTuples();

//    c = 0;
//    for (uint64_t i =0; i < sizeA; i ++){
//        if(tupA[i].getKey() == 18278284510384154391)
//            c++;
//    }
//    std::cout << c << std::endl;
    if (!relA->isSorted() || !relB->isSorted())
        return nullptr;

    if(tupA == nullptr or tupB == nullptr)
        return nullptr;

    int j=0;
    int jj=0;
    int flag = false;
    uint64_t counter = 0;

    LinkedList *Results = new LinkedList(BUFFERSIZE);
    for(uint64_t i = 0; i<sizeA; i++){

        if(tupA[i].getKey() == tupB[j].getKey()){
            //Results->insert(tupA[i].getPayload(), tupB[j].getPayload());
            counter++;

            while(tupA[i].getKey() == tupB[++j].getKey()){
               // Results->insert(tupA[i].getPayload(), tupB[j].getPayload());
                counter++;
                if(j == sizeB-1) break;
            }
            j = jj;
        }
        else if(tupA[i].getKey() > tupB[j].getKey()){


            while(tupA[i].getKey() > tupB[++j].getKey()){
                if (j == sizeB-1) {
                    flag = true;
                    break;
                }
            }

            if (j == sizeB-1) {
                break;
            }
            if(flag) break;
            jj = j--;
            while(tupA[i].getKey() == tupB[++j].getKey()){
                //Results->insert(tupA[i].getPayload(), tupB[j].getPayload());
                counter++;
            }
            j = jj;
        }
    }
    std::cout << "Number of tuples after join: " << counter << std::endl;
    return Results;
}

void clean_up(Matrix **matrix1, Matrix **matrix2,
              Relation **R1, Relation **R2, LinkedList **ResultsList)
{
    delete *matrix1; *matrix1 = nullptr;
    delete *matrix2; *matrix2 = nullptr;
    delete *R1; *R1 = nullptr;
    delete *R2;  *R2 = nullptr;
    delete *ResultsList; *ResultsList = nullptr;
}

uint64_t mcg64(void)
{
    static uint64_t i = 1;
    return (i = (164603309694725029ull * i) % 14738995463583502973ull);
}