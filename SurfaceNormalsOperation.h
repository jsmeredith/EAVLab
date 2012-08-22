// Copyright 2012 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef OP_SURFACE_NORMALS_H
#define OP_SURFACE_NORMALS_H

#include "Operation.h"

#include <eavlSurfaceNormalMutator.h>

// ****************************************************************************
// Class:  SurfaceNormalsOperation
//
// Purpose:
///   Operation that creates a new field that set that is the surface
///   normals from another cell set.
///   Note: this currently has no settings.
//
// Programmer:  Jeremy Meredith
// Creation:    August 20, 2012
//
// Modifications:
// ****************************************************************************
class SurfaceNormalsOperation : public Operation
{
    eavlSurfaceNormalMutator *mutator;
  public:
  //  Is
    SurfaceNormalsOperation()
        : Operation()
    {
        mutator = new eavlSurfaceNormalMutator;
    }
    virtual std::string GetOperationName()
    {
        return "SurfaceNormals";
    }
    virtual std::string GetOperationInfo()
    {
        return "-";
    }
    virtual Attribute *GetSettings()
    {
        return NULL;
    }
    virtual void Execute()
    {
        mutator->SetDataSet(input);
        ///\todo: assuming last cell set
        mutator->SetCellSet(input->cellsets[input->cellsets.size()-1]->GetName());
        mutator->Execute();
        output = input;
    }
};

#endif
