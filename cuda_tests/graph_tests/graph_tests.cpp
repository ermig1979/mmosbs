#include "defs.h"
#include "utils.h"
#include "tests.h"

int main(int argc, char* argv[])
{
    print_device_info();

    //add_test();

    //cublas_test();

    //capture_test();

    //subgraph_manual_test();

    subgraph_capture_test();
}