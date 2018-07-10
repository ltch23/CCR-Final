#include <iostream>
#include <fstream>

#include <string.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>

using namespace std;


int main () {

	std::vector<std::string> v_text;
	std::vector<std::string> v_rpta;
    ifstream file;
    file.open ("html.txt");
    
    string word;
    while (file >> word){
        v_text.push_back(word);
    }

	for (int i=0; i< v_text.size();i++){
		if (v_text[i]=="<td>"){
			v_rpta.push_back(v_text[i+1]);
		}
	}
	ofstream myfile;
  	myfile.open ("table.txt");
	for (int i=0; i< v_rpta.size();i++)
		cout<<v_rpta[i];
		cout<<endl;

	for (int i=0; i< v_rpta.size();i+=3){
		myfile << v_rpta[i]+"\t"+v_rpta[i+1]+"\t"+
				v_rpta[i+2]+"\n";
	}


  	myfile.close();
	std::cout<<std::endl;

	// char * str= new char [_text.length()+1];
	// strcpy (str, _text.c_str());
	// split(char * str);
   // char str[80] = "This is - www.tutorialspoint.com - website";
   // const char s[2] = "-";
   // char *token;
   
   // /* get the first token */
   // token = strtok(str, s);
   
   // /* walk through other tokens */
   // while( token != NULL ) {
   //    printf( " %s\n", token );
    
   //    token = strtok(NULL, s);
   
   return(0);
}