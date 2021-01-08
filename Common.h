#ifndef COMMON_H
#define COMMON_H

#define TIFF_LI 0x4949
#define TIFF_BI 0x4D4D

#define TIFF_ORDER_UNKNOWN 0
#define TIFF_ORDER_BIGENDIAN 1
#define TIFF_ORDER_LITTLEENDIAN 2

#define TIFF_LAST_ERROR_SIZE 1024

#define TIFF_COMPRESSION_NONE 1
#define TIFF_COMPRESSION_CCITT 2
#define TIFF_COMPRESSION_PACKBITS 32773

#define TIFF_PLANARCONFIG_CHUNKY 1
#define TIFF_PLANARCONFIG_PLANAR 2


#define TIFFREADER_SAMPLEFORMAT_UINT 1
#define TIFFREADER_SAMPLEFORMAT_INT 2
#define TIFFREADER_SAMPLEFORMAT_FLOAT 3
#define TIFFREADER_SAMPLEFORMAT_UNDEFINED 4

#define TIFF_TYPE_BYTE 1
#define TIFF_TYPE_ASCII 2
#define TIFF_TYPE_SHORT 3
#define TIFF_TYPE_LONG 4
#define TIFF_TYPE_RATIONAL 5

// tags
#define TIFF_FIELD_IMAGEWIDTH 256
#define TIFF_FIELD_IMAGELENGTH 257
#define TIFF_FIELD_BITSPERSAMPLE 258
#define TIFF_FIELD_COMPRESSION 259
#define TIFF_FIELD_PHOTOMETRICINTERPRETATION 262
#define TIFF_FIELD_IMAGEDESCRIPTION 270
#define TIFF_FIELD_STRIPOFFSETS 273
#define TIFF_FIELD_SAMPLESPERPIXEL 277
#define TIFF_FIELD_ROWSPERSTRIP 278
#define TIFF_FIELD_STRIPBYTECOUNTS 279
#define TIFF_FIELD_XRESOLUTION 282
#define TIFF_FIELD_YRESOLUTION 283
#define TIFF_FIELD_PLANARCONFIG 284
#define TIFF_FIELD_RESOLUTIONUNIT 296
#define TIFF_FIELD_SAMPLEFORMAT 339
#define TIFF_FIELD_IMAGEJMETADATA 50839

#include <inttypes.h>
#include <iostream>
namespace zeroth {


extern int getByteorder();

// swap byte little to big Endianness
// we are focusing on modern architecture
// https://en.wikipedia.org/wiki/Endianness
template<typename T> T byteswap(T value);
template<> inline uint32_t byteswap<uint32_t >(uint32_t nLongNumber)
{
    return (((nLongNumber&0x000000FF)<<24)+((nLongNumber&0x0000FF00)<<8)+
            ((nLongNumber&0x00FF0000)>>8)+((nLongNumber&0xFF000000)>>24));
}

template<> inline uint16_t byteswap<uint16_t>(uint16_t nValue)
{
    return (((nValue>> 8)) | (nValue << 8));
}

template<> inline uint8_t byteswap<uint8_t>(uint8_t nValue)
{
    return nValue;
}

typedef struct _TiffHeader
{
    uint16_t Identifier;  // Byte-order Identifier (2 bytes)
    uint16_t Version;     // TIFF version number (always 2Ah or 42 answer to everything ;) ) (2 bytes)
    uint32_t IFDOffset;   // Offset of the first Image File Directory (IFD) (4 bytes)
    void swap() {
        Identifier = byteswap<uint16_t>(Identifier);
        Version = byteswap<uint16_t>(Version);
        IFDOffset = byteswap<uint32_t>(IFDOffset);
    }
} TIFHEAD;



typedef struct _TifTag
{
    uint16_t TagId;       // The tag identifier (2 bytes)
    uint16_t DataType;    // The scalar type of the data items (2 bytes)
    uint32_t DataCount;   // The number of items in the tag data (4 bytes)
    uint32_t DataOffset;  // The byte offset to the data items (4 bytes)

    void swap() {
        TagId = byteswap<uint16_t>(TagId);
        DataType = byteswap<uint16_t>(DataType);
        DataCount = byteswap<uint32_t>(DataCount);
        DataOffset = byteswap<uint32_t>(DataOffset);
    }

    void print() {
        std::cout << "TagId : " << TagId << ", "
                  << "Data Type : " << DataType  << ", "
                  << "Data Count : " << DataCount << ", "
                  << "Data Offset : " << DataOffset << std::endl;
    }

} TIFTAG;

typedef struct _TifIfd
{
    uint16_t NumDirEntries;    //Number of Tags in IFD  (2 bytes)
    TIFTAG*  TagList;        // Array of Tags   (12* NumDirEntries)
    uint32_t NextIFDOffset;    // Offset to next IFD (4 bytes)

} TIFIFD;


extern inline std::string& ltrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
{
    str.erase(0, str.find_first_not_of(chars));
    return str;
}

extern inline std::string& rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
{
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
}

extern inline std::string& trim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
{
    return ltrim(rtrim(str, chars), chars);
}

}

#endif // COMMON_H
