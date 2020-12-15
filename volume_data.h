#ifndef VOLUMEDATA_H
#define VOLUMEDATA_H

#include <GL/glew.h>

#include <vector>
#include <string>

using namespace std;

class VolumeData
{
public:
    ~VolumeData();

    void readFromDicom(const string &filename);

    bool initialized();

    vector<float> histogram_;

    int width_;
    int height_;
    int depth_;

    GLuint texture_ = 0;

private:
    bool initialized_ = false;
};

#endif // VOLUMEDATA_H
