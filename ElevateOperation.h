#ifndef OP_ELEVATE_H
#define OP_ELEVATE_H

#include "Operation.h"
#include <eavlElevateMutator.h>

class ElevateAttributes : public Attribute
{
  public:
    string field;
  public:
    virtual const char *GetType() {return "ElevateAttributes";}
    ElevateAttributes() : Attribute()
    {
        field = "(default)";
    }
    virtual ~ElevateAttributes()
    {
    }
    virtual void AddFields()
    {
        Add("field", field);
    }    
};

class ElevateOperation : public Operation
{
    ElevateAttributes  *atts;
    eavlElevateMutator *mutator;
  public:
    ElevateOperation()
        : Operation()
    {
        atts = new ElevateAttributes;
        mutator = new eavlElevateMutator;
    }
    virtual std::string GetOperationName()
    {
        return "Elevate";
    }
    virtual std::string GetOperationShortName()
    {
        return "elev";
    }
    virtual std::string GetOperationInfo()
    {
        return atts->field;
    }
    virtual Attribute *GetSettings()
    {
        return atts;
    }
    virtual std::vector<std::string> GetNeededVariables()
    {
        std::vector<std::string> vars;
        if (atts->field != "")
            vars.push_back(atts->field);
        return vars;
    }
    virtual std::vector<std::string> GetOutputVariables()
    {
        return std::vector<std::string>();
    }
    virtual void Execute()
    {
        mutator->SetDataSet(input);
        mutator->SetField(atts->field);
        mutator->Execute();
        output = input;
    }
};

#endif
