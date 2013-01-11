// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef OP_SURFACE_NORMALS_H
#define OP_SURFACE_NORMALS_H

#include "Operation.h"

#include <eavlSurfaceNormalMutator.h>
#include <eavlCellToNodeRecenterMutator.h>

// ****************************************************************************
// Class:  SurfaceNormalsAttributes
//
// Purpose:
///   Attributes for the surface normal operation.  Currently, just
///   a boolean flag whether to add the nodal version, or just have
///   face normals.
//
// Programmer:  Jeremy Meredith
// Creation:    November 29, 2012
//
// Modifications:
// ****************************************************************************
class SurfaceNormalsAttributes : public Attribute
{
  public:
    bool nodal;
  public:
    virtual const char *GetType() {return "SurfaceNormalsAttributes";}
    SurfaceNormalsAttributes() : Attribute()
    {
        nodal = true;
    }
    virtual ~SurfaceNormalsAttributes()
    {
    }
    virtual void AddFields()
    {
        Add("nodal", nodal);
    }
    
};

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
//   Jeremy Meredith, Thu Nov 29 12:20:34 EST 2012
//   Optionally recenter to a nodal variable.
//
// ****************************************************************************
class SurfaceNormalsOperation : public Operation
{
    SurfaceNormalsAttributes *atts;
    eavlSurfaceNormalMutator *mutator;
    eavlCellToNodeRecenterMutator *recenter;
  public:
    SurfaceNormalsOperation()
        : Operation()
    {
        atts = new SurfaceNormalsAttributes;
        mutator = new eavlSurfaceNormalMutator;
        recenter = new eavlCellToNodeRecenterMutator;
    }
    virtual std::string GetOperationName()
    {
        return "SurfaceNormals";
    }
    virtual std::string GetOperationInfo()
    {
        if (atts->nodal)
            return "node";
        else
            return "face";
    }
    virtual Attribute *GetSettings()
    {
        return atts;
    }
    virtual void Execute()
    {
        mutator->SetDataSet(input);
        ///\todo: assuming last cell set
        string cellset = input->GetCellSet(input->GetNumCellSets()-1)->GetName();
        mutator->SetCellSet(cellset);
        mutator->Execute();

        if (atts->nodal) // nodal surface normals
        {
            recenter->SetDataSet(input);
            recenter->SetField("surface_normals");
            recenter->SetCellSet(cellset);
            recenter->Execute();
        }

        output = input;
    }
};

#endif
