#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <semaphore.h>
#include <string>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <sstream>
#include <math.h>

using namespace std;

void read_input();
void read_weight();
void calculate_result();
void make_output();


//input arrays
double bias;
vector<double> weights;
vector< vector<double> > inputs;
vector<double> results;

int main(int argc, char const *argv[]) {
  read_input();
  read_weight();
  calculate_result();
  make_output();
}

void read_input() {
  ifstream in_file("inputs.txt");
  string line;
  double num;
  vector<double> row;
  if(in_file.is_open()) {
    getline(in_file, line);
    while(getline(in_file, line)) {
      string token = line.substr(line.find('{')+1);
      token = token.substr(0,token.find('}'));
      while(true) {
      if(token.find(',') != string::npos) {
        string tmp = token.substr(0,token.find(','));
        num = atof (tmp.c_str());
        row.push_back(num);
        token = token.substr(token.find(',')+1);
      }
      else if(token.size()==0) {
        break;
     }
     else {
       string tmp = token;
       num = atof (tmp.c_str());
       row.push_back(num);
       inputs.push_back(row);
       row.clear();
       break;
      }
     }
    }
  }
  else {
    cout << "ERR: problem in opening the file" << endl;
    exit(-1);
  }
}

void read_weight() {
  ifstream w_file("weights.txt");
  string line;
  double num;
  string bias_str;
  if(w_file.is_open()) {
    getline(w_file, line);
    while(getline(w_file, line)) {
      string token = line.substr(line.find('{')+1);
      token = token.substr(0,token.find('}'));
      if(token.find(':')!=string::npos){
        bias_str = token.substr(token.find(':')+1);
        bias = atof (bias_str.c_str());
        break;
      }

      while(true){
        if(token.find(',') != string::npos){
          string tmp = token.substr(0,token.find(','));
          num = atof (tmp.c_str());
          weights.push_back(num);
          token = token.substr(token.find(',')+1);
        }
        else{
          string tmp = token;
          num = atof (tmp.c_str());
          weights.push_back(num);
          break;
        }
      }
    }
  }
  else {
    cout << "ERR: problem in opening the file" << endl;
    exit(-1);
  }
}

void calculate_result() {
  for(int i =0; i < inputs.size(); i++) {
    results.push_back(0);
  }
  for(int i =0; i < results.size(); i++) {
    for(int j = 0; j < 128; j++) {
      results[i] += (weights[j] * inputs[i][j]);
    }
    results[i] += bias;
  }
}

void make_output() {
  ofstream outfile;
  outfile.open("outputs.txt", ios_base::app);
  for (int i =0; i < results.size(); i++) {
    ostringstream strs;
    double result = atan (results[i]);
    strs << result;
    string str = strs.str();
    outfile << str << endl;
  }

}
