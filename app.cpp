#include "TFlow.hpp"


int main(int argc,char *argv[])
{
        TFlow t;
        
        if (argc < 2)
        {
                cout << "Use './exec -c configFile.xml' to create a new configuration file." << endl;
                cout << "Use './exec configFile.xml' to run the program with an existing configuration file." << endl;
                return 0;
        }
                
        if (string(argv[1]) == "-c")
        {
                t.genConfig(string(argv[2]));
                return 0;
        }
        else
                t.loadConfig(string(argv[1]));
                        
        t.play();
        return 0;
}
