
#ifndef HISTOGRAMWIDGET_H
#define HISTOGRAMWIDGET_H

#include "histogram_channel_widget.h"
#include "histogram_widget_callback.h"

#include <QWidget>

class HistogramWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HistogramWidget(QWidget *parent = nullptr);

    ~HistogramWidget();

    void volumeData(VolumeData &volumeData);

    void volumeDataUpdated();

    void dataUpdated();

    HistogramWidgetCallback *histogram_widget_callback_;

    bool initialized();

    void initialized(int channel);

private:
    static const int NUM_CHANNELS = 4;

    HistogramChannelWidget *channel_[NUM_CHANNELS];

    bool channel_initialized_[NUM_CHANNELS];

    bool initialized_ = false;
};

#endif // HISTOGRAMWIDGET_H
