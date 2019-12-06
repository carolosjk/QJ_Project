//
// Created by karolos on 26/11/19.
//

#include <iostream>
#include <cstring>
#include <cmath>
#include "Datatypes_part2.h"
#include "MACROS.h"
#include "../project_jj_lib/Utils.h"

Predicate::Predicate() {
    operation = '\0';

    Matrices[0] = Matrices[1] = -1;
    RowIds[0] = RowIds[1] = -1;

    filter = 0;
}

char Predicate::getOperation() {
    return operation;
}

Query::Query() {
    NumOfMatrices = NumOfPredicates = NumOfResults = 0;
    Matrices = nullptr;
    Results = nullptr;
    Predicates = nullptr;
    FilteredMatrices[0] = FilteredMatrices[1] = FilteredMatrices[2] = FilteredMatrices[3] = nullptr;
    ListOfResults = nullptr;
    MatricesJoined = nullptr;
}

inline void parse_err() {
    std::cerr << "Invalid inq to parser" << std::endl;
    exit(EXIT_FAILURE);
}

void Query::parse(char *inq) {
    char *matrices = strtok(inq, "|");
    char *predicates = strtok(nullptr, "|");
    char *results = strtok(nullptr, "");

//  calc num of matrices
    for (int i = 1; i < strlen(matrices) + 1; i++) {
        if ((matrices[i - 1] < '0' or matrices[i - 1] > '9') and matrices[i - 1] != ' ') parse_err();
        if (matrices[i - 1] >= '0' and matrices[i - 1] <= '9' and (matrices[i] == '\0' or matrices[i] == ' '))
            NumOfMatrices++;
    }
//    alloc Matrices and set
    Matrices = new int[NumOfMatrices];

    char *tmp = strtok(matrices, " ");
    Matrices[0] = atoi(tmp);
    for (int i = 1; i < NumOfMatrices; i++) {
        tmp = strtok(nullptr, " ");
        Matrices[i] = atoi(tmp);
        if (Matrices[i] < 0) parse_err();
        else if (Matrices[i] == 0 and strcmp(tmp, "0") != 0) parse_err();
    }

//    calc num of results
    for (int i = 1; i < strlen(results) + 1; i++) {
        if ((results[i - 1] < '0' or results[i - 1] > '9') and results[i - 1] != ' ' and
            results[i - 1] != '.')
            parse_err();
        if (results[i - 1] >= '0' and results[i - 1] <= '9' and (results[i] == '\0' or results[i] == ' '))
            NumOfResults++;
    }
//    alloc Results and set
    Results = new double[NumOfResults];

    tmp = strtok(results, " ");
    Results[0] = atof(tmp);
    for (int i = 1; i < NumOfResults; i++) {
        tmp = strtok(nullptr, " ");
        Results[i] = atof(tmp);
        if (Results[i] < 0) parse_err();
    }

//    calc num of predicates
    for (int i = 1; i < strlen(predicates) + 1; i++) {
        // Looking for illegal characters in the predicates
        if ((predicates[i - 1] < '0' or predicates[i - 1] > '9') and predicates[i - 1] != '>' and
            predicates[i - 1] != '<'
            and predicates[i - 1] != '=' and predicates[i - 1] != '&' and predicates[i - 1] != '.')
            parse_err();
        // Counting the predicates
        if (predicates[i - 1] >= '0' and predicates[i - 1] <= '9' and (predicates[i] == '\0' or predicates[i] == '&'))
            NumOfPredicates++;
    }

    Predicates = new Predicate[NumOfPredicates];
    char *strArray[NumOfPredicates];

    strArray[0] = strtok(predicates, "&"); // reading the 1st predicate
    for (int i = 1; i < NumOfPredicates; i++)
        strArray[i] = strtok(nullptr, "&");

    for (int j = 0; j < NumOfPredicates; j++) {
        char operation = 'a';
        bool operationFound = false;

        tmp = strtok(strArray[j], "&");

        for (int i = 0; i < strlen(tmp); i++) {
            //Looking for the operation symbol '>' or '<' or '='
            if (tmp[i - 1] == '>' or tmp[i - 1] == '=' or tmp[i - 1] == '<') {
                //More than 1 operation symbol is not allowed within the same predicate
                if (operation == 'a') {  //checking for more than 1 ope
                    operation = tmp[i - 1];
                    operationFound = true;
                } else parse_err();
            }

            if (operationFound and tmp[i - 1] == '.')
                operation = 'j';
        }

        char del[2] = {operation};
        if (del[0] == 'j')
            del[0] = '=';

        tmp = strtok(tmp, ".");
//        in predicates as given when there is <rel>.<relcolmn>, <rel> is index of rel in Matrices[], in class Query, given .pdf file of second part of project
        Predicates[j].Matrices[0] = Matrices[atoi(tmp)];
        tmp = strtok(nullptr, del);
        Predicates[j].RowIds[0] = atoi(tmp);
        if (operation == 'j') {
            tmp = strtok(nullptr, ".");
            Predicates[j].Matrices[1] = Matrices[atoi(tmp)];
            tmp = strtok(nullptr, "\0");
            Predicates[j].RowIds[1] = atoi(tmp);
        } else {
            tmp = strtok(nullptr, "\0");
            Predicates[j].filter = atoll(tmp);
        }
        Predicates[j].operation = operation;
    }
}

bool Query::filtering(uint64_t &size) {
//    calc num of filters
    uint64_t v = 0;
    for (uint64_t i = 0; i < NumOfPredicates; i++)
        if (Predicates[i].filter) v++;

    for (uint64_t i = 0; i < NumOfPredicates; i++)
        if (Predicates[i].filter) {
            Relation *rel;

            rel = MATRICES[Predicates[i].Matrices[0]].getRelation(Predicates[i].RowIds[0]);
            if (rel == nullptr)
                return false;

            auto vector = new Vector();

            Tuple *tuples = rel->getTuples();
            uint64_t numOfTuples = rel->numTuples;
            switch (Predicates[i].operation) {
                case '>':
                    for (uint64_t j = 0; j < numOfTuples; j++)
                        if (tuples[j].key > Predicates[i].filter)
                            vector->push_back(tuples[j].getPayloads()[0]);
                    break;
                case '<':
                    for (uint64_t j = 0; j < rel->numTuples; j++)
                        if (tuples[j].key < Predicates[i].filter)
                            vector->push_back(tuples[j].getPayloads()[0]);
                    break;
                case '=':
                    for (uint64_t j = 0; j < rel->numTuples; j++)
                        if (tuples[j].key == Predicates[i].filter)
                            vector->push_back(tuples[j].getPayloads()[0]);
                    break;
                default:
                    std::cout << "Invalid operation for filtering!" << std::endl;
                    return false;
            }

            for (int x = 0; x < NumOfMatrices; x++)
                if (Matrices[x] == Predicates[i].Matrices[0])
                    FilteredMatrices[x] = vector;
               // std::cout << "vector size: " << vector->size() << std::endl;
        }

    size = v;
    return true;
}

void Query::exec() {
//    start with filtering query
    uint64_t f = 0;
    bool filters = filtering(f);
    if (!filters) {
        log("Exec: No filters\n");
        empty_sum();
        return;
    }

    //Then we execute the joins
    for (int i = 0; i < NumOfPredicates; i++) {
        char operation = Predicates[i].getOperation();
        if (operation != 'j')  //not a join operation
            continue;

        Relation *R1, *R2;
        //Get the first relation of the predicate, filter it and sort it
        int index = -1;
        if(MatricesJoined != nullptr)
            index = MatricesJoined->getIndex(Predicates[i].Matrices[0]);
        if(index == -1) { //matrix not in Results structure
            R1 = MATRICES[Predicates[i].Matrices[0]].getRelation(Predicates[i].RowIds[0]);
            for (int j = 0; j < NumOfMatrices; j++) {
                if (Predicates[i].Matrices[0] == Matrices[j]) {
                    R1->filter(FilteredMatrices[j]);
                    break;
                }
            }
            if (!R1->numTuples) {
                //log("Exec: Empty filtered rel1\n");
                empty_sum();
                return;
            }
        }
        else
            R1 = MATRICES[Predicates[i].Matrices[0]].getRelation(ListOfResults,index,rowsInResults,Predicates[i].RowIds[0]);

        //Same thing for the second relation of the predicate
        index = -1;
        if(MatricesJoined != nullptr)
            index = MatricesJoined->getIndex(Predicates[i].Matrices[1]);
        if(index == -1) { //matrix not in Results structure
            R2 = MATRICES[Predicates[i].Matrices[1]].getRelation(Predicates[i].RowIds[1]);
            for (int j = 0; j < NumOfMatrices; j++) {
                if (Predicates[i].Matrices[1] == Matrices[j]) {
                    R2->filter(FilteredMatrices[j]);
                    break;
                }
            }
            if (!R2->numTuples) {
                //log("Exec: Empty filtered rel2\n");
                empty_sum();
                return;
            }
        }
        else
            R2 = MATRICES[Predicates[i].Matrices[1]].getRelation(ListOfResults,index,rowsInResults,Predicates[i].RowIds[1]);

        if(R1 == nullptr or R2 == nullptr){
            log("Exec: No rel1||re2\n");
            empty_sum();
            return;
        }


        if(MatricesJoined==nullptr){
            MatricesJoined = new Vector();
            MatricesJoined->push_back(Predicates[i].Matrices[0]);
            MatricesJoined->push_back(Predicates[i].Matrices[1]);
            ListOfResults = join(R1, R2);
        }
        else if ( MatricesJoined->search(Predicates[i].Matrices[0])) {
            delete ListOfResults;
            MatricesJoined->push_back(Predicates[i].Matrices[1]);
            //Save the joined result in an array
            ListOfResults = join(R1, R2);
        } else {
            delete ListOfResults;
            MatricesJoined->push_back(Predicates[i].Matrices[0]);
            ListOfResults = join(R2, R1);
        }
        if(ListOfResults == nullptr){
            //log("Exec: Empty results\n");
            empty_sum();
            return;
        }
        //MatricesJoined->print();
       // std:: cout << rowsInResults << std:: endl;
    }

    calc_sum();
}


List* Query::join(Relation *relA, Relation *relB) {

    Radixsort(relA,0,relA->numTuples-1);
    Radixsort(relB,0,relB->numTuples-1);

    if (!relA->isSorted() || !relB->isSorted())
    {
        log("Join: No sorted rels\n");
        return nullptr;
    }

    Tuple* tupA = relA->getTuples();
    Tuple* tupB = relB->getTuples();

    if(tupA == nullptr or tupB == nullptr)

    {
        log("Join: Empty tuples\n");
        return nullptr;
    }

    uint64_t sizeA = relA->numTuples;
    uint64_t sizeB = relB->numTuples;
    uint64_t j=0;
    uint64_t jj=0;
    bool flag = false;
    uint64_t counter = 0;
    struct Node* N;

    List* results = new List();
    for(uint64_t i = 0; i<sizeA; i++){

        if(tupA[i].key == tupB[j].key){
            N = results->insert_node();
            for(int x =0; x < (int) tupA[i].payloads.size(); x++)
                results->insert(N,tupA[i].payloads[x]);
            results->insert(N,tupB[j].payloads[0]);
            counter++;

            if(j == sizeB-1) continue;
            jj = j;
            while(tupA[i].key == tupB[++j].key){
                N = results->insert_node();
                for(int x =0; x < (int) tupA[i].payloads.size(); x++)
                    results->insert(N,tupA[i].payloads[x]);
                results->insert(N,tupB[j].payloads[0]);
                counter++;
                if(j == sizeB-1) break;
            }
            j = jj;
        }
        else if(tupA[i].key > tupB[j].key){

            if(j == sizeB-1) break;
            while(tupA[i].key > tupB[++j].key){
                if (j == sizeB-1) {
                    flag = true;
                    break;
                }
            }
            if(flag) break;
            jj = j--;
            while(tupA[i].key == tupB[++j].key){
                N = results->insert_node();
                for(int x =0; x < (int) tupA[i].payloads.size(); x++)
                    results->insert(N,tupA[i].payloads[x]);
                results->insert(N,tupB[j].payloads[0]);
                counter++;
                if (j == sizeB-1) {
                    break;
                }
            }
            j = jj;
        }
    }
    if(counter==0)
    {
        log("Join: No joins\n");
        return nullptr;
    }
    rowsInResults = counter;
    return results;
}

int fracto_int(double number, int dec_num) {
    double dummy;
    double frac = modf(number, &dummy);
    return round(frac * pow(10, dec_num));
}

void Query::empty_sum() {
    for (uint64_t i = 0; i < NumOfResults; i++)
        std::cout << "NULL ";
    std::cout << std::endl;
}

void Query::calc_sum() {
    Vector sum;
    Tuple *data;
    for (uint64_t i = 0; i < NumOfResults; i++) {
        uint64_t s = 0;
        double frack, intprt;
        int x, y;
        frack = modf(Results[i], &intprt);
        x = (int) intprt;
        y = fracto_int(frack, 1);

        if (!MatricesJoined->search(Matrices[x])) {
            log("Calculation: No such relation in joined ones!\n");
            return;
        }

        int indx = MatricesJoined->getIndex(Matrices[x]);
        if (indx != -1) {
            Relation *rel = MATRICES[Matrices[x]].getRelation(y);
            data = rel->getTuples();
            for (struct Node *h = ListOfResults->getHead(); h != nullptr; h = h->next) {
                if (h->data[indx] > rel->numTuples) {
                    log("Calculation: RowId OUT OF BOUNDS\n");
                    return;
                }
                s += data[h->data[indx]].key;
            }
            sum.push_back(s);
        } else {
            log("Calculation: No such relation in List object\n");
            return;
        }
    }
    delete ListOfResults;

    for (uint64_t i = 0; i < sum.size(); i++)
        std::cout << sum[i] << " ";

    std::cout << std::endl;
}
