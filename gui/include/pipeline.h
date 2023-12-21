#ifndef PIPELINE_H
#define PIPELINE_H

class Pipeline {
public:
    struct Image {
        unsigned char *data;
        int width;
        int height;
        int bytesPerLine;
        Image(unsigned char * data, int width, int height, int bytesPerLine): data(data), width(width), height(height), bytesPerLine(bytesPerLine){};
    };
    virtual Pipeline::Image execute(Pipeline::Image image) =0;
};


#endif

