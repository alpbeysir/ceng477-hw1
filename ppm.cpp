#include "ppm.h"
#include <fstream>
#include <stdexcept>

vec4fc vec127 = {127.0f, 127.0f, 127.0f, 127.0f};
vec4fc vec128 = {128.0f, 128.0f, 128.0f, 128.0f};
vec4fc vec255 = {255.0f, 255.0f, 255.0f, 255.0f};

static std::array<unsigned char, 3> convert_to_zort(vec4fc v) {
    std::array<unsigned char, 3> output;
    auto arr = amake(vec255 * v);
    arr[0] = std::min(255.0f, std::max(arr[0], 0.0f));
    arr[1] = std::min(255.0f, std::max(arr[1], 0.0f));
    arr[2] = std::min(255.0f, std::max(arr[2], 0.0f));
    output[0] = (unsigned char)arr[0];
    output[1] = (unsigned char)arr[1];
    output[2] = (unsigned char)arr[2];
    return output;
}

void write_ppm(std::string filename, vec4f* data, int width, int height)
{
    std::ofstream file;
    file.open(filename);

    file << "P6\n" << width << " " << height << "\n255\n";

    for (size_t j = 0; j < height; ++j)
    {
        for (size_t i = 0; i < width; ++i)
        {
            auto converted = convert_to_zort(data[i * height + j]);
            file << converted[0] << converted[1] << converted[2];
        }
    }
    file.close();
}
