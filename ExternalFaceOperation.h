// Copyright 2012 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef OP_EXTERNAL_FACE_H
#define OP_EXTERNAL_FACE_H

#include "Operation.h"

#include <eavlExternalFaceMutator.h>

// ****************************************************************************
// Class:  ExternalFaceOperation
//
// Purpose:
///   Operation that creates a new cell set that is the external
///   faces of another cell set.
///   Note: this currently has no settings.
//
// Programmer:  Jeremy Meredith
// Creation:    August 9, 2012
//
// Modifications:
// ****************************************************************************
class ExternalFaceOperation : public Operation
{
    eavlExternalFaceMutator *mutator;
  public:
  //  Is
    ExternalFaceOperation()
        : Operation()
    {
        mutator = new eavlExternalFaceMutator;
    }
    virtual std::string GetOperationName()
    {
        return "ExternalFace";
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
