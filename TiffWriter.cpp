#include "TiffWriter.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>

namespace zeroth {
struct DataHandler {
    std::ofstream fh;
    std::string fileName;
};

struct TiffWriterData {
    TiffWriterData(const std::string & path, uint16_t _bitsbersample, uint16_t _sampleformat,
                   uint32_t _width, uint32_t _height, const std::string& desc) {
        bitsbersample = _bitsbersample;
        sampleformat = _sampleformat;
        width = _width;
        height = _height;
        description = desc;
//        extraOffset = 0;

        handler = new DataHandler;
        handler->fh.open(path.c_str(), std::ios::out | std::ios::binary);
        handler->fileName = std::string(path);
        sliceCount = 0;

        load();
    }

    ~TiffWriterData() {
        handler->fh.close();
        delete handler;
    }

    void load() {
        TIFHEAD head;
        head.Identifier = getByteorder();
        head.Version = 42;
        head.IFDOffset = 8;
        handler->fh.write(reinterpret_cast<char*>(&head), sizeof (head));
    }

    template<typename T>
    void writeTag(uint16_t TagId, uint16_t DataType, uint32_t DataCount, uint32_t DataOffset) {
        TIFTAG tag;
        tag.TagId = TagId;
        tag.DataType = DataType;
        tag.DataCount = DataCount;
        tag.DataOffset = DataOffset;
        handler->fh.write(reinterpret_cast<char*>(&tag), sizeof(tag));
    }

    void writeDescriptionTag(const std::string& desc) {
        TIFTAG tag;
        tag.TagId = TIFF_FIELD_IMAGEDESCRIPTION;
        tag.DataType = TIFF_TYPE_ASCII;
        tag.DataCount = 1024;
        tag.DataOffset = expectedLastPos;
        handler->fh.write(reinterpret_cast<char*>(&tag), sizeof(tag));
        fpos_t currentPos = handler->fh.tellp(); // store current pos
        handler->fh.seekp(expectedLastPos+1); // go to end of the header
        char s [1024];
        std::fill_n(s, 1024, '\0');

        std::strcpy(s, desc.c_str());
        s[1024-1] = '\0';
        handler->fh.write(s, 1024); // write the description
        expectedLastPos = ((uint32_t)handler->fh.tellp())+2; // becomes the new expected last pos
        handler->fh.seekp(currentPos); // go back to end of the tag
    }

    void writeRationalTag(uint16_t TagId, uint32_t numerator, uint32_t denominator) {
        TIFTAG tag;
        tag.TagId = TagId;
        tag.DataType = TIFF_TYPE_RATIONAL;
        tag.DataCount = 1;
        tag.DataOffset = expectedLastPos;
        handler->fh.write(reinterpret_cast<char*>(&tag), sizeof(tag));

        fpos_t currentPos = handler->fh.tellp(); // store current pos
        handler->fh.seekp(expectedLastPos); // go to end of the header

        handler->fh.write(reinterpret_cast<char*>(&numerator), sizeof (uint32_t));
        handler->fh.write(reinterpret_cast<char*>(&denominator), sizeof (uint32_t));
        expectedLastPos = ((uint32_t)handler->fh.tellp()); // becomes the new expected last pos
        handler->fh.seekp(currentPos); // go back to end of the tag
    }

    void writeIFD() {
        /*
            uint16_t NumDirEntries;    //Number of Tags in IFD  (2 bytes)
            TIFTAG*  TagList;        // Array of Tags   (12* NumDirEntries)
            uint32_t NextIFDOffset;    // Offset to next IFD (4 bytes)

            // tag list
            01 - width TIFF_TYPE_LONG
            02 - height TIFF_TYPE_LONG
            03 - bitspersample TIFF_TYPE_SHORT
            04 - compression TIFF_TYPE_SHORT
            05 - photometric interpretation TIFF_TYPE_SHORT
            06 - *Image description TIFF_TYPE_ASCII
            07 - StripOffset TIFF_TYPE_LONG
            08 - SamplePerPixel TIFF_TYPE_SHORT
            09 - RowsPerStrip TIFF_TYPE_LONG
            10 - StripByteCounts TIFF_TYPE_LONG
            11 - XResolution TIFF_TYPE_RATIONAL
            12 - YResolution TIFF_TYPE_RATIONAL
            13 - Planarconfig TIFF_TYPE_SHORT
            14 - ResolutionUnit TIFF_TYPE_RATIONAL

            TODO: 
            Write ImageJ tiff tags imagej_metadata_tags
            https://github.com/blink1073/tifffile/blob/af9d70058b01f501b29cfe924a6463414c388480/tifffile/tifffile.py#L7710
            also write ImageJ description  imagej_metadata
            https://github.com/blink1073/tifffile/blob/af9d70058b01f501b29cfe924a6463414c388480/tifffile/tifffile.py#L7787
        */
        description = trim(description);
        bool isDescription = description.size() > 1;
        totalNumberOfTags = isDescription ? 15 : 14;

        // expectedLastPos will changed based on description location and RATIONAL

        handler->fh.write(reinterpret_cast<char*>(&totalNumberOfTags), sizeof (uint16_t));

        expectedLastPos = (uint32_t)handler->fh.tellp() + (totalNumberOfTags*12) + 4 +2; // +2 is just a buffer of 2 bytes

        writeTag<uint32_t>(TIFF_FIELD_IMAGEWIDTH, TIFF_TYPE_LONG, 1, width);
        writeTag<uint32_t>(TIFF_FIELD_IMAGELENGTH, TIFF_TYPE_LONG, 1, height);
        writeTag<uint16_t>(TIFF_FIELD_BITSPERSAMPLE, TIFF_TYPE_SHORT, 1, bitsbersample);
        writeTag<uint16_t>(TIFF_FIELD_COMPRESSION, TIFF_TYPE_SHORT, 1, 1);
        writeTag<uint16_t>(TIFF_FIELD_PHOTOMETRICINTERPRETATION, TIFF_TYPE_SHORT, 1, 1);
        // write description
        if(isDescription){
            writeDescriptionTag(description);
        }

        stripOffset = expectedLastPos + 16 + 2; // 16 bytes because we are writting 2 rationals and 4 for the nextoffset
        writeTag<uint32_t>(TIFF_FIELD_STRIPOFFSETS, TIFF_TYPE_LONG, 1, stripOffset);
        writeTag<uint16_t>(TIFF_FIELD_SAMPLESPERPIXEL, TIFF_TYPE_SHORT, 1, 1);
        writeTag<uint32_t>(TIFF_FIELD_ROWSPERSTRIP, TIFF_TYPE_LONG, 1, height);
        writeTag<uint32_t>(TIFF_FIELD_STRIPBYTECOUNTS, TIFF_TYPE_LONG, 1, width*height * (bitsbersample/8));
        writeRationalTag(TIFF_FIELD_XRESOLUTION, 1,1);
        writeRationalTag(TIFF_FIELD_YRESOLUTION, 1,1);
        writeTag<uint16_t>(TIFF_FIELD_PLANARCONFIG, TIFF_TYPE_SHORT, 1, 1);
        writeTag<uint16_t>(TIFF_FIELD_RESOLUTIONUNIT, TIFF_TYPE_SHORT, 1, 1);
        writeTag<uint16_t>(TIFF_FIELD_SAMPLEFORMAT, TIFF_TYPE_SHORT, 1, sampleformat);

        // we have writting almost all the tags
        // calculate NextIFDOffset and write
        nextIFDOffset = 2 + stripOffset + ((width* height) * (bitsbersample/8));
        handler->fh.write(reinterpret_cast<char*>(&nextIFDOffset), sizeof (uint32_t));
        sliceCount++;
    }

    void writeSlice(void* data) {
        // 1st write a ifd + tags
        writeIFD();

        // write the image
        // 1st goto striposset
        handler->fh.seekp(stripOffset);
        handler->fh.write(reinterpret_cast<char*>(data), width*height * (bitsbersample/8));
        handler->fh.seekp(nextIFDOffset);
    }

    DataHandler* handler;
    uint16_t bitsbersample;
    uint16_t sampleformat;
    std::string path;
    uint32_t width;
    uint32_t height;
    std::string description;
    uint16_t totalNumberOfTags;
    uint32_t expectedLastPos;
    uint32_t nextIFDOffset;
    uint32_t stripOffset;
    uint32_t sliceCount;
};

TiffWriter::~TiffWriter()
{
    delete  d;
}

TiffWriter::TiffWriter(const std::string &filepath, uint16_t bitsbersample,uint16_t sampleformat,
                       uint32_t width, uint32_t height,
                       const std::string& descriptions)
{
    d = new TiffWriterData(filepath, bitsbersample, sampleformat, width, height, descriptions);
}

void TiffWriter::addNextSlice(void *data)
{
    d->writeSlice(data);
}
}
