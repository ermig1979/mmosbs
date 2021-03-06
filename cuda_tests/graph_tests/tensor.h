#pragma once

#include "defs.h"

typedef std::vector<size_t> shape_t;

inline size_t to_size(const shape_t& shape)
{
    if (shape.empty())
        return 0;
    size_t size = 1; 
    for (size_t i = 0; i < shape.size(); ++i)
        size *= shape[i];
    return size;
}

inline shape_t to_shape(size_t a)
{
    return shape_t({ a });
}

inline shape_t to_shape(size_t a, size_t b)
{
    return shape_t({ a, b });
}

inline shape_t to_shape(size_t a, size_t b, size_t c)
{
    return shape_t({ a, b, c });
}

inline shape_t to_shape(size_t a, size_t b, size_t c, size_t d)
{
    return shape_t({ a, b, c, d });
}

struct gpu_tensor_t
{
    float* data;
    size_t size;
    shape_t shape;
    cudaStream_t stream;


    gpu_tensor_t(const shape_t& shape_ = shape_t(), cudaStream_t stream_ = 0)
        : shape(shape_)
        , size(to_size(shape_))
        , data(0)
        , stream(stream_)
    {
        //if(stream)
        //    CHECK(cudaMallocAsync(&data, size * sizeof(float), stream));
        //else
            CHECK(cudaMalloc(&data, size * sizeof(float)));
    }

    ~gpu_tensor_t()
    {
        if (data)
        {
            //if (stream)
            //    CHECK(cudaFreeAsync(data, stream));
            //else
                CHECK(cudaFree(data));
            data = 0;
        }
    }
};

typedef std::vector<gpu_tensor_t> gpu_tensors_t;

struct cpu_tensor_t
{
    float* data;
    size_t size;
    shape_t shape;

    cpu_tensor_t(const shape_t& shape_ = shape_t())
        : shape(shape_)
        , size(to_size(shape_))
        , data(0)
    {
        CHECK(cudaHostAlloc(&data, size * 4, cudaHostAllocDefault));
    }

    ~cpu_tensor_t()
    {
        if (data)
        {
            CHECK(cudaFreeHost(data));
            data = 0;
        }
    }
};

typedef std::vector<cpu_tensor_t> cpu_tensors_t;

inline void copy(const cpu_tensor_t& src, gpu_tensor_t& dst)
{
    assert(src.size == dst.size);
    if(dst.stream)
        CHECK(cudaMemcpyAsync(dst.data, src.data, src.size * sizeof(float), cudaMemcpyDeviceToHost, dst.stream));
    else
        CHECK(cudaMemcpy(dst.data, src.data, src.size * sizeof(float), cudaMemcpyHostToDevice));
}

inline void copy(const gpu_tensor_t& src, cpu_tensor_t& dst)
{
    assert(src.size == dst.size);
    if(src.stream)
        CHECK(cudaMemcpyAsync(dst.data, src.data, src.size * sizeof(float), cudaMemcpyDeviceToHost, src.stream));
    else
        CHECK(cudaMemcpy(dst.data, src.data, src.size * sizeof(float), cudaMemcpyDeviceToHost));
}

inline void init_rand(cpu_tensor_t& tensor, float lo, float hi, int seed = 0)
{
    srand(seed);
    for (int i = 0; i < tensor.size; ++i)
    {
        float val = float(rand()) / float(RAND_MAX);
        tensor.data[i] = val * (hi - lo) + lo;
    }
}

