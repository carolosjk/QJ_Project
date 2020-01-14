//#include <iostream>
//#include <climits>
//#include <cstring>
//#include "project_jj_lib/Utils.h"
//#include "project_jj_lib_part2/Utils_part2.h"
//#include "project_jj_lib_part2/MACROS.h"
//
//using namespace std;
//
//int main(int argc, char **argv)
//{
//    Get_Input();
//    Set_output();
//
//    return 0;
//}

#include <cstdlib>
#include "project_jj_lib_part3/JobScheduler.h"


int main(int argc, char **argv)
{
    if (argc < 2) {
        s_cout{} << "give #threads...\n";
        exit(1);
    }
    JobScheduler sched;
    sched.init(atoi(argv[1]));

    Job *t0 = new job{0};
    Job *t1 = new job{1};
    Job *t2 = new job{2};
    Job *t3 = new job{3};
    Job *t4 = new job{4};
    Job *t5 = new job{5};

    sched.schedule(*t0);
    sched.schedule(*t1);
    sched.schedule(*t2);
    sched.schedule(*t3);
    sched.schedule(*t4);
    sched.schedule(*t5);

    // sleep(10);
    sched.stop();
}