// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef EL_RENDER_OPTIONS_H
#define EL_RENDER_OPTIONS_H

#include "ELAttributeControl.h"

class RenderingAttributes : public Attribute
{
  public:
    float Ka;
    float Kd;
    float Ks;
    bool eyeLight;
    float Lx, Ly, Lz;
  public:
    virtual const char *GetType() {return "RenderingAttributes";}
    RenderingAttributes() : Attribute()
    {
        Ka = 0.2;
        Kd = 0.8;
        Ks = 0.2;

        eyeLight = true;

        Lx = 0.2;
        Ly = 0.2;
        Lz = 1.0;

    }
    virtual ~RenderingAttributes()
    {
    }
    virtual void AddFields()
    {
        Add("Ka",Ka);
        Add("Kd",Kd);
        Add("Ks",Ks);
        Add("eyeLight",eyeLight);
        Add("Lx",Lx);
        Add("Ly",Ly);
        Add("Lz",Lz);
    }
};

class ELRenderOptions : public ELAttributeControl
{
    Q_OBJECT
  public:
    ELRenderOptions(QWidget *parent);
    virtual void ConnectAttributes(Attribute *a);
};

#endif
