
#include "volume_data.h"

#include <boost/filesystem.hpp>
#include <QImage>
#include <iostream>

bool compare(const boost::filesystem::path& a, const boost::filesystem::path& b)
{
    if (a.size() == b.size()) {
      return a < b;
    } else {
       return a.size() < b.size();
    }
}

VolumeData::~VolumeData()
{
    glDeleteTextures(1, &texture_);
}

void VolumeData::readFromDicom(const std::string& path)
{
    const boost::filesystem::path kDir = boost::filesystem::path(path);

    if (!boost::filesystem::exists(kDir) || !boost::filesystem::is_directory(kDir)) {
        throw 1;
    }

    histogram_.clear();
    histogram_.resize(256, 0);

    std::vector<boost::filesystem::path> paths(boost::filesystem::directory_iterator{kDir}, boost::filesystem::directory_iterator{});
    std::sort(paths.begin(), paths.end(), compare);

    depth_ = 0;
    std::vector<uchar> data;
    for (auto const& file_path : paths) {
        if (boost::filesystem::is_regular_file(file_path) && file_path.extension() == ".jpg") {
            cout << file_path.string() << std::endl;

            QImage img(QString::fromStdString(file_path.string()));

            if (img.isNull()) {
                throw 1;
            }

            if (depth_ == 0) {
                width_ = img.width();
                height_ = img.height();
            } else if (width_ != img.width() || height_ != img.height()) {
                throw 1;
            }

            depth_++;

            for (int i = 0; i < width_; ++i) {
                for (int j = 0; j < height_; ++j) {
                    data.push_back(img.pixel(j, i) & 0xFF);
                }
            }
        }
    }

    const int kDataSize = data.size();

    // Generate the 3D texture.
    for (int i = 0; i < kDataSize; i++) {
        histogram_[static_cast<int>(data[i])] += 1.0;
    }

    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_3D, texture_);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, width_, height_, depth_, 0, GL_RED, GL_UNSIGNED_BYTE, &data[0]);
    glGenerateMipmap(GL_TEXTURE_3D);

    std::vector<double> sorted_histogram_;
    sorted_histogram_.insert(sorted_histogram_.begin(), histogram_.begin(), histogram_.end());
    sort(sorted_histogram_.begin(), sorted_histogram_.end());

    const double kMaximum = sorted_histogram_[sorted_histogram_.size() * 0.98f];

    const int kHistSize = histogram_.size();
    for (int i = 0; i < kHistSize; ++i) {
        histogram_[i] = histogram_[i] / kMaximum;
    }

    cout << "Volume loaded, 3D texture built: " << width_ << " x " << height_ << " x " << depth_ << std::endl;
}

