// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef OP_HISTOGRAM_H
#define OP_HISTOGRAM_H

#include "Operation.h"

#include <eavlScalarBinFilter.h>

// ****************************************************************************
// Class:  HistogramAttributes
//
// Purpose:
///   Attributes for the surface normal operation.  Currently, just
///   a boolean flag whether to add the nodal version, or just have
///   face normals.
//
// Programmer:  Jeremy Meredith
// Creation:    January 17, 2013
//
// Modifications:
// ****************************************************************************
class HistogramAttributes : public Attribute
{
  public:
    string field;
    int nbins;
  public:
    virtual const char *GetType() {return "HistogramAttributes";}
    HistogramAttributes() : Attribute()
    {
        field = "(default)";
        nbins = 10;
    }
    virtual ~HistogramAttributes()
    {
    }
    virtual void AddFields()
    {
        Add("field", field);
        Add("nbins", nbins);
    }
    
};

// ****************************************************************************
// Class:  HistogramOperation
//
// Purpose:
///   Operation that creates a new field that set that is the surface
///   normals from another cell set.
///   Note: this currently has no settings.
//
// Programmer:  Jeremy Meredith
// Creation:    January 17, 2013
//
// Modifications:
//
// ****************************************************************************
class HistogramOperation : public Operation
{
    HistogramAttributes *atts;
    eavlScalarBinFilter *filter;
  public:
    HistogramOperation()
        : Operation()
    {
        atts = new HistogramAttributes;
        filter = new eavlScalarBinFilter;
    }
    virtual std::string GetOperationName()
    {
        return "Histogram";
    }
    virtual std::string GetOperationShortName()
    {
        return "hist";
    }
    virtual std::string GetOperationInfo()
    {
        ostringstream os;
        os << atts->field << " in " << atts->nbins << " bins";
        return os.str();
    }
    virtual Attribute *GetSettings()
    {
        return atts;
    }
    virtual std::vector<std::string> GetNeededVariables()
    {
        std::vector<std::string> vars;
        if (atts->field != "(default)")
            vars.push_back(atts->field);
        return vars;
    }
    virtual std::vector<std::string> GetOutputVariables()
    {
        std::vector<std::string> vars;
        vars.push_back("counts");
        return vars;
    }
    virtual void Execute()
    {
        filter->SetInput(input);
        filter->SetField(atts->field);
        filter->SetNumBins(atts->nbins);
        filter->Execute();

        output = filter->GetOutput();
    }
};

#endif
