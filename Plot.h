// Copyright 2010-2013 UT-Battelle, LLC. See LICENSE.txt for more information.
#ifndef PLOT_H
#define PLOT_H

#include "eavl.h"
#include "eavlView.h"
#include "eavlPlot.h"
#include "eavlColorTable.h"

struct Plot
{
    Pipeline *pipe;
    string colortable;
    bool reversect;
    bool logct;
    string cellset;
    string field;
    eavlColor color;
    bool wireframe;
    void (*xform)(double,double,double,double&,double&,double&);
    eavlPlot *eavlplot;
    bool valid;

    // these two are hacks; need a better way to get this info
    // to create the right type of plots....
    bool oneDimensional;
    bool barsFor1D;

    Plot() : pipe(NULL),
             colortable("default"),
             reversect(false),
             logct(false),
             cellset(""),
             field(""),
             color(eavlColor::grey50),
             wireframe(false),
             xform(NULL),
             eavlplot(NULL),
             valid(true)
    {
        oneDimensional = false;
        barsFor1D = false;
    }
    void UpdateDataSet(eavlDataSet *)
    {
        //cerr << "update data set\n";
        delete eavlplot;
        eavlplot = NULL;
    }
    void CreateEAVLPlot()
    {
        try
        {
            // Create the EAVL Plot if needed
            //cerr << "ELPlot: creating? " << (eavlplot) << endl;
            if (!eavlplot)
            {
                if (oneDimensional)
                {
                    eavlplot = new eavl1DPlot(pipe->results.back(), cellset);
                    dynamic_cast<eavl1DPlot*>(eavlplot)->SetBarStyle(barsFor1D);
                }
                else
                {
                    eavlplot = new eavlPlot(pipe->results.back(), cellset);
                    //cerr << "new eavlplot = " << eavlplot << endl;
                }
            }

            // update its values

            if (xform)
                eavlplot->SetTransformFunction(xform);

            eavlplot->SetField(field);
            eavlplot->SetSingleColor(color);
            eavlplot->SetWireframe(wireframe);
            eavlplot->SetColorTableByName(colortable,reversect);
            eavlplot->SetLogarithmicColorScaling(logct);

            valid = true;
        }
        catch (...)
        {
            eavlplot = NULL;
            valid = false;
        }
    }
};

#endif
