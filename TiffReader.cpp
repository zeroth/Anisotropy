#include "TiffReader.h"
#include "Common.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>
/*
To exapnd it to RGB images and more read
https://www.fileformat.info/format/tiff/egff.htm
// WORD = 2 bytes
// DWORD = 4 bytes

// HEADER
typedef struct _TiffHeader
{
    WORD  Identifier;  // Byte-order Identifier (2 bytes)
    WORD  Version;     // TIFF version number (always 2Ah or 42 answer to everything ;) ) (2 bytes)
    DWORD IFDOffset;   // Offset of the first Image File Directory (IFD) (4 bytes)
} TIFHEAD;

// IFD
 typedef struct _TifIfd
 {
    WORD    NumDirEntries;    //Number of Tags in IFD  (2 bytes)
    TIFTAG  TagList[];        // Array of Tags   (12* NumDirEntries)
    DWORD   NextIFDOffset;    // Offset to next IFD (4 bytes)
 } TIFIFD;

 // TAG
typedef struct _TifTag
{
    WORD   TagId;       // The tag identifier (2 bytes)
    WORD   DataType;    // The scalar type of the data items (2 bytes)
    DWORD  DataCount;   // The number of items in the tag data (4 bytes)
    DWORD  DataOffset;  // The byte offset to the data items (4 bytes)
} TIFTAG;
*/


namespace zeroth {
struct FileHandler {
    std::ifstream fh;
    std::string fileName;
    uint64_t fileSize;
    bool needSwap;
};


typedef struct _TifTagData {
    uint16_t tagId;       // The tag identifier (2 bytes)
    uint16_t dataType;    // The scalar type of the data items (2 bytes)
    uint32_t dataCount;   // The number of items in the tag data (4 bytes)
    uint32_t dataOffset;  // The byte offset to the data items (4 bytes)

    // as we dont know what the data will be but we just know the data type and number of element of that data type
    // lets try to make everything looks same for the moment
    uint32_t *value;
    uint32_t *value2;

    // data handler
    FileHandler* handler;
    template<typename T>
    T read(){
        T val;
        handler->fh.read(reinterpret_cast<char*>(&val), sizeof(T));
        if(handler->needSwap) {
            val = byteswap<T>(val);
        }
        return val;
    }

    void print() {
        std::cout << "TagId : " << tagId << ", "
                  << "Data Type : " << dataType  << ", "
                  << "Data Count : " << dataCount << ", "
                  << "Data Offset : " << dataOffset << ", "
                  << "Value [0] : " << value[0] << std::endl;
    }


    template<typename T> void loadTag() {
        value =  new uint32_t[dataCount];
        // dataOffest is uint32_t type which can have max 4 uint8_t
        // lets findout based on T how much the dataOffset can store

        uint8_t sizeCheck = sizeof (uint32_t) / sizeof (T);

        if(dataCount <= sizeCheck) {
            // this means we dont need to change the filepointer (seek).
            for(uint16_t i= 0; i< sizeCheck; i++) {
                uint16_t index = handler->needSwap ? (sizeCheck-1)-i : i;
                uint32_t v = ((T*)&dataOffset)[index];
                if(i < dataCount)
                    value[i] = v;
            }
        } else {
            // we need to change the filepointer (seek)
            fpos_t currentPos = handler->fh.tellg();

            // check if we are not reaching EOF
            if(dataOffset + (dataCount*sizeof (T)) <= handler->fileSize) {
                handler->fh.seekg(dataOffset);

                for(uint32_t i= 0; i< dataCount; i++) {
                    value[i] = read<T>();;
                }
            }
            handler->fh.seekg(currentPos);
        }

    }

    void loadData(FileHandler* _handler, TIFTAG& tag) {
        tagId = tag.TagId;
        dataType = tag.DataType;
        dataCount = tag.DataCount;
        dataOffset = tag.DataOffset;
        handler = _handler;
        if(dataCount >0){
            switch (dataType) {
            case TIFF_TYPE_BYTE :
            case TIFF_TYPE_ASCII : {
                loadTag<uint8_t>();
                break;
            }
            case TIFF_TYPE_SHORT : {
                loadTag<uint16_t>();
                break;
            }
            case TIFF_TYPE_LONG : {
                loadTag<uint32_t>();
                break;
            }
            case TIFF_TYPE_RATIONAL : {
                value =  new uint32_t[dataCount];
                value2 =  new uint32_t[dataCount];

                // we need to change the filepointer (seek)
                fpos_t currentPos = handler->fh.tellg();

                // check if we are not reaching EOF
                if(dataOffset + (dataCount * sizeof (uint32_t)) <= handler->fileSize) {
                    handler->fh.seekg(dataOffset);

                    for(uint32_t i= 0; i< dataCount; i++) {
                        value[i] = read<uint32_t>();
                        value2[i] = read<uint32_t>();
                    }
                }
                handler->fh.seekg(currentPos);

                break;
            }
            default: break;

            }
        } else {
            value = nullptr;
            value2 = nullptr;
        }
    }

} TIFFTAGDATA;


struct TiffSlice {
    TiffSlice(uint32_t offset, FileHandler* d) {
        startingOffset= offset;
        handler = d;
        hasNext = false;
        compression = TIFF_COMPRESSION_NONE;
        plannerConfiguration = TIFF_PLANARCONFIG_PLANAR;
        sampleFormat = TIFFREADER_SAMPLEFORMAT_UINT;
        stripOffsets = nullptr;
        stripByteCounts = nullptr;
        width = 0;
        height = 0;
        rowsPerStrip = 0;
        stripCount = 0;
        samplePerPixel = 0;
        load();
    }
    ~TiffSlice() {
        delete [] bitsPerSample;
        delete [] stripOffsets;
        delete [] stripByteCounts;
    }

    template<typename T>
    std::istream & readByte(T& value){
        return handler->fh.read(reinterpret_cast<char*>(&value), sizeof(T));
    }

    template<typename T>
    std::istream & readBytes(T *value, size_t size){
        return handler->fh.read(reinterpret_cast<char*>(value), size);
    }

    template<typename T>
    void* loadImageData(uint32_t imageSize) {
        T* buffer = new T[imageSize];
        uint32_t startPoint = 0;
        for(uint32_t s =0; s< stripCount; s++){
            T* tmp = new T[stripByteCounts[s]/ sizeof (T)];
//            changeFpos(currentSlice->stripOffsets[s]);
            handler->fh.seekg(stripOffsets[s]);
            readBytes<T>(tmp, stripByteCounts[s]);

            uint32_t pixels = rowsPerStrip * width; // total number of pixels
            startPoint = s * pixels;

            if(startPoint+pixels > imageSize) {
                pixels = imageSize - startPoint;
            }
            if(handler->needSwap) {
                // revers the byte order for every 16 bit element
                for(uint32_t x =0; x< pixels; x++) {
                    buffer[startPoint+x] = byteswap<T>(tmp[x]);
                }
            } else {
                std::copy(tmp, tmp + stripByteCounts[s]/ sizeof (T), &buffer[startPoint]);
            }

            delete [] tmp;
        }
//        changeFpos(pos);
        return buffer;

    }
    void* read(uint16_t sample=0) {
        if(compression != TIFF_COMPRESSION_NONE){
            // we don't support compression
            hasError = true;
            errorString = "Compressiong is not supported by this lib yet!\n";
            return nullptr;
        }

        if(samplePerPixel > 1 && plannerConfiguration != TIFF_PLANARCONFIG_PLANAR) {
            // we only support Planar config
            hasError = true;
            errorString = "Only planner config is supported as of now!\n";
            return nullptr;
        }

        if(width == 0 && height == 0) {
            hasError = true;
            errorString = "The slice does not contain Image\n";
            return nullptr;
        }

        if(bitsPerSample[sample] != 8
                && bitsPerSample[sample] != 16
                && bitsPerSample[sample] != 32) {
            hasError = true;
            errorString = "This library only supports 8, 16 and 32 bits per sample!\n";
            return nullptr;
        }

        fpos_t pos = handler->fh.tellg();
        hasError = false;

        if(stripCount > 0 && stripByteCounts && stripOffsets) {
            uint32_t imageSize = width* height;
            if(bitsPerSample[sample] == 8) {
                uint8_t* buffer = (uint8_t*)loadImageData<uint8_t>(imageSize);
                handler->fh.seekg(pos);
                return buffer;
            } else if(bitsPerSample[sample] == 16){
                uint16_t* buffer = (uint16_t*)loadImageData<uint16_t>(imageSize);
                handler->fh.seekg(pos);
                return  buffer;
            } else if(bitsPerSample[sample] == 32){
                uint32_t* buffer = (uint32_t*)loadImageData<uint32_t>(imageSize);
                handler->fh.seekg(pos);
                return  buffer;
            } else {
                hasError = true;
                errorString = "Tiff format not recognized\n";
            }
        }

        hasError = true;
        errorString = "File not open!\n";

        return nullptr;
    }


    void loadTag(TIFFTAGDATA& tagData) {
        switch(tagData.tagId) {
        case TIFF_FIELD_IMAGEWIDTH: {
            width = tagData.value[0];
            break;
        }
        case TIFF_FIELD_IMAGELENGTH: {
            height = tagData.value[0];
            break;
        }
        case TIFF_FIELD_BITSPERSAMPLE: {
            bitsPerSample = new uint16_t[tagData.dataCount];
            std::copy(tagData.value, tagData.value + tagData.dataCount, bitsPerSample);
            break;
        }
        case TIFF_FIELD_COMPRESSION: {
            compression = tagData.value[0];
            break;
        }
        case TIFF_FIELD_STRIPOFFSETS: {
            stripCount = tagData.dataCount;
            stripOffsets = new uint32_t[tagData.dataCount];
            std::fill_n(stripOffsets, tagData.dataCount, 0);
            std::copy(tagData.value, tagData.value + tagData.dataCount, stripOffsets);

            break;
        }
        case TIFF_FIELD_SAMPLESPERPIXEL: {
            samplePerPixel = tagData.value[0];
            break;
        }
        case TIFF_FIELD_ROWSPERSTRIP: {
            rowsPerStrip = tagData.value[0];
            break;
        }
        case TIFF_FIELD_SAMPLEFORMAT: {
            sampleFormat = tagData.value[0];
            break;
        }
        case TIFF_FIELD_IMAGEDESCRIPTION: {
            if(tagData.dataCount) {
                char* desc = new char[tagData.dataCount+1];
                std::fill_n(desc, tagData.dataCount+1, '\0');
                std::copy(tagData.value, tagData.value + tagData.dataCount, desc);
                description = std::string(desc);
            }
            break;
        }

        case TIFF_FIELD_STRIPBYTECOUNTS: {
            stripCount = tagData.dataCount;
            stripByteCounts = new uint32_t[tagData.dataCount];
            std::copy(tagData.value, tagData.value+tagData.dataCount, stripByteCounts);
            break;
        }
        case TIFF_FIELD_PLANARCONFIG: {
            plannerConfiguration = tagData.value[0];
            break;
        }
        case TIFF_FIELD_IMAGEJMETADATA:{
            std::cout << "ImageJ metadata tag " << std::endl;
            tagData.print();
            break;
        }
        default: break;

        }
    }

    void load() {
        handler->fh.seekg(startingOffset);
        ifd = new TIFIFD;

        readByte<uint16_t>(ifd->NumDirEntries);
        if(handler->needSwap) ifd->NumDirEntries = byteswap<uint16_t>(ifd->NumDirEntries);

        if(ifd->NumDirEntries <=0) {
            throw std::runtime_error("No entry found");
        }

        for(uint32_t i = 0; i < ifd->NumDirEntries; i++) {
            TIFTAG tag;
            readByte<TIFTAG>(tag);
            if(handler->needSwap) tag.swap();

            // load tag info locally
            TIFFTAGDATA tagData;
            tagData.loadData(handler, tag);

            loadTag(tagData);
        }


        readByte<uint32_t>(ifd->NextIFDOffset);
        if(handler->needSwap) ifd->NextIFDOffset = byteswap<uint32_t>(ifd->NextIFDOffset);

        nextIFDOffset = ifd->NextIFDOffset;
        hasNext = (nextIFDOffset >0);

        delete ifd;
    }

    uint32_t startingOffset;
    //    std::ifstream* fh;
    TIFIFD* ifd;
    bool hasNext;
    uint32_t nextIFDOffset;
    FileHandler* handler;
    bool hasError;
    std::string errorString;

    // tag info
    uint32_t width;
    uint32_t height;
    uint16_t *bitsPerSample;
    uint16_t compression;
    uint32_t stripCount;
    uint32_t *stripOffsets;
    uint16_t samplePerPixel;
    uint32_t rowsPerStrip;
    uint16_t sampleFormat;
    std::string description = "";
    uint32_t *stripByteCounts;
    uint16_t plannerConfiguration;

};

struct TiffReaderData {

    TiffReaderData(const std::string& path){
        handler = new FileHandler;
        handler->fileName = path;
        handler->fh.open(handler->fileName.c_str(), std::ios::in | std::ios::binary);
        handler->fileSize = std::filesystem::file_size(handler->fileName);
        load();
    }

    template<typename T>
    std::istream & read(T& value){
        return handler->fh.read(reinterpret_cast<char*>(&value), sizeof(T));
    }

    void load() {
        header = new TIFHEAD;
        if(!read<TIFHEAD>(*header)) {
            throw std::runtime_error("Empty file");
        }
        if(header->Identifier != getByteorder()) {
            handler->needSwap = true;
        } else {
            handler->needSwap = false;
        }
        if(handler->needSwap) header->swap();

        uint32_t nextOffset = header->IFDOffset;
        while (nextOffset > 0 && nextOffset < handler->fileSize) {
            currentSlice = new TiffSlice(nextOffset, handler);
            slices.push_back(currentSlice);
            nextOffset = currentSlice->nextIFDOffset;
        }

        currentSlice = slices[currentSliceIndex];
    }

    // all variables
    FileHandler* handler;
    TiffSlice* currentSlice;
    std::vector<TiffSlice*> slices;
    TIFHEAD* header;
    uint32_t currentSliceIndex =0;
};

TiffReader::TiffReader(const std::string &filename)
{
    d = new TiffReaderData(filename);
}

TiffReader::~TiffReader()
{
    delete d;
}

uint32_t TiffReader::width() const
{
    return d->currentSlice->width;
}

uint32_t TiffReader::height() const
{
    return d->currentSlice->height;
}

uint32_t TiffReader::sliceCount() const
{
    return d->slices.size();
}

std::string TiffReader::description() const
{
    return  d->currentSlice->description;
}

uint16_t TiffReader::bitsPerSample() const
{
    if(d->currentSlice->bitsPerSample) {
        return d->currentSlice->bitsPerSample[0];
    }
    return 0;
}

uint16_t TiffReader::sampleFormat() const
{
    return d->currentSlice->sampleFormat;
}

void *TiffReader::readImage()
{
    return d->currentSlice->read();
}

void TiffReader::next()
{
    d->currentSliceIndex++;
    if(d->currentSliceIndex < d->slices.size()) d->currentSlice = d->slices[d->currentSliceIndex];
}

bool TiffReader::hasNext()
{
    return ((d->currentSliceIndex) < d->slices.size());
}
}
