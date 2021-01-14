#include <iostream>
#include <string>
#include <ctime>
#include"../header/simplex.h"
using namespace std;

int main() {
    int testbench_sel;
    cout << "Select testbench :" << endl;
    cout << "1 : AFIRO" << endl;
    cout << "2 : SC105" << endl;
    cout << "3 : STOCFOR1" << endl;
    cout << "4 : SCAGR7" << endl;
    cout << "5 : LOTFI" << endl;
    cout << "6 : Largest Testbench MAROS-R7" << endl;   // replace XXX with the testbench name
    cin >> testbench_sel;

    string testbench_name;
    if (testbench_sel == 1){
        testbench_name = "AFIRO";
    }
    else if (testbench_sel == 2){
        testbench_name = "SC105";
    }
    else if (testbench_sel == 3){
        testbench_name = "STOCFOR1";
    }
    else if (testbench_sel == 4){
        testbench_name = "SCAGR7";
    }
    else if (testbench_sel == 5){
        testbench_name = "LOTFI";
    }
    else if (testbench_sel == 6){
        testbench_name = "MAROS-R7";                    // replace XXX with the testbench name
    }
    else{
        cout << "Please enter valid selection!" << endl;
        return 0;
    }

    clock_t  time_start, time_end;
    time_start = clock();

    cout << "start solving " << testbench_name << endl;
    testbench_name = testbench_name+".mps";
    Simplex *s = new Simplex();
    s->simpex_initinalize_from_parser(testbench_name);
    double result;
    result = s->run();

    time_end = clock();

    cout << "Result : " << scientific <<result << endl;          //replace "XXX" with the final result
    cout<<"-----End-----"<<endl;
    cout << "Runtime : " << (double(time_end - time_start)/CLOCKS_PER_SEC) << " second" << endl;

    return 0;
}