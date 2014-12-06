#include "RokkoParser.h"

using namespace std;

int main(int argc, char** argv)
{
    if(argc < 3)
        cout << "Usage: " << argv[0]
        << " [output filename] [input filename] [0|1 = verbose]"
        << endl;
    
    string obj_filename;
    string out_filename;
    
    if(argc > 2)
    {
        out_filename = argv[2];
    }
    else
    {
        cout << "Enter name of output file: ";
        getline(cin, out_filename);
    }
    
    if(argc > 1)
    {
        out_filename = argv[1];
    }
    else
    {
        cout << "Enter name of input file: ";
        getline(cin, obj_filename);
    }
    
    bool verbose = false;
    if(argc > 3)
        if(argv[3][0] != '0')
            verbose = true;
    
    RokkoParser::objToTxt(obj_filename, out_filename, verbose);
    
    return 0;
}