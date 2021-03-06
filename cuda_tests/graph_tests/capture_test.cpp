#include "tests.h"
#include "nodes.h"
#include "tensor.h"
#include "utils.h"

void capture_test()
{
    std::cout << "Start capture_test:" << std::endl;

    const size_t N = 1, C = 1, H = 32, W = 32;
    shape_t shape = to_shape(N, C, H, W);

    cpu_tensor_t ca(shape), cb(shape), cc(shape), cd(shape);

    init_rand(ca, 0.0f, 1.0f);
    init_rand(cb, 0.0f, 1.0f);
    init_rand(cc, -2.0f, -1.0f);

    add_cpu((int)ca.size, ca.data, cb.data, cd.data);

    cudaStream_t stream;
    CHECK(cudaStreamCreateWithFlags(&stream, cudaStreamNonBlocking));

    gpu_tensor_t ga(shape, stream), gb(shape, stream), gc(shape, stream);

    cublasHandle_t cublasHandle;
    CHECK(cublasCreate(&cublasHandle));
    CHECK(cublasSetStream(cublasHandle, stream));

    CHECK(cudaStreamBeginCapture(stream, cudaStreamCaptureModeThreadLocal));

    copy(ca, ga);
    copy(cb, gb);

    add_cublas(cublasHandle, (int)ga.size, ga.data, gb.data, gc.data);

    copy(gc, cc);

    cudaGraph_t graph;
    CHECK(cudaStreamEndCapture(stream, &graph));

    print_graph_info(graph);

    cudaGraphExec_t graphExec;
    CHECK(cudaGraphInstantiate(&graphExec, graph, NULL, NULL, 0));

    CHECK(cudaGraphLaunch(graphExec, stream));

    CHECK(cudaStreamSynchronize(stream));

    cublasDestroy(cublasHandle);

    CHECK(cudaStreamDestroy(stream));

    if (!check(cd, cc, "cublas_test"))
        return;

    std::cout << "capture_test is finished." << std::endl;
}

