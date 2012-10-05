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
        bool rotating    = atts->rx != 0 || atts->ry != 0 || atts->rz != 0;
        bool scaling     = atts->sx != 1 || atts->sy != 1 || atts->sz != 1;
        bool translating = atts->tx != 0 || atts->ty != 0 || atts->tz != 0;

        if (!rotating && !scaling && !translating)
        {
            os << "none";
        }
        if ((rotating?1:0) + (scaling?1:0) + (translating?1:0) > 1)
        {
            if (rotating)
            {
                float ident = 0;
                os << "Rotate(";
                if (atts->rx!=ident) os << "x";
                if (atts->ry!=ident) os << "y";
                if (atts->rz!=ident) os << "z";
                os << "),";
            }
            if (scaling)
            {
                float ident = 1;
                os << "Scale(";
                if (atts->sx!=ident) os << "x";
                if (atts->sy!=ident) os << "y";
                if (atts->sz!=ident) os << "z";
                os << ")";
                if (translating)
                    os << ",";
            }
            if (translating)
            {
                float ident = 0;
                os << "Translate(";
                if (atts->tx!=ident) os << "x";
                if (atts->ty!=ident) os << "y";
                if (atts->tz!=ident) os << "z";
                os << ")";
            }
        }
        else if (rotating)
        {
            float ident = 0;
            os << "Rotate(";
            if (atts->ry==ident && atts->rz==ident)
                os << "x="<<atts->rx;
            else if (atts->rx==ident && atts->rz==ident)
                os << "y="<<atts->ry;
            else if (atts->rx==ident && atts->ry==ident)
                os << "z="<<atts->rz;
            else
                os << atts->rx<<","<<atts->ry<<","<<atts->rz;
            os << ")";
        }
        else if (scaling)
        {
            float ident = 1;
            os << "Scale(";
            if (atts->sy==ident && atts->sz==ident)
                os << "x="<<atts->sx;
            else if (atts->sx==ident && atts->sz==ident)
                os << "y="<<atts->sy;
            else if (atts->sx==ident && atts->sy==ident)
                os << "z="<<atts->sz;
            else
                os << atts->sx<<","<<atts->sy<<","<<atts->sz;
            os << ")";
        }
        else if (translating)
        {
            float ident = 0;
            os << "Translate(";
            if (atts->ty==ident && atts->tz==ident)
                os << "x="<<atts->tx;
            else if (atts->tx==ident && atts->tz==ident)
                os << "y="<<atts->ty;
            else if (atts->tx==ident && atts->ty==ident)
                os << "z="<<atts->tz;
            else
                os << atts->tx<<","<<atts->ty<<","<<atts->tz;
            os << ")";
        }
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
