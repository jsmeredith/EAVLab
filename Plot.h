// Copyright 2010-2013 UT-Battelle, LLC. See LICENSE.txt for more information.
#ifndef PLOT_H
#define PLOT_H

#include "eavl.h"
#include "eavlView.h"
#include "eavlRenderer.h"
#include "eavlColorTable.h"

struct Plot
{
    Pipeline *pipe;
    string colortable;
    string cellset;
    string field;
    eavlColor color;
    bool wireframe;
    eavlRenderer *renderer;

    Plot() : pipe(NULL),
             colortable("default"),
             cellset(""),
             field(""),
             color(eavlColor::white),
             wireframe(false),
             renderer(NULL)
    {
    }
    void UpdateDataSet(eavlDataSet *ds)
    {
        delete renderer;
        renderer = NULL;
    }
    void CreateRenderer()
    {
        if (renderer)
            return;

        if (field != "")
        {
            renderer = new eavlPseudocolorRenderer(pipe->results.back(), 
                                                   colortable,
                                                   wireframe,
                                                   cellset,
                                                   field);
            return;
        }
        else
        {
            renderer = new eavlSingleColorRenderer(pipe->results.back(), 
                                                   color,
                                                   wireframe,
                                                   cellset);
        }
        
    }
};

#endif
