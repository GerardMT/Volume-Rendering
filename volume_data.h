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

    vector<double> histogram_;

    int width_;
    int height_;
    int depth_;

    GLuint texture_ = 0;
};

#endif // VOLUMEDATA_H
