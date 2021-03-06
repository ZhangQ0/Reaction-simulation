//NOTE:
//If you are using linux system, a wine program should be installed firstly!
//The electronic energy loss and electronic energy loss divergence table are 
//generated by calling srimodule in SRIM program.
//Use the functions provided by corteo program
// Generate dE/dx files for all elements in all elements (part of setup) what you wanted
// - for each Z from 1 to 92
//     - create a file Z.stop and Z.strag
//     - for each element from H to U
//The method of electric stopping power:
//     - write a SRIModule input file to have the dE/dx at DIMD energies
//     - run SRIModule
//     - read the output
//     - write the result in Z.stop file

//Program start
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cmath>
//The system command in this header file will be responsible for running programs such as system ("SRModule.exe").
#include <cstdlib>         
using namespace std;
//The number of energy loss points you calculated
#define DIMD  849          // number of stopping power
//===================================================================================================



int new_line(istream& in_stream)
{
  char symbol;
  do
    {
      in_stream.get(symbol);
    }while(symbol !='\n');
}

int Dindex(double val)
{
  int ll;
  ll = log(val/10.0)/log(1.02);
  return ll;
}

double Dval(int index) //stopping power tables
{
  double temp1, temp2;
  temp1=pow(1.02,index);
  temp2=pow(1.02,index+1); 
  return (temp1+temp2)*5.0;
}

int main() 
{
  string TargetElecNum[2]={"1","6"};
  string TargetMass[2]={"2.013553","12.011"};


  int    IonZ=3;
  string IonName[1]={"7Li"};
  float  Isotope[2]={1,7.016004};
  string TargetName[2]={"Hydrogen","Carbon"};
 
  string filename;
  int i,j,k,maxElements = (int)Isotope[0];
  double m;
  double Energy,Se,Sn;//Energy(unit):eV;Se,Sn(unit)eV/(1E15 atoms/cm2)
  ifstream readsrim;
  ofstream writeinput,writesrim;
  #ifdef WIN64
  string endline= "\n";
  #else
  // in Linux, when SRModule is used with Wine, we must feed a input file containing return+linefeed
  string endline= "\r\n"; 
  #endif
  for(j=0;j<maxElements;j++){
    for(i=0;i<2;i++){
	    filename="../Stoppingpower/"+IonName[j]+"In"+TargetName[i]+".dat";
	    cout<<"Generating stopping table "<<filename<<endl;
	    m=Isotope[j+1];
      //write SRModule input file according to the file given by SRIM
	    writeinput.open("sr.in");
	    writeinput<<"---Stopping/Range Input Data (Number-format: Period = Decimal Point)"<<endline;
	    writeinput<<"---Output File Name,here arbitrary"<<endline;
	    writeinput<<"\"sr.out\""<<endline;
	    writeinput<<"---Ion(Z), Ion Mass(u)"<<endline;
	    writeinput<<IonZ<<" "<<m<<endline;
	    writeinput<<"---Target Data: (Solid=0,Gas=1), Density(g/cm3), Compound Corr."<<endline;
	    writeinput<<"0 1 0"<<endline;
  	  writeinput<<"---Number of Target Elements"<<endline;
	    writeinput<<"1"<<endline;
	    writeinput<<"---Target Elements: (Z), Target name, Stoich, Target Mass(u)"<<endline;
	    writeinput<<TargetElecNum[i]<<" \"X\" "<<"1"<<" "<<TargetMass[i]<<endline;// last 1 is target element mass, affects nuclear stopping not needed here
	    // writeinput<<TargetElecNum[i+1]<<" \"X\" "<<"1"<<" "<<TargetMass[i+1]<<endline;// last 1 is target element mass, affects nuclear stopping not needed here
	    writeinput<<"---Output Stopping Units (1-8)"<<endline;
	    writeinput<<"7"<<endline; 
	    writeinput<<"---Ion Energy : E-Min(keV), E-Max(keV)"<<endline; 
	    writeinput<<"0 0"<<endline;
	    for(k=0;k<DIMD;k++){
	        writeinput<<Dval(k)/1000.0<<endline; //Give the calculated energy loss points
	    }
	    writeinput<<"0"<<endline;
	    writeinput.close();
      #ifdef WIN32
	      system("SRModule.exe");
      #else
	      system("wine SRModule.exe");
      #endif
      //Start to read the calculated results and sort them out
	    readsrim.open("sr.out");
	    writesrim.open(filename.c_str());
	    for(k=0;k<4;k++) new_line(readsrim);
	    for(k=0;k<DIMD;k++){
	      readsrim>>Energy>>Se>>Sn;
	      writesrim<<setw(15)<<Energy * 1000.0<<"  "<<setw(15)<<Se+Sn<<endl;
	      new_line(readsrim);
	    }   
	    writesrim.close();
	    readsrim.close();	 
	  }
  }
}
