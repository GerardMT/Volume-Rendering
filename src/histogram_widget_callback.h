
#ifndef HISTOGRAMWIDGETCALLBACK_H
#define HISTOGRAMWIDGETCALLBACK_H

#include <glm/glm.hpp>
#include <vector>

using namespace std;

class HistogramWidgetCallback
{
public:
    virtual ~HistogramWidgetCallback() {};

    virtual void histogramUpdated(vector<glm::vec4> &data) = 0;

};

#endif // HISTOGRAMWIDGETCALLBACK_H
