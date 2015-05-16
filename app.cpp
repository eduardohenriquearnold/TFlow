#include "TFlow.hpp"

int main(int argc,char *argv[])
{
        TFlow t;
        
        if (argc>1)
                if (string(argv[1]) == "-c")
                {
                        t.genConfig();
                        return 0;
                }
                        
        t.play();
        return 0;
}
