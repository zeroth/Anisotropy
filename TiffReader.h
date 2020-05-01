#ifndef TIFFREADER_H
#define TIFFREADER_H

#include <string>
namespace zeroth {


struct TiffReaderData;
class TiffReader
{
public:
    TiffReader() = default;
    TiffReader(const std::string& filename);
    ~TiffReader();

    uint32_t width() const;
    uint32_t height() const;
    uint32_t sliceCount() const;
    std::string description() const;
    uint16_t bitsPerSample() const;
    uint16_t sampleFormat() const;
    void *readImage();

    void next();
    bool hasNext();

private:
    TiffReaderData *d;
};

}
#endif // TIFFREADER_H
