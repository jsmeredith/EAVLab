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
    bool valid;

    // these two are hacks; need a better way to get this info
    // to create the right renderers for plots....
    bool oneDimensional;
    bool barsFor1D;

    Plot() : pipe(NULL),
             colortable("default"),
             cellset(""),
             field(""),
             color(eavlColor::grey50),
             wireframe(false),
             renderer(NULL),
             valid(true)
    {
        oneDimensional = false;
        barsFor1D = false;
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

        try
        {
            if (oneDimensional)
            {
                if (barsFor1D)
                {
                    if (field != "")
                        renderer = new eavlBarRenderer(pipe->results.back(),
                                                       color,
                                                       0.10,
                                                       cellset,
                                                       field);
                }
                else
                {
                    if (field != "")
                        renderer = new eavlCurveRenderer(pipe->results.back(),
                                                         color,
                                                         cellset,
                                                         field);
                }
            }
            else
            {
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
            valid = true;
        }
        catch (...)
        {
            renderer = NULL;
            valid = false;
        }
    }
};

#endif
