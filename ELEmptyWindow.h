// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef EL_EMPTY_WINDOW_H
#define EL_EMPTY_WINDOW_H

#include "ELWindowManager.h"

#include <QTextEdit>

class Pipeline;

// ****************************************************************************
// Class:  ELEmptyWindow
//
// Purpose:
///   Output window containing, well, nothing.
//
// Programmer:  Jeremy Meredith
// Creation:    August 20, 2012
//
// Modifications:
// ****************************************************************************
class ELEmptyWindow : public QLabel
{
    Q_OBJECT
  protected:
  public:
    ELEmptyWindow(ELWindowManager *parent);
  public slots:
    void PipelineUpdated(int index, Pipeline *p);
    void watchedPipelinesChanged(vector<bool>);
    void CurrentPipelineChanged(int index);
};



#endif
