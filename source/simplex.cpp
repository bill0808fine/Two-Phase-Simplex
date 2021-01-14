#include"../header/simplex.h"
#include<vector>

// #define OBJ_COFF_ERROR -0.1e-5
// #define CONSTRAINT_COFF_ERROR 0.78e-7
// #define RHS_ERROR 1e-8


//Current Best
#define OBJ_COFF_ERROR -1e-6
#define RHS_ERROR 1e-5

#define CONSTRAINT_COFF_ERROR 1e-9
#define OPTIMAL_CONDITION -8.06e-4

Simplex::Simplex(){
    ori_col_size=0;
    row_size = 0;
    col_size = 0;
    slack_size = 0;
    artifical_size = 0;
    solution = 0;
}

void Simplex::test(){
    cout<<"----test----"<<endl;
    erase_artifical();
    output_matrix("./testbench/test.txt");
    if(!simplex_solver()){
        cout<<"Phase 1 failure"<<endl;
        cout<<"There is no feasible soultion"<<endl;
        return;
    };//false 無解

    solution = matrix.at(0).at(col_size-1);
    cout<<solution<<endl;
}

double Simplex::run(){
    //store obj function() and alter to min sun of artifical
    // cout<<"----- run -----"<<endl;

    vector<double> temp_obj(matrix.at(0));
    vector<double> new_obj(ori_col_size+slack_size,0);
    new_obj.reserve(col_size);
    new_obj.resize(col_size-1,1);//cofficient of artifical is 1
    new_obj.push_back(0);
    assert(temp_obj.size() == new_obj.size());

    matrix.at(0) = new_obj;



    // output_matrix("./testbench/matrix_obj.txt");



    //run phase 1
    cout<<"-----Phase 1-----"<<endl;
    phase1_legalize_obj();


    //assertion
    /*
    for(map<int, int >::iterator iter_map = basic_variable_map.begin() ; iter_map!=basic_variable_map.end() ; iter_map++){
        assert(matrix.at(iter_map->first).at(iter_map->second)==1 );
    }
    */


    // output_matrix("./testbench/matrix_afterlegal.txt");

    if(!simplex_solver()){
        cout<<"Phase 1 failure"<<endl;
        cout<<"There is no feasible soultion"<<endl;
        return 0 ;
    };//false 無解



    // output_matrix("./testbench/after_phase1.txt");

    solution = matrix.at(0).at(col_size-1);
    cout<<"Phase 1 solution is "<<solution<<endl;
    if(fabs(solution)>ZERO_ERROR){
        cout<<"Notice! The sol isn't 0"<<endl;
        cout<<"There is no feasible soultion"<<endl;
        return 0 ;
    }

    //if exist soultion then must satisfy the following assertion
    /* Assertion
    for(map<int,int>::iterator iter_map = basic_variable_map.begin() ; iter_map!=basic_variable_map.end() ; iter_map++){
        assert( iter_map->second < ori_col_size+slack_size);
    }
    */



    cout<<"-----Phase 2-----"<<endl;
    //change to original obj
    matrix.at(0) = temp_obj;

    //remove artifical variable
    erase_artifical();
    // clear_zero_error();


    //run phase 2
    // legalize_obj();

    phase2_legalize_obj();
    // output_matrix("./testbench/phase2_legal.txt");

    if(!simplex_solver()){
        cout<<"Phase 2 failure"<<endl;
        cout<<"There is no feasible soultion"<<endl;
        return 0;
    };//false 無解
    // output_matrix("./testbench/final_matrix.txt");
    solution = matrix.at(0).at(col_size-1) * (-1);
    // cout<<"Final Solution : "<<scientific <<solution<<endl;
    cout<<"-----End-----"<<endl;
    return solution;
}

bool Simplex::simplex_solver(){
    //check whether the table is optimal
    //if in first run is optimal , need I check matrix?
    int pivot_col = 0;
    int pivot_row = 0;
    int solver_count = 0;
    while(!is_optimal()){

        pivot_col = find_pivot_col();
        pivot_row = find_pivot_row(pivot_col);

        // cout<<"col&row :"<<pivot_col <<" "<<pivot_row<<endl;
        if(pivot_row==-1){
            cout<<"The problem is unbounded"<<endl;
            return false;
        }
        gaussian_elimination(pivot_row ,pivot_col);
        error_correction();
        // check_sumof_pivot_col_is_one(pivot_col);
        //turn the pivot col into BV !
        basic_variable_map[pivot_row] = pivot_col;
		// cout<< matrix[0][col_size-1]*(-1)<<endl;
        solver_count++;

    }
    cout<<"Iteration times : "<<solver_count<<endl;
    // cout<<"solve count : "<<solver_count<<endl;
    // cout<<"Max : is "<<max<<endl;

    return true;
}

bool Simplex::is_optimal(){
    for(int i = 0 ; i <col_size-1 ; i++){
        //Exist a negative value
        //if(matrix.at(0).at(i) < 0){
        if(matrix.at(0).at(i) < OPTIMAL_CONDITION){
            return false;
        }
    }
    return true;
}

int Simplex::find_pivot_col(){
    int index = 0;
    double min = matrix.at(0).at(0);

    //find the minimum value in obj function (din't contain RHS -> until col_size-1)
    for(int i = 1 ; i < col_size-1 ; i++){

        if( matrix.at(0).at(i) < min ){
            min = matrix.at(0).at(i);
            index = i;
        }
    }
    //check min is negative
    assert(min<0);

    return index;
}

void Simplex::phase1_legalize_obj(){
    for(vector<vector<double> >::iterator iter_list = matrix.begin()+1 ; iter_list!=matrix.end() ; iter_list++ ){
        for(int i = 0 ; i < col_size ; i++){
            matrix.at(0).at(i) -= (*iter_list).at(i);
        }
    }
}

void Simplex::phase2_legalize_obj(){
    int BS_col =0 ;
    double mult = 0 ;
    for(int i = 1 ; i < row_size ; i++){
        //when the cofficient of BV in obj isn't 0
        BS_col = basic_variable_map[i];
        if(matrix.at(0).at(BS_col) != 0  ){
            mult = matrix.at(0).at(BS_col);
            for(int j = 0 ; j < col_size ; j++){
                matrix[0][j] = matrix[0][j] - matrix[i][j] * mult ;
            }
            assert(matrix[0][BS_col]==0 );
        }
    }
}


//Choosing the row which never is chose in advance.
//turn on the declaration!!!
/*
int Simplex::find_pivot_row(int col){
    double min = 99999999999999;
    double ratio = 0;
    int index = -1;
    double max_col = -99999999;
    //from row 1
    for(set<int>::iterator row_index=pivot_row_list.begin() ; row_index != pivot_row_list.end() ; row_index++ ){
        if(matrix.at(*row_index).at(col) > 0 ){
            ratio = (*(matrix.at(*row_index).end()-1)) / matrix.at(*row_index).at(col);
            if(ratio < min){
                index = *row_index;
                min =ratio;
                max_col = matrix.at(*row_index).at(col);
            }else if(ratio==0 && matrix.at(*row_index).at(col) > max_col){
                index = *row_index;
                min =ratio;
                max_col = matrix.at(*row_index).at(col);
            }
        }
    }

    if(index==-1){
        //repeat row
        for(set<int>::iterator row_index=pivot_row_used_list.begin() ; row_index != pivot_row_used_list.end() ; row_index++ ){
            if(matrix.at(*row_index).at(col) > 0 ){
                ratio = (*(matrix.at(*row_index).end()-1)) / matrix.at(*row_index).at(col);
                if(ratio < min){
                    index = *row_index;
                    min =ratio;
                    max_col = matrix.at(*row_index).at(col);
                }else if(ratio==0 && matrix.at(*row_index).at(col) > max_col){
                    index = *row_index;
                    min =ratio;
                    max_col = matrix.at(*row_index).at(col);
                }
            }
        }

    }else{
        //find a row which is choose firstly
        pivot_row_used_list.insert(index);
        pivot_row_list.erase(index);
    }


    return index;
}
*/




int Simplex::find_pivot_row(int col){
    double min = 99999999999999;
    double ratio = 0;
    int index = -1;
    double max_col = -99999999;
    //from row 1
    for(int i = 1 ; i <row_size ; i++){
        if(matrix.at(i).at(col) > 0 ){
        // if(matrix.at(i).at(col) > -10e-15 ){
            ratio = (*(matrix.at(i).end()-1)) / matrix.at(i).at(col);
            if(ratio < min){
                index = i;
                min =ratio;
                max_col = matrix.at(i).at(col);
            }else if(ratio==0 && matrix.at(i).at(col) > max_col){
                //if ratio==0 then find the max col (RHS/col)
                index = i;
                min =ratio;
                max_col = matrix.at(i).at(col);
            }
        }
    }
    return index;
}


// int Simplex::find_pivot_row(int col){
//     double min = 99999999999999;
//     double ratio = 0;
//     int index = -1;

//     //from row 1
//     for(int i = 1 ; i <row_size ; i++){
//         if(matrix.at(i).at(col) > 0 ){
//         // if(matrix.at(i).at(col) > -10e-15 ){
//             ratio = (*(matrix.at(i).end()-1)) / matrix.at(i).at(col);
//             if(ratio < min){
//                 index = i;
//                 min =ratio;
//             }
//         }
//     }

//     // cout<<"pitvow row is "<<index<<endl;
//     // cout<<"ratio : "<<ratio<<endl;

//     return index;
// }



void Simplex::gaussian_elimination(int pivot_row ,int pivot_col){
    double pivot_element = matrix.at(pivot_row).at(pivot_col);
    if(pivot_element!=1){
        for(vector<double>::iterator iter_value = matrix.at(pivot_row).begin() ; iter_value != matrix.at(pivot_row).end() ; iter_value++){
            (*iter_value) = (*iter_value)/pivot_element;
        }
    }
    assert(matrix.at(pivot_row).at(pivot_col)==1);
    double mult = 0;
    for(int i = 0 ; i < row_size ; i++){
        if(i != pivot_row && matrix.at(i).at(pivot_col)!=0 ){
            mult = (matrix.at(i).at(pivot_col));
            for(int j = 0 ; j < col_size ; j++){
                matrix.at(i).at(j) = matrix.at(i).at(j) - (matrix.at(pivot_row).at(j)) *mult ;
            }
            assert(matrix.at(i).at(pivot_col)==0);
        }
    }
}

void Simplex::check_sumof_pivot_col_is_one(int col){
    double sum =0;
    for(vector<vector<double> >::iterator iter_row = matrix.begin() ; iter_row != matrix.end() ; iter_row++){
        sum+=(*iter_row).at(col);
    }
    // cout<<sum<<endl;
    assert(sum == 1);
}


void Simplex::erase_artifical(){
    for(vector<vector<double> >::iterator iter = matrix.begin() ; iter!=matrix.end() ; iter++ ){
        (*iter).erase((*iter).begin()+ori_col_size+slack_size ,(*iter).end()-1 );
    }
    col_size = (int)matrix.at(0).size();
}

void Simplex::clear_zero_error(){
    /*
        The cofficient in matrix may have zero error,
        it will lead to the problem while chossing the pivot row (pivot only choose the value > 0)
    */
    for(int i = 1 ; i < row_size ; i++){
        for(int j = 0 ; j < col_size-1 ; j++){
            if( fabs(matrix.at(i).at(j) )< ZERO_ERROR){
                matrix.at(i).at(j) = 0;
            }
        }
    }
}


/*
    The cofficient in matrix may have zero error,
    it will lead to the problem while chossing the pivot row (pivot only choose the value > 0)
*/

void Simplex::error_correction(){
    /*
	for(int j = 0 ; j < col_size-1 ; j++){
        if( matrix.at(0).at(j)<0 && matrix.at(0).at(j) >  OBJ_COFF_ERROR ){
            matrix.at(0).at(j) = 0;
        }
    }
	*/
    for(int i = 1 ; i < row_size ; i++){
        for(int j = 0 ; j < col_size-1 ; j++){
            if( matrix.at(i).at(j)>0 && matrix.at(i).at(j) <  CONSTRAINT_COFF_ERROR ){
                matrix.at(i).at(j) = 0;
            }
        }
        //RHS
		/*
        if( matrix.at(i).at(col_size-1) < RHS_ERROR ){
            matrix.at(i).at(col_size-1) = 0 ;
        }
		*/
    }
}


void show_row(vector<double> row_list){
    for(vector<double>::iterator iter = row_list.begin() ; iter!=row_list.end() ; iter++){
        cout<<(*iter)<<" ";
    }
    cout<<endl;
}


void Simplex::simpex_initinalize_from_parser(string filename){
    fstream f;
    f.open(filename.c_str());
    string line;

    bool ROWS_flag = true;
    bool COLUMNS_flag = true;
    bool RHS_flag = true;
    bool ENDATA_flag = true;


    if(f.is_open()){
        /**************************************************************/
        //readline until the line : "ROWS"
        while(ROWS_flag){
            getline(f,line);
            if(line.compare("ROWS")==0){
                ROWS_flag = false;
            }
        }
        string equation;
        string row;
        int row_count = 1 ;

        /**************************************************************/
        //Constucting the mapping table from row to row_count(A index)
        while(COLUMNS_flag){
            f>>equation;//convert to char
            if(equation.compare("COLUMNS") ==0 ){
                COLUMNS_flag=false;
                break;
            }
            f>>row;
            if(equation.compare("N")==0){
                obj_name = row;
                row_map[row] = 0;//set obj function as row0 !!!
            }else{
                operator_map[row_count] = equation[0];
                row_map[row] = row_count;//mapping from string to number
                row_count++;
                /* only Equal/Greater row has artifical
                if(equation[0]=='E' || equation[0]=='G'){
                    artifical_size++;
                }
                */
            }

        }



        //show operator
        /*
        for(map<int , char> ::iterator iter = operator_list.begin() ; iter!=operator_list.end() ; iter++){
            cout<<iter->first <<" "<< iter->second<<endl;
        }
        */

        //show row map
        /*
        for(TABLE::iterator itermap = row_map.begin() ; itermap!=row_map.end() ; itermap++){
            cout<<itermap->first <<" "<< itermap->second<<endl;
        }
        */
       /**************************************************************/

        row_size = row_count;
        // for(int i = 1 ; i < row_size ; i++){
        //     pivot_row_list.insert(i);
        // }


        string col , row1 , row2;
        row2 = "";
        double v1,v2;
        int col_count = 0 ;
        getline(f,line);//clear line

        /**************************************************************/
        //Constucting the mapping table from col to col_count(A index),
        //and store the matrix to matrix_register

        MATRIX_REG matrix_register;//map<pair<row,col> , value>
        while(RHS_flag){
            getline(f,line);
            if(line.compare("RHS")==0){
                RHS_flag = false;
                break;
            }

            stringstream test(line);
            vector<string>seg_list;

            while(test>>line){
                seg_list.push_back(line);
            }

            if(seg_list.size() == (unsigned int)5 ){
                col = seg_list.at(0);
                row1 = seg_list.at(1);
                v1 = stof(seg_list.at(2));
                row2 = seg_list.at(3);
                v2 = stof(seg_list.at(4));

                if(col_map.find(col) == col_map.end()){//construct col mapping
                    col_map.insert(pair<string,int>(col ,col_count ) );
                    col_count++;
                }
                matrix_register[make_pair( row_map[row1] , col_map[col] )] = v1;
                matrix_register[make_pair( row_map[row2] , col_map[col] )] = v2;

            }else if(seg_list.size() == (unsigned int)3){
                col = seg_list.at(0);
                row1 = seg_list.at(1);
                v1 = stof(seg_list.at(2));
                if(col_map.find(col) == col_map.end()){//construct col mapping
                    col_map.insert(pair<string,int>(col ,col_count ) );
                    col_count++;
                }
                matrix_register[make_pair( row_map[row1] ,col_map[col] )] = v1;
            }
        }

        col_size = col_count;
        ori_col_size = col_size ;

        //col map
        /*
        for(TABLE::iterator itermap = col_map.begin() ; itermap!=col_map.end() ; itermap++){
            cout<<itermap->first <<" "<< itermap->second<<endl;
        }
        */

        /**************************************************************/
        //initinalize the matrix value (row by col)
        matrix.reserve(row_size);//initinalize row
        vector<double>row_list(col_count , 0);//initinalize col don't contain RHS
        for(int i = 0 ; i <row_size ; i++){
            matrix.push_back(row_list);
        }

        //insert value from matrix_register to matrix
        for(MATRIX_REG::iterator iter_reg = matrix_register.begin() ; iter_reg!=matrix_register.end() ; iter_reg++){
            matrix.at(iter_reg->first.first).at(iter_reg->first.second) = iter_reg->second;
        }

        /**************************************************************/
        //Read RHS value to RHS_list (temporary)


        vector<double>RHS_list(row_size , 0) ;//obj doesn't have RHS
        map<int , double> RHS_map;

        while(ENDATA_flag){
            getline(f,line);
            if(line.compare("ENDATA")==0){
                RHS_flag = false;
                break;
            }

            stringstream ss(line);
            vector<string>seg_list;

            while(ss>>line){
                seg_list.push_back(line);
            }

            if(seg_list.size() == (unsigned int)5 ){

                row1 = seg_list.at(1);
                v1 = stof(seg_list.at(2));
                row2 = seg_list.at(3);
                v2 = stof(seg_list.at(4));
                RHS_map[row_map[row1]] = v1;
                RHS_map[row_map[row2]] = v2;
            }else if(seg_list.size() == (unsigned int)3){
                row1 = seg_list.at(1);
                v1 = stof(seg_list.at(2));
                RHS_map[row_map[row1]] = v1;
            }
        }

        /*
        for(int i =0 ; i <RHS_list.size() ; i++ ){
            cout<<RHS_list[i]<<" "<<endl;
        }
        */

        // show_matrix();

        /**************************************************************/
        //Adde slack variable
        // map<int , char> operator_map; store index of row  and operator
        // map<int , double> RHS_map; store the index of row and RHS value

        //Adjusted the row of negative RHS
        for(map<int , double>::iterator iter_map = RHS_map.begin() ; iter_map!=RHS_map.end() ; iter_map++){
            if(iter_map->second < 0 ){
                if(operator_map[iter_map->first] == 'L'){
                   operator_map[iter_map->first] ='G';
                }else if(operator_map[iter_map->first] == 'G'){
                    operator_map[iter_map->first] ='L';
                }
                //the cofficient of the row multiple (-1)
                for(vector<double>::iterator iter_value = matrix.at(iter_map->first ).begin() ;\
                 iter_value != matrix.at(iter_map->first).end() ; iter_value++){
                    if((*iter_value)!=0){
                        (*iter_value) = (*iter_value) *(-1);
                    }
                }
                //RGS of the row
                iter_map->second = (iter_map->second) * (-1);

            }
        }

        assert((int)matrix.size() == row_size && (int)matrix[1].size() == col_size);


        MATRIX_REG slack_coffi_map; //map<pair<int,int> , double>
        //Add slack variable
        for(map<int ,char>::iterator iter_map = operator_map.begin();iter_map !=operator_map.end() ; iter_map++ ){
            if(iter_map->second == 'L'){//lower than
                slack_coffi_map[make_pair(iter_map->first,col_size++)] =1;
                slack_size++;
            }else if(iter_map->second =='G' ){//great than
                slack_coffi_map[make_pair(iter_map->first,col_size++)] =-1;
                slack_size++;
            }
        }

        /**************************************************************/
        //Added Artifical variable
        // map<int , char> operator_map; store index of row  and operator
        // map<int , double> RHS_map; store the index of row and RHS value

        artifical_size = row_size -1;//rowsize - row_obj
        col_size +=artifical_size;
        col_size ++; //RHS value
        // resize the vector!!
        for(vector<vector<double> >::iterator iter_list = matrix.begin() ; iter_list!=matrix.end(); iter_list++){
            (*iter_list).resize(col_size , 0);
        }
        // fill the cofficient of slack variable
        for(MATRIX_REG::iterator iter_reg = slack_coffi_map.begin() ; iter_reg!=slack_coffi_map.end() ; iter_reg++){
            matrix.at(iter_reg->first.first).at(iter_reg->first.second) = iter_reg->second;
        }

        // fill the cofficient of artificial variable
        int artifi_col_count = ori_col_size+slack_size;
        int artifi_row_count = 1;
        for(vector<vector<double> >::iterator iter_list = matrix.begin()+1; iter_list!=matrix.end(); iter_list++){
            basic_variable_map.insert(make_pair(artifi_row_count,artifi_col_count ));
            (*iter_list).at(artifi_col_count) = 1;
            artifi_col_count++;
        }

        // RHS_map indexofrow value
        for(int i = 1 ; i < row_size ; i++){
            *(matrix.at(i).end()-1) = RHS_map[i] ;
        }


        /*
        //only equal row has artifical
        for(int i = 1 ; i < row_size ; i++){
            if( operator_map[i] == 'E' || operator_map[i] == 'G'){
                matrix.at(i).at(artifi_count) = 1;
                artifi_count++;
            }
        }
        */



    }else{
        cout<<"Open File Error"<<endl;
    }


}

void Simplex::show_slack_matrix(){

    int op_count = 1;
    for(vector<vector<double> >::iterator iter_row = matrix.begin()+1 ; iter_row!=matrix.end() ; iter_row++){
        cout<< operator_map[op_count++]<<": ";
        for(int i = ori_col_size ; i < ori_col_size+slack_size ; i++){
            cout<<(*iter_row).at(i)<<" ";
        }
        cout<<endl;
    }
    cout<<endl;
}


void Simplex::show_matrix(){
    for(vector<vector<double> >::iterator iter_row = matrix.begin() ; iter_row!=matrix.end() ; iter_row++){
        for(vector<double>::iterator iter_col = iter_row->begin() ; iter_col != iter_row->end() ; iter_col++){
            cout<< setw(3) << *iter_col <<"  ";
        }
        cout<<endl;
    }
    cout<<endl;
}


void Simplex::show_artifical_matrix(){
    for(vector<vector<double> >::iterator iter_row = matrix.begin() ; iter_row!=matrix.end() ; iter_row++){
        for(int i = ori_col_size+slack_size ; i < col_size ; i++){
            cout<<(*iter_row).at(i)<<" ";
        }
        cout<<endl;
    }
    cout<<endl;
}


void Simplex::output_matrix(string filename){
    fstream fout;

    fout.open(filename.c_str(),ios::out);

    if(fout.is_open()){
        fout<<" ";
        for(int i = 0 ; i < col_size ; i++){
            fout<<setw(14)<<i;
        }
        fout<<endl;
        int row_count = 0;
        for(vector<vector<double> >::iterator iter_row = matrix.begin() ; iter_row!=matrix.end() ; iter_row++){
            fout<<std::left <<setw(14) <<row_count++;
            for(vector<double>::iterator iter_col = iter_row->begin() ; iter_col != iter_row->end() ; iter_col++){
                fout<< setw(14) << *iter_col;
            }
            fout<<endl;
        }
        fout<<endl;
    }else{
        cout<<"output file error"<<endl;
    }

    fout.close();
}


// ostream& operator<<(ostream& os , const NODE& obj ){
//     os<<""<<obj.node_x<<","<<obj.node_y <<" w,h :(" <<obj.node_w <<","<<obj.node_h<<")"<<obj.node_x_mid<<" "<<obj.node_y_mid<<endl;
//     // os<<"w,h :( " <<obj.node_w <<","<<obj.node_h<<")"<<endl;
//     return os;
// }
