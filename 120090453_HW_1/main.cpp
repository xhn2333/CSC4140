#include<iostream>
#include<cstring>
#include "src/q2-1.cpp"
#include "src/q2-2.cpp"
#include "src/q2-3.cpp"
#include "src/q2-4.cpp"

using namespace std;

int main(int args, char* argv[]){
    int num=0;
    if(!strcmp(argv[1],"2-1")) num=q2_1(argv);
    else if(!strcmp(argv[1],"2-2")) num=q2_2(argv);
    else if(!strcmp(argv[1],"2-3")) num=q2_3(argv);
    else if(!strcmp(argv[1],"2-4")) num=q2_4(argv);
    else num=-1;
}