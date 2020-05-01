#ifndef TIFFWRITER_H
#define TIFFWRITER_H

#include "Common.h"

namespace zeroth {
struct TiffWriterData;
class TiffWriter
{
public:
    TiffWriter() = default;
    ~TiffWriter();
    TiffWriter(const std::string& filepath, uint16_t bitsbersample, uint16_t sampleformat,
               uint32_t width, uint32_t height,
               const std::string& descriptions = "");
    void addNextSlice(void* data);

private:
    TiffWriterData *d;
};
}
#endif // TIFFWRITER_H
