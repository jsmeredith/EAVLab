// Copyright 2010-2013 UT-Battelle, LLC. See LICENSE.txt for more information.
#ifndef PLOT_H
#define PLOT_H

#include "eavl.h"
#include "eavlView.h"
#include "eavlRenderer.h"
#include "eavlColorTable.h"

struct Plot
{
    eavlDataSet *data;
    string colortable;
    int cellset_index;
    int variable_fieldindex;
    //int variable_cellindex;
    eavlRenderer *renderer;

    Plot() : data(NULL),
             colortable("default"),
             cellset_index(-1),
             variable_fieldindex(-1),
             renderer(NULL)
    {
    }
};

#endif
