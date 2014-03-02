#ifndef SEND_GRID
#define SEND_GRID

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>

using namespace std;

void sendGrid(string destination, string subject, string source, string text);

/*int main() {
    string destination; // Format: Name <example@something.com>
    string subject;     // Format: Subject
    string source;      // Format: My Name <example@another.com>
    string text;        // Rest of the text
    destination = "Sam <shong010@ucr.edu>";
    subject = "Hello World!!";
    source = "torch <torch@stuff.com>";
    text = "sam: lol!\njohn: stuff\nsam: again";

    sendGrid(destination, subject, source, text);
    
}*/

void sendGrid(string destination, string subject, string source, string text) {
    ofstream fout;
    fout.open("input_sendgrid.txt");
    fout << destination << endl;
    fout << subject << endl;
    fout << source << endl;
    fout << text;
    fout.close();

    // Execute the python file here
    system("./send2grid.py");
}

#endif
