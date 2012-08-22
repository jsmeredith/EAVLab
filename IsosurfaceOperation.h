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
    virtual void Execute()
    {
        filter->SetInput(input);
        ///\todo: assuming cell set 0
        filter->SetCellSet(input->cellsets[0]->GetName());
        filter->SetField(atts->field);
        filter->SetIsoValue(atts->value);
        filter->Execute();
        output = filter->GetOutput();
    }
};

#endif
