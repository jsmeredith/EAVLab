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
    eavlRenderer *renderer;

    Plot() : pipe(NULL),
             colortable("default"),
             cellset(""),
             field(""),
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
                                                   cellset,
                                                   field);
            return;
        }
        else
        {
            renderer = new eavlSingleColorRenderer(pipe->results.back(), 
                                                   eavlColor::white,
                                                   cellset);
        }
        
    }
};

#endif
