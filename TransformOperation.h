// Copyright 2012 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef OP_TRANSFORM_H
#define OP_TRANSFORM_H

#include "Operation.h"

#include <eavlTransformMutator.h>

class TransformAttributes : public Attribute
{
  public:
    bool  transformCoordinates;
    int   csIndex;
    float rx, ry, rz;
    float sx, sy, sz;
    float tx, ty, tz;
  public:
    virtual const char *GetType() {return "TransformAttributes";}
    TransformAttributes() : Attribute()
    {
        transformCoordinates = false;
        csIndex = 0;
        rx = ry = rz = 0.;
        sx = sy = sz = 1.;
        tx = ty = tz = 0.; 
    }
    virtual ~TransformAttributes()
    {
    }
    virtual void AddFields()
    {
        Add("Transform Coordinate Data", transformCoordinates);
        Add("Coordinate System Index", csIndex);

        Add("rx", rx);
        Add("ry", ry);
        Add("rz", rz);
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
        os << "Rotate" << endl
           << "   x=" << atts->rx << endl
           << "   y=" << atts->ry << endl
           << "   z=" << atts->rz << endl;
        os << "Scale" << endl
           << "   x=" << atts->sx << endl
           << "   y=" << atts->sy << endl
           << "   z=" << atts->sz << endl;
        os << "Translate" << endl
           << "   x=" << atts->tx << endl
           << "   y=" << atts->ty << endl
           << "   z=" << atts->tz;
        return os.str();        
    }
    virtual Attribute *GetSettings()
    {
        return atts;
    }

    virtual void Execute()
    {
        mutator->SetDataSet(input);
 
        mutator->SetCoordinateSystemIndex(atts->csIndex);
        mutator->SetTransformCoordinates(atts->transformCoordinates);

        // Build up transform from different components. Do more here, add rotation, etc.
        eavlMatrix4x4 M;
        M(0,0) = atts->sx;
        M(1,1) = atts->sy;
        M(2,2) = atts->sz;
        M(3,0) = atts->tx;
        M(3,1) = atts->ty;
        M(3,2) = atts->tz;

        eavlMatrix4x4 rx,ry,rz;
        rx.CreateRotateX(atts->rx * (3.141592653589793 / 180.));
        ry.CreateRotateY(atts->ry * (3.141592653589793 / 180.));
        rz.CreateRotateZ(atts->rz * (3.141592653589793 / 180.));

        M = rx * ry * rz * M;
        mutator->SetTransform(M);

        mutator->Execute();
        output = input;
    }
};

#endif
