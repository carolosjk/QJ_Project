//
// Created by karolos on 26/11/19.
//

#include <iostream>
#include <cstring>
#include "Datatypes_part2.h"
#include "MACROS.h"

Predicate::Predicate()
{
    operation = '\0';

    Matrices[0] = Matrices[1] = -1;
    RowIds[0] = RowIds[1] = -1;

    filter = 0;
}

Query::Query()
{
    NumOfMatrices = NumOfPredicates = NumOfResults = 0;
    Matrices = nullptr;
    Results = nullptr;
    Predicates = nullptr;
}

inline void parse_err()
{
    std::cerr << "Invalid inq to parser" << std::endl;
    exit(EXIT_FAILURE);
}

void Query::parse(char *inq)
{
    char* matrices = strtok(inq,"|");
    char* predicates = strtok(nullptr,"|");
    char* results = strtok(nullptr,"");

//  calc num of matrices
    for(int i = 1; i < strlen(matrices)+1; i++){
        if((matrices[i-1] < '0' or matrices[i-1] > '9') and matrices[i-1] != ' ') parse_err();
        if(matrices[i-1] >= '0' and matrices[i-1] <= '9' and (matrices[i] == '\0' or matrices[i] == ' '))
            NumOfMatrices++;
    }
//    alloc Matrices and set
    Matrices = new int[NumOfMatrices];
    ALLOC_CHECK(Matrices);

    char* tmp = strtok(matrices," ");
    Matrices[0] = atoi(tmp);
    for(int i =1; i < NumOfMatrices; i++){
        tmp = strtok(nullptr," ");
        Matrices[i] = atoi(tmp);
        if(Matrices[i] < 0) parse_err();
        else if(Matrices[i] == 0 and strcmp(tmp,"0")!=0) parse_err();
    }

//    calc num of results
    for(int i = 1; i < strlen(results)+1; i++){
        if((results[i-1] < '0' or results[i-1] > '9') and results[i-1] != ' ' and results[i-1] != '.') parse_err();
        if(results[i-1] >= '0' and results[i-1] <= '9' and (results[i] == '\0' or results[i] == ' '))
            NumOfResults++;
    }
//    alloc Results and set
    Results= new double[NumOfResults];
    ALLOC_CHECK(Results);

    tmp = strtok(results," ");
    Results[0] = atof(tmp);
    for(int i =1; i < NumOfResults; i++){
        tmp = strtok(nullptr," ");
        Results[i] = atof(tmp);
        if(Results[i] < 0) parse_err();
    }

//    calc num of predicates
    for(int i = 1; i < strlen(predicates)+1; i++){
        // Looking for illegal characters in the predicates
        if((predicates[i-1] < '0' or predicates[i-1] > '9') and predicates[i-1] != '>' and predicates[i-1] != '<'
           and predicates[i-1] != '=' and predicates[i-1] != '&' and predicates[i-1] != '.') parse_err();
        // Counting the predicates
        if(predicates[i-1] >= '0' and predicates[i-1] <= '9' and (predicates[i] == '\0' or predicates[i] == '&'))
            NumOfPredicates++;
    }

    Predicates = new Predicate[NumOfPredicates];
    ALLOC_CHECK(Predicates);
    char* strArray[NumOfPredicates];

    strArray[0] = strtok(predicates,"&"); // reading the 1st predicate
    for(int i =1; i< NumOfPredicates; i++)
        strArray[i] = strtok(nullptr,"&");

    for(int j =0; j<NumOfPredicates;j++) {
        char operation = 'a';
        bool operationFound = false;

        tmp = strtok(strArray[j],"&");

        for(int i =0; i < strlen(tmp);i++) {
            //Looking for the operation symbol '>' or '<' or '='
            if (tmp[i - 1] == '>' or tmp[i - 1] == '=' or tmp[i - 1] == '<') {
//???
                if (operation == 'a') {  //checking for more than 1 ope
                    operation = tmp[i - 1];
                    operationFound = true;
                } else parse_err();
            }

            if (operationFound and tmp[i - 1] == '.')
                operation = 'j';
        }

//???
        char del[2] = {operation};
        if(del[0] == 'j')
            del[0] = '=';

        tmp = strtok(tmp,".");
//        in predicates as given when there is <rel>.<relcolmn>, <rel> is index of rel in Matrices[], in class Query, given .pdf file of second part of project
        Predicates[j].Matrices[0] = Matrices[atoi(tmp)];
        tmp = strtok(nullptr,del);
        Predicates[j].RowIds[0] = atoi(tmp);
        if(operation == 'j') {
            tmp = strtok(nullptr,".");
            Predicates[j].Matrices[1] = Matrices[atoi(tmp)];
            tmp = strtok(nullptr, "\0");
            Predicates[j].RowIds[1] = atoi(tmp);
        }else{
            tmp = strtok(nullptr, "\0");
            Predicates[j].filter = atoll(tmp);
        }
        if (operation == 'j') Predicates[j].operation = '=';
        else Predicates[j].operation = operation;

    }
}

int Query::exec()
{
//    check for matrix in array of matrices
    for (int i = 0; i < NumOfMatrices; i++)
        if (Matrices[i] > MATRICES_SIZE)
        {
            std::cerr << "Matrix " << Matrices[i] << std::endl;
            std::cerr << "No such Matrix object in array of Matrices." << std::endl;
            exit(EXIT_FAILURE);
        }

//    TO DO ...

}