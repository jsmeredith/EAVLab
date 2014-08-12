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
    string field;
    float minvalue;
    float maxvalue;
  public:
    virtual const char *GetType() {return "ThresholdAttributes";}
    ThresholdAttributes() : Attribute()
    {
        field = "(default)";
        minvalue = -FLT_MAX;
        maxvalue = +FLT_MAX;
    }
    virtual ~ThresholdAttributes()
    {
    }
    virtual void AddFields()
    {
        Add("field", field);
        Add("minvalue", minvalue);
        Add("maxvalue", maxvalue);
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
        ///\todo: assuming cell set 0
        mutator->SetCellSet(input->GetCellSet(0)->GetName());
        mutator->SetField(atts->field);
        mutator->SetRange(atts->minvalue, atts->maxvalue);
        mutator->Execute();
        output = input;
    }
};

#endif
