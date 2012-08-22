// Copyright 2012 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef OPERATION_H
#define OPERATION_H

#include <QWidget>
#include <QLineEdit>
#include <QGridLayout>
#include <QLabel>

#include "STL.h"
#include "Attribute.h"

// ****************************************************************************
// Class:  Operation
//
// Purpose:
///   Base class for an Operation like isosurface or slicee.
//
// Programmer:  Jeremy Meredith
// Creation:    August 9, 2012
//
// Modifications:
// ****************************************************************************
class Operation
{
  protected:
    eavlDataSet *input;
    eavlDataSet *output;
  public:
    Operation() : input(NULL), output(NULL) { }
    /// Get the variables the operation is requesting.
    virtual std::vector<std::string> GetNeededVariables() { return std::vector<std::string>(); }
    /// Get the variables this operation creates.
    virtual std::vector<std::string> GetOutputvariables() { return std::vector<std::string>(); }
    /// Get the Attribute containing this operation's settings.
    virtual Attribute *GetSettings() = 0;
    /// Actual execution method for an operation.
    virtual void Execute() = 0;
    /// Set the input data set.
    void SetInput(eavlDataSet *ds) { input = ds; }
    /// Get the output data set.  This is only safe after Execute().
    eavlDataSet *GetOutput() { return output; }
    /// Get the user-visible name for the operation.
    virtual std::string GetOperationName() = 0;
    /// Get a short string describing the settings for this operation.
    virtual std::string GetOperationInfo() = 0;
};

#endif
