// Copyright 2012 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "ELEmptyWindow.h"

#include "Pipeline.h"

// ****************************************************************************
// Constructor:  ELEmptyWindow::ELEmptyWindow
//
// Programmer:  Jeremy Meredith
// Creation:    August 20, 2012
//
// Modifications:
// ****************************************************************************
ELEmptyWindow::ELEmptyWindow(ELWindowManager *parent)
    : QLabel("Please Select a Window Type", parent)
{
    setAlignment(Qt::AlignCenter);
}

// ****************************************************************************
// Method:  ELEmptyWindow::SetPipeline
//
// Purpose:
///   Set the pipeline this window is showing.
//
// Arguments:
//   p          the pipeline to show
//
// Programmer:  Jeremy Meredith
// Creation:    August 20, 2012
//
// Modifications:
// ****************************************************************************
void
ELEmptyWindow::PipelineUpdated(int, Pipeline *)
{
}


// ****************************************************************************
// Method:  ELEmptyWindow::watchedPipelinesChanged
//
// Purpose:
///   Change which pipelines this window should watch.
//
// Arguments:
//   watched    the new vector of size NUMPIPES+1 for new pipelines watch set
//
// Programmer:  Jeremy Meredith
// Creation:    August 20, 2012
//
// Modifications:
// ****************************************************************************
void
ELEmptyWindow::watchedPipelinesChanged(vector<bool>)
{
}

// ****************************************************************************
// Method:  ELEmptyWindow::CurrentPipelineChanged
//
// Purpose:
///   Tell this window what the currently-edited pipeline index is.
//
// Arguments:
//   index      the new pipeline being edited
//
// Programmer:  Jeremy Meredith
// Creation:    August 20, 2012
//
// Modifications:
// ****************************************************************************
void
ELEmptyWindow::CurrentPipelineChanged(int)
{
}
