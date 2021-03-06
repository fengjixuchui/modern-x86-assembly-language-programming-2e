//------------------------------------------------
//               Ch14_02.cpp
//------------------------------------------------

#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <random>
#include "AlignedMem.h"

using namespace std;

// Ch14_02_Misc.cpp
extern bool Avx512ConvertImgU8ToF32Cpp(float* des, const uint8_t* src, uint32_t num_pixels);
extern bool Avx512ConvertImgF32ToU8Cpp(uint8_t* des, const float* src, uint32_t num_pixels);
extern uint32_t Avx512ConvertImgVerify(const float* src1, const float* src2, uint32_t num_pixels);
extern uint32_t Avx512ConvertImgVerify(const uint8_t* src1, const uint8_t* src2, uint32_t num_pixels);

// Ch14_02_.asm
extern "C" bool Avx512ConvertImgU8ToF32_(float* des, const uint8_t* src, uint32_t num_pixels);
extern "C" bool Avx512ConvertImgF32ToU8_(uint8_t* des, const float* src, uint32_t num_pixels);

void InitU8(uint8_t* x, uint32_t n, unsigned int seed)
{
    uniform_int_distribution<> ui_dist {0, 255};
    default_random_engine rng {seed};

    for (uint32_t i = 0; i < n; i++)
        x[i] = ui_dist(rng);
}

void InitF32(float* x, uint32_t n, unsigned int seed)
{
    uniform_int_distribution<> ui_dist {0, 1000};
    default_random_engine rng {seed};

    for (uint32_t i = 0; i < n; i++)
        x[i] = (float)ui_dist(rng) / 1000.0f;
}

void Avx512ConvertImgU8ToF32(void)
{
    const size_t align = 64;
    const uint32_t num_pixels = 1024;
    AlignedArray<uint8_t> src_aa(num_pixels, align);
    AlignedArray<float> des1_aa(num_pixels, align);
    AlignedArray<float> des2_aa(num_pixels, align);
    uint8_t* src = src_aa.Data();
    float* des1 = des1_aa.Data();
    float* des2 = des2_aa.Data();

    InitU8(src, num_pixels, 12);

    bool rc1 = Avx512ConvertImgU8ToF32Cpp(des1, src, num_pixels);
    bool rc2 = Avx512ConvertImgU8ToF32_(des2, src, num_pixels);

    cout << "\nResults for Avx512ConvertImgU8ToF32\n";

    if (!rc1 || !rc2)
    {
        cout << "Invalid return code - ";
        cout << "rc1 = " << boolalpha << rc1 << ", ";
        cout << "rc2 = " << boolalpha << rc2 << '\n';
        return;
    }

    uint32_t num_diff = Avx512ConvertImgVerify(des1, des2, num_pixels);
    cout << "  Number of pixel compare errors (num_diff) = " << num_diff << '\n';
}

void Avx512ConvertImgF32ToU8(void)
{
    const size_t align = 64;
    const uint32_t num_pixels = 1024;
    AlignedArray<float> src_aa(num_pixels, align);
    AlignedArray<uint8_t> des1_aa(num_pixels, align);
    AlignedArray<uint8_t> des2_aa(num_pixels, align);
    float* src = src_aa.Data();
    uint8_t* des1 = des1_aa.Data();
    uint8_t* des2 = des2_aa.Data();

    InitF32(src, num_pixels, 20);

    // Test values to demonstrate clipping in conversion functions
    src[0] = 0.5f;          src[8]  = 3.33f;
    src[1] = -1.0f;         src[9]  = 0.67f;
    src[2] = 0.38f;         src[10] = 0.75f;
    src[3] = 0.62f;         src[11] = 0.95f;
    src[4] = 2.1f;          src[12] = -0.33f;
    src[5] = 0.25f;         src[13] = 0.8f;
    src[6] = -1.25f;        src[14] = 0.12f;
    src[7] = 0.45f;         src[15] = 4.0f;

    bool rc1 = Avx512ConvertImgF32ToU8Cpp(des1, src, num_pixels);
    bool rc2 = Avx512ConvertImgF32ToU8_(des2, src, num_pixels);

    cout << "\nResults for Avx512ConvertImgF32ToU8\n";

    if (!rc1 || !rc2)
    {
        cout << "Invalid return code - ";
        cout << "rc1 = " << boolalpha << rc1 << ", ";
        cout << "rc2 = " << boolalpha << rc2 << '\n';
        return;
    }

    uint32_t num_diff = Avx512ConvertImgVerify(des1, des2, num_pixels);
    cout << "  Number of pixel compare errors (num_diff) = " << num_diff << '\n';
}

int main()
{
    Avx512ConvertImgU8ToF32();
    Avx512ConvertImgF32ToU8();
    return 0;
}
