#ifndef SIMPLEX_H
#define SIMPLEX_H
#include<iostream>
#include<fstream>
#include<vector>
#include<map>
#include<set>
#include <utility>
#include <math.h>
#include <string>
#include <sstream>
#define NDEBUG
#include <assert.h>
#include <iomanip>

#define ZERO_ERROR 10
#define MAX_ITERATION 10000
//Original
// #define OBJ_COFF_ERROR -0.1e-5
// #define CONSTRAINT_COFF_ERROR 0.78e-7
// #define RHS_ERROR 1e-8

// #define OBJ_COFF_ERROR -0.1e-5
// #define CONSTRAINT_COFF_ERROR 2e-7
// #define RHS_ERROR 1e-8

//10^-8
// 0.000000001 
using namespace std;

typedef map<string , int> TABLE;

typedef map<pair<int,int> , double> MATRIX_REG;//store coordinate and value
// <pair<row,col> , float>

class Simplex{
    private:
        int row_size;
        int col_size;
        int ori_col_size;//original col size
        int slack_size;
        int artifical_size;
        // int ori_row_size;

        vector<vector<double> > matrix;//row x col
        map<int , char> operator_map; //record the operator of each row

        //the col is BV!!
        map<int,int>basic_variable_map;//each row has a basic variable
        string obj_name;


        // set<int> pivot_row_list;
        // set<int> pivot_row_used_list;


        TABLE col_map;
        TABLE row_map;

        double solution;

        //record the best solution

    public:
        friend ostream& operator<<(ostream& os , const Simplex& obj );
        Simplex();
        void simpex_initinalize_from_parser(string filename);



        /*for debugging*/
        void show_matrix();
        void show_slack_matrix();
        void show_artifical_matrix();
        void output_matrix(string );

        void test();
        double run();
        bool simplex_solver();
        bool is_optimal();
        int find_pivot_col();
        int find_pivot_row(int col);
        void gaussian_elimination(int row , int col);
        void phase1_legalize_obj();
        void phase2_legalize_obj();
        void erase_artifical();
        void clear_zero_error();
        
        void error_correction();

        void check_sumof_pivot_col_is_one(int col);
        

};




void show_row(vector<double>);

#endif //SIMPLEX_H
