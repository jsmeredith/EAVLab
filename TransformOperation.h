// Copyright 2012 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef OP_TRANSFORM_H
#define OP_TRANSFORM_H

#include "Operation.h"

#include <eavlTransformMutator.h>

class TransformAttributes : public Attribute
{
  public:
    float sx, sy, sz; // for now.
    float tx, ty, tz; // for now..
  public:
    virtual const char *GetType() {return "TransformAttributes";}
    TransformAttributes() : Attribute()
    {
        sx = sy = sz = 1.;
        tx = ty = tz = 0.; 
    }
    virtual ~TransformAttributes()
    {
    }
    virtual void AddFields()
    {
        Add("sx", sx);
        Add("sy", sy);
        Add("sz", sz);
        Add("tx", tx);
        Add("ty", ty);
        Add("tz", tz);
    }
    
};

// ****************************************************************************
// Class:  TransformOperation
//
// Purpose:
///   Operation that sets a matrix transform within the coordinate system.
///   Note: this currently has no settings.
//
// Programmer:  Brad Whitlock
// Creation:    September 19, 2012
//
// Modifications:
// ****************************************************************************
class TransformOperation : public Operation
{
    TransformAttributes  *atts;
    eavlTransformMutator *mutator;
  public:
  //  Is
    TransformOperation()
        : Operation()
    {
        atts = new TransformAttributes;
        mutator = new eavlTransformMutator;
    }
    virtual std::string GetOperationName()
    {
        return "Transform";
    }
    virtual std::string GetOperationInfo()
    {
        ostringstream os;
        os << "sx=" << atts->sx << endl;
        os << "sy=" << atts->sy << endl;
        os << "sz=" << atts->sz << endl;
        os << "tx=" << atts->tx << endl;
        os << "ty=" << atts->ty << endl;
        os << "tz=" << atts->tz << endl;
        return os.str();        
    }
    virtual Attribute *GetSettings()
    {
        return atts;
    }

    virtual void Execute()
    {
        mutator->SetDataSet(input);

        // Build up transform from different components. Do more here, add rotation, etc.
        eavlMatrix4x4 M;
        M(0,0) = atts->sx;
        M(1,1) = atts->sy;
        M(2,2) = atts->sz;
        M(3,0) = atts->tx;
        M(3,1) = atts->ty;
        M(3,2) = atts->tz;
        mutator->SetTransform(M);

        ///\todo: assuming last cell set
 //       mutator->SetCellSet(input->GetCellSet(input->GetNumCellSets()-1)->GetName());
        mutator->Execute();
        output = input;
    }
};

#endif
