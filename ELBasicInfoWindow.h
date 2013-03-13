// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef EL_BASIC_INFO_WINDOW_H
#define EL_BASIC_INFO_WINDOW_H

#include "ELWindowManager.h"

#include <QTextEdit>

class Pipeline;

// ****************************************************************************
// Class:  ELBasicInfoWindow
//
// Purpose:
///   Output window containing basic info about the source.
//
// Programmer:  Jeremy Meredith
// Creation:    August  3, 2012
//
// Modifications:
// ****************************************************************************
class ELBasicInfoWindow : public QWidget
{
    Q_OBJECT
  protected:
    QTextEdit     *info;
    std::vector<bool> watchedPipelines;
  public:
    ELBasicInfoWindow(ELWindowManager *parent);
    void FillFromPipeline(Pipeline *p);
  public slots:
    void PipelineUpdated(int index, Pipeline *p);
    void CurrentPipelineChanged(int index);
};



#endif
