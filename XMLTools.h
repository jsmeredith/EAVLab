// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef XML_TOOLS_H
#define XML_TOOLS_H

#include "STL.h"
#include "Exception.h"

// ****************************************************************************
//  Class:  XMLParser
//
//  Purpose:
//    Parse an XML file.  To use it, simply derive a new class from the base
//    XMLParser and override the virtual functions beginElement, endElement,
//    handleText, and optionally handleComment.
//
//  Programmer:  Jeremy Meredith
//  Creation:    February 25, 2008
//
// ****************************************************************************

enum XMLToken {
    TokOpen,
    TokClose,
    TokEqual,
    TokSlash,
    TokBang,
    TokQuestion,
    TokString,
    TokLiteral,
    TokEOF,
    TokError,
    TokNone
};

class XMLScanner
{
  public:
    XMLScanner(istream &input);
    XMLToken GetNextToken(char *outbuff);
    int      GetCurrentLine();
  private:
    istream  &in;
    char      c;
    bool      havePeek;
    int       currentLine;
};


struct XMLAttribute
{
    string type;
    string value;

    XMLAttribute()  { }
    XMLAttribute(string t, string v) : type(t), value(v) { }
    XMLAttribute(string t, int n) : type(t)
    {
        char str[256];
        sprintf(str, "%d", n);
        value = str;
    }
};

struct XMLAttributes
{
    vector<XMLAttribute> atts;

    XMLAttribute &operator[](int i) { return atts[i]; }
    const XMLAttribute &operator[](int i) const { return atts[i]; }
    int size() const { return atts.size(); }
    void push_back(const XMLAttribute &att) { atts.push_back(att); }
    const string &GetValue(const string &type) const
    {
        for (unsigned int i=0; i<atts.size(); i++)
            if (atts[i].type == type)
                return atts[i].value;
        throw Exception("Value didn't exist");
    }
};

class XMLParser
{
  public:
    XMLParser();
    virtual ~XMLParser();
    void Initialize(istream &input);
    void ParseSingleEntity();
    void ParseAllEntities();

  protected:
    virtual void beginElement(const string &name,const XMLAttributes &atts) = 0;
    virtual void handleText(const string &text) = 0;
    virtual void handleComment(const string &) {}; // no-op default is good
    virtual void endElement(const string &name) = 0;
    istream *savedInput;

  private:
    XMLScanner *scanner;
    XMLToken token;
    char *currentText;
    char *acceptedText;
    char buff1[4096];
    char buff2[4096];
    void GetNextToken();
    bool Accept(XMLToken t);
    void Expect(XMLToken t);
    bool ParseXMLNestingAfterOpenBracket();
};

class XMLWriter
{
  public:
    XMLWriter();
    void Open(ostream &output);
    void BeginElement(const std::string&);
    void AddAttribute(const XMLAttribute&);
    void AddData(bool);
    void AddData(int);
    void AddData(long long);
    void AddData(float);
    void AddData(double);
    void AddData(const std::string&);
    void EndElement(bool allowSingleLine=false);
    void Close();
  protected:
    void Indent();
    void BeginAddData();
    void EndAddData();
    ostream *out;
    vector<string> elementStack;
    bool stillBeginningElement;
};


#endif
