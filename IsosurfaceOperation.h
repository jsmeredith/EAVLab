// Copyright 2012 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef OP_ISOSURFACE_H
#define OP_ISOSURFACE_H

#include "Operation.h"

#include <eavlIsosurfaceFilter.h>

// ****************************************************************************
// Class:  IsosurfaceAttributes
//
// Purpose:
///   Attributes for the isosurface operation.  Currently, one var and one
///   target value.
//
// Programmer:  Jeremy Meredith
// Creation:    August 9, 2012
//
// Modifications:
// ****************************************************************************
class IsosurfaceAttributes : public Attribute
{
  public:
    string field;
    float value;
  public:
    virtual const char *GetType() {return "IsosurfaceAttributes";}
    IsosurfaceAttributes() : Attribute()
    {
        field = "(default)";
        value = -1;
    }
    virtual ~IsosurfaceAttributes()
    {
    }
    virtual void AddFields()
    {
        Add("field", field);
        Add("value", value);
    }
    
};

// ****************************************************************************
// Class:  IsosurfaceOperation
//
// Purpose:
///   Operation that's an isosurface.
//
// Programmer:  Jeremy Meredith
// Creation:    August 9, 2012
//
// Modifications:
// ****************************************************************************
class IsosurfaceOperation : public Operation
{
    IsosurfaceAttributes *atts;
    eavlIsosurfaceFilter *filter;
  public:
  //  Is
    IsosurfaceOperation()
        : Operation()
    {
        atts = new IsosurfaceAttributes;
        filter = new eavlIsosurfaceFilter;
    }
    virtual std::string GetOperationName()
    {
        return "Isosurface";
    }
    virtual std::string GetOperationInfo()
    {
        ostringstream os;
        os << atts->field << "=" << atts->value;
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
        if (false)
        {
            // these were mostly internal debug fields from
            // the isosurface; we stopped exposing them for
            // now but might want them for debug again later
            vars.push_back("revInputIndex");
            vars.push_back("revInputSubindex");
            vars.push_back("alpha");
            vars.push_back("newx");
            vars.push_back("newy");
            vars.push_back("newz");
        }
        return vars;
    }
    virtual void Execute()
    {
        filter->SetInput(input);
        ///\todo: assuming cell set 0
        filter->SetCellSet(input->GetCellSet(0)->GetName());
        filter->SetField(atts->field);
        filter->SetIsoValue(atts->value);
        filter->Execute();
        output = filter->GetOutput();
    }
};

#endif
