
#include "histogram_widget.h"

#include <QHBoxLayout>
#include <QLabel>

static const vector<string> CHANNEL_NAMES = {"Red", "Green", "Blue", "Alpha"};

HistogramWidget::HistogramWidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);

    for (int i = 0; i < NUM_CHANNELS; ++i) {
        channel_[i] = new HistogramChannelWidget(i);
        channel_[i]->histogramWidget(*this);
        channel_initialized_[i] = false;

        QLabel *label = new QLabel(CHANNEL_NAMES[i].c_str());
        label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        layout->addWidget(label);
        layout->addWidget(channel_[i]);
    }

    setLayout(layout);
}

HistogramWidget::~HistogramWidget()
{
    for (int i = 0; i < NUM_CHANNELS; ++i) {
        delete channel_[i];
    }
}

void HistogramWidget::volumeData(VolumeData &volumeData)
{
    for (int i = 0; i < NUM_CHANNELS; ++i) {
        channel_[i]->volumeData(volumeData);
    }
}

void HistogramWidget::volumeDataUpdated()
{
    for (int i = 0; i < NUM_CHANNELS; ++i) {
        channel_[i]->volumeDataUpdated();
    }
}

void HistogramWidget::dataUpdated()
{
    if (!initialized_) {
        return;
    }

    vector<glm::vec4> data;
    data.resize(256);

    for (int i = 0; i < 256; ++i) {
        for (int j = 0; j < NUM_CHANNELS; ++j) {
            data[i][j] = channel_[j]->data_[i];
        }
    }

    histogram_widget_callback_->histogramUpdated(data);
}

bool HistogramWidget::initialized()
{
    return initialized_;
}

void HistogramWidget::initialized(int channel)
{
    channel_initialized_[channel] = true;

    initialized_ = true;
    for (int i = 0; i < NUM_CHANNELS; ++i) {
        initialized_ &= channel_initialized_[i];
    }
}


