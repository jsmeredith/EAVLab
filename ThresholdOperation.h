// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef OP_THRESHOLD_H
#define OP_THRESHOLD_H

#include "Operation.h"

#include <eavlThresholdMutator.h>

// ****************************************************************************
// Class:  ThresholdAttributes
//
// Purpose:
///   Attributes for the threshold operation.  Currently, one var and one
///   target value.
//
// Programmer:  Jeremy Meredith
// Creation:    August 12, 2014
//
// Modifications:
// ****************************************************************************
class ThresholdAttributes : public Attribute
{
  public:
    string field, cellset;
    float minvalue;
    float maxvalue;
    bool all_points_required;
  public:
    virtual const char *GetType() {return "ThresholdAttributes";}
    ThresholdAttributes() : Attribute()
    {
        field = "(default)";
        cellset = "(default)";
        minvalue = -FLT_MAX;
        maxvalue = +FLT_MAX;
    }
    virtual ~ThresholdAttributes()
    {
    }
    virtual void AddFields()
    {
        Add("Field name", field);
	Add("Cell set", cellset);
        Add("Minimum value", minvalue);
        Add("Maximum value", maxvalue);
        Add("Require all cell points in range for nodal fields", all_points_required);
    }
    
};

// ****************************************************************************
// Class:  ThresholdOperation
//
// Purpose:
///   Operation that's an threshold.
//
// Programmer:  Jeremy Meredith
// Creation:    August 12, 2014
//
// Modifications:
// ****************************************************************************
class ThresholdOperation : public Operation
{
    ThresholdAttributes *atts;
    eavlThresholdMutator *mutator;
  public:
    ThresholdOperation()
        : Operation()
    {
        atts = new ThresholdAttributes;
        mutator = new eavlThresholdMutator;
    }
    virtual std::string GetOperationName()
    {
        return "Threshold";
    }
    virtual std::string GetOperationShortName()
    {
        return "thresh";
    }
    virtual std::string GetOperationInfo()
    {
        ostringstream os;
        os << atts->minvalue << "<=" << atts->field << "<=" << atts->maxvalue;
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
        return vars;
    }
    virtual void Execute()
    {
        mutator->SetDataSet(input);
	if (atts->cellset == "(default)")
	    mutator->SetCellSet(input->GetCellSet(0)->GetName());
	else
	    mutator->SetCellSet(atts->cellset);
        mutator->SetField(atts->field);
        mutator->SetRange(atts->minvalue, atts->maxvalue);
        mutator->SetNodalThresholdAllPointsRequired(atts->all_points_required);
        mutator->Execute();
        output = input;
    }
};

#endif
