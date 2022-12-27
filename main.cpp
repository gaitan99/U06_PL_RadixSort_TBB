#include <iostream>
#include <vector>
#include <algorithm>

#include <iostream>
#include <algorithm>
using namespace std;


#include <tbb/tbb.h>
#include "oneapi/tbb/blocked_range.h"
#include "oneapi/tbb/parallel_for.h"

using namespace std;
using namespace oneapi;

//Función con la que obtenemos que modificamos dos vectores:
    //a: Esta función depende de la máscara para poder saber que bit estamos analizando
    //out1: Tendrá valores a 1 en las posiciones realizando un and con la máscara sea 
    //      igual a la máscara, es decir, que ese bit sea 1
    //out0: Tendrá valores a 1 en las posiciones realizando un and con la máscara sea 
    //      diferente a la máscara, es decir, que ese bit sea 0
void doMAP(int a,int x[], int n,vector<int> &out1, vector<int> &out0)
{

    tbb::parallel_for(
        tbb::blocked_range<int>(0, n),

        // lambda function
        [&](tbb::blocked_range<int> r) {
            for (auto i = r.begin(); i != r.end(); i++) {
                if( (x[i] & a)  == a){ //Miramos si el bit que nos interesa es igual 1
                    out1[i] = 1;//En el vector de 1s introducimos un 1 en la posición correspondiente
                    out0[i] = 0;//En el vector de 0s introducimos un 0 
                }
                else{//Si el bit no es igual a 1
                    out1[i] = 0;//En el vector de 1s introducimos un 0 en la posición correspondiente
                    out0[i] = 1;//En el vector de 0s introducimos un 1 

                }
            }
        }

    );

}

//Funcion que realiza una acumulación de 1s y que retorna la suma total de estos
int doSCAN(int out[], const int in[],int n){
    int total_sum = tbb::parallel_scan(
        tbb::blocked_range<int>(0, n), //range
        0, //id
        [&](tbb::blocked_range<int> r, int sum, bool is_final_scan){        
            int tmp = sum;
            for (int i = r.begin(); i < r.end(); ++i) {
                tmp = tmp + in[i];
                if (is_final_scan)
                    out[i] = tmp;
            }
            return tmp;
        },
        [&]( int left, int right ) {
            return left + right;
        }
    );
    return total_sum;
}

//Función que nos realiza la ordenación del vector inicial x[1] en out según los tracamientos
//resultantes den ixMatch1 y ixMatch0
void doMAPFilter(int bolMatch[], int ixMatch1[], int ixMatch0[],int x[], int out[], int n, int sum1){
    tbb::parallel_for(
        tbb::blocked_range<int>(0, n),
        // lambda function
        [&](tbb::blocked_range<int> r) {

            for (auto i = r.begin(); i < r.end(); i++) {
                if (bolMatch[i]){ //En caso de que tengamos un 1 en bolMatch
                    //Introducimos en las primeras posiciones de out , teniendo en cuenta como indice
                    //ixMatch de los 1s , el valor correspondiente de la array incial x[i]
                    out[ixMatch1[i]-1] = x[i];
                }else{
                    //Introducimos en las primeras posiciones de out , teniendo en cuenta como indice
                    //ixMatch de los 0s , el valor correspondiente de la array incial x[i]
                    out[ixMatch0[i] + sum1 - 1] = x[i];
                 }
            }
        }
    );
}

int main(){

    int a = 0b0001; //máscara que utilizaremos para ir revisando bit a bit
    static vector<int> x{7,1,0,13,0,15,20,5}; // vector inicial
    static int maxValue = 20;
	tbb::tick_count t0 = tbb::tick_count::now();
 	vector<int> y = x;

    for(int bits = 0;bits < int(log2(maxValue)+1); bits ++){
        cout << "Iteration : " << bits << endl;  
        cout << endl;
        //MAP operation
        vector<int> bolMatch1(y.size());
        vector<int> bolMatch0(y.size()); 
        doMAP(a,&y[0], y.size(),bolMatch1,bolMatch0);

        cout << "----Map vector----: "<< endl;
        cout << endl;
        cout << "Map vector1: "<< endl;
        for (int i: bolMatch1){
            cout << i << ',';
        }
        cout << endl;

        cout << "Map vector0: "<< endl;
        for (int i: bolMatch0){
            cout << i << ',';
        }
        cout << endl;

        //SCAN
        vector<int> ixMatch1(y.size());
        vector<int> ixMatch0(y.size());
        int sum1 = doSCAN(&ixMatch1[0], &bolMatch1[0],  y.size()); //get index order
        int sum0 = doSCAN(&ixMatch0[0], &bolMatch0[0],  y.size());

        cout << endl;
        cout << "----Scan vector----: "<< endl;
        cout << endl;
        cout << "Scan vector1: " << sum1 << endl;
        for (int i: ixMatch1){
            cout << i << ',';
        }
        cout << endl;

        cout << "Scan vector0: " << sum0 << endl;
        for (int i: ixMatch0){
            cout << i << ',';
        }
        cout << endl;

        //JOIN
        vector<int> filtered_results1(x.size());
        doMAPFilter(&bolMatch1[0],&ixMatch1[0],&ixMatch0[0],&y[0],&filtered_results1[0], y.size(),sum1);

        cout << endl;
        cout << "----Filtered vector----" << endl;
        for (int i: filtered_results1){
            cout << i << ',';
        }
        cout << endl;

        cout << endl;
        
        y = filtered_results1;
        a = a << 1;

    }
   
 	cout << "\nTime: " << (tbb::tick_count::now()-t0).seconds() << "seconds" << endl;

 	return 0;

 }
 //bazel run //:Demo
