// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "XMLTools.h"

const bool ErrorOnMismatchedTags = false;

static string XMLTokenTypeToString(XMLToken t)
{
    switch (t)
    {
      case TokOpen:    return "TokOpen";
      case TokClose:   return "TokClose";
      case TokEqual:   return "TokEqual";
      case TokSlash:   return "TokSlash";
      case TokBang:    return "TokBang";
      case TokQuestion:return "TokQuestion";
      case TokString:  return "TokString";
      case TokLiteral: return "TokLiteral";
      case TokEOF:     return "TokEOF";
      case TokError:   return "TokError";
      case TokNone:    return "TokNone";
    }
    return "Unknown token type"; // Logic error if we got here
}


XMLScanner::XMLScanner(istream &input)
    : in(input),
      c('\0'),
      havePeek(false),
      currentLine(1)
{
}

XMLToken XMLScanner::GetNextToken(char *outbuff)
{
    char *buff = outbuff;
    *buff = '\0';

    if (!havePeek)
    {
        c=in.get();
    }
    havePeek = false;

    // strip leading whitespace
    while (!in.eof() && (c==' ' || c=='\n' || c=='\t'))
    {
        if (c == '\n')
            currentLine++;
        c=in.get();
    }

    // Check for EOF
    if (in.eof())
    {
        return TokEOF;
    }

    // default case for simple token text; longer ones will override this.
    buff[0] = c;
    buff[1] = 0;

    // figure out what we've got
    if (c=='<')
        return TokOpen;
    else if (c=='>')
        return TokClose;
    else if (c=='=')
        return TokEqual;
    else if (c=='/')
        return TokSlash;
    else if (c=='!')
        return TokBang;
    else if (c=='?')
        return TokQuestion;
    else if (c=='\"')
    {
        c=in.get();
        while (c!='\"')
        {
#define ACCEPT_AMPERSAND_CODES
#ifdef ACCEPT_AMPERSAND_CODES
            if (c=='&')
            {
                string tmp;
                c=in.get();
                while (c != ';' && !in.eof())
                {
                    tmp += c;
                    c=in.get();
                }
                if (tmp == "quot")
                    *buff++ = '"';
                else if (tmp == "amp")
                    *buff++ = '&';
                else if (tmp == "lt")
                    *buff++ = '<';
                else if (tmp == "gt")
                    *buff++ = '>';
                else
                    cerr << "UNEXPECTED AMPERSAND CODE: "<<tmp<<endl;
            }
#endif
#define ACCEPT_BACKSLASH_CODES
#ifdef ACCEPT_BACKSLASH_CODES
            else if (c=='\\')
            {
                c=in.get();
                if      (c=='n')  *buff++ = '\n';
                else if (c=='t')  *buff++ = '\t';
                else if (c=='\\') *buff++ = '\\';
                else if (c=='x')
                {
                    char c1 = in.get();
                    char c2 = in.get();
                    int v1;
                    if (c1>='0' && c1<='9')
                        v1 = c1-'0';
                    else if (c1>='a' && c1<='f')
                        v1 = 10 + c1-'a';
                    else if (c1>='A' && c1<='F')
                        v1 = 10 + c1-'A';
                    else
                    {
                        // Error: non-hex digit in string.
                        v1 = -1;
                    }
                    int v2;
                    if (c2>='0' && c2<='9')
                        v2 = c2-'0';
                    else if (c2>='a' && c2<='f')
                        v2 = 10 + c2-'a';
                    else if (c2>='A' && c2<='F')
                        v2 = 10 + c2-'A';
                    else
                    {
                        // Error: non-hex digit in string.
                        v2 = -1;
                    }
                    if (v1 < 0 || v2 < 0)
                    {
                        // could handle this error better
                        //*buff++ = '\\';
                        //*buff++ = 'x';
                        //*buff++ = c1;
                        //*buff++ = c2;
                    }
                    else
                    {
                        // good hex-character representation
                        *buff++ = char(v1*16 + v2);
                    }
                }
                else
                    *buff++ = c;
            }
#endif
            else
            {
                if (c == '\n')
                    currentLine++;
                *buff++ = c;
            }
            c=in.get();
        }
        *buff++ = '\0';
        return TokString;
    }
    else
    {
        havePeek = true;
        while (!in.eof() &&
               c!=' ' &&
               c!='\n' &&
               c!='\t' &&
               c!='=' &&
               c!='>' &&
               c!='<')
        {
            *buff++ = c;
            c=in.get();
        }
        *buff++ = '\0';

        return TokLiteral;
    }
}

int XMLScanner::GetCurrentLine()
{
    return currentLine++;
}

XMLParser::XMLParser()
{
    scanner = NULL;
}

void XMLParser::GetNextToken()
{
    char *newText = acceptedText;
    acceptedText = currentText;
    token = scanner->GetNextToken(newText);
    currentText = newText;
}

bool XMLParser::Accept(XMLToken t)
{
    if (t == token)
    {
        GetNextToken();
        //cerr << "Accepted token type="<<XMLTokenTypeToString(t)
        //     << ", text=\""<<acceptedText<<"\"\n";
        return true;
    }
    else
    {
        return false;
    }
}

void XMLParser::Expect(XMLToken t)
{
    if (Accept(t))
    {
        // success
    }
    else
    {
        // we didn't match the token
        throw Exception(
                "Expected token of type %s at line %d, got %s instead (text=%s)",
                XMLTokenTypeToString(t).c_str(),
                scanner->GetCurrentLine(),
                XMLTokenTypeToString(token).c_str(),
                acceptedText);
    }
}

bool XMLParser::ParseXMLNestingAfterOpenBracket()
{
    // comments start with "!"; handle them here
    if (Accept(TokBang))
    {
        while (!Accept(TokClose))
        {
            handleComment(currentText);
            GetNextToken();
        }
        return false;
    }

    // the XML identifier <?xml blah blah > we can support as well
    if (Accept(TokQuestion))
    {
        while (!Accept(TokClose))
        {
            // Just ignore it....
            GetNextToken();
        }
        return false;
    }

    // Not a comment; we expect a literal for the element name
    Expect(TokLiteral);
    string elementName = acceptedText;

    // Read the attributes, if there are any
    XMLAttributes attributes;
    while (Accept(TokLiteral))
    {
        XMLAttribute a;
        a.type = acceptedText;
        Expect(TokEqual);
        Expect(TokString);
        a.value = acceptedText;
        attributes.push_back(a);
    }
    if (Accept(TokSlash))
    {
        // if we see a slash, then we're getting a begin/end in a single step
        Expect(TokClose);
        beginElement(elementName, attributes);
        endElement(elementName);
    }
    else
    {
        Expect(TokClose);

        // Okay, we got a whole element intro; let derived classes handle it
        beginElement(elementName, attributes);

        // If we get an open token, handle another nest; otherwise, it's just text
        while (true)
        {
            if (Accept(TokOpen))
            {
                if (Accept(TokSlash))
                    break;
                ParseXMLNestingAfterOpenBracket();
            }
            else
            {
                handleText(currentText);
                GetNextToken();
            }
        }

        // Can only get here once we get to a open-bracket and slash, so
        // it had better be the matching close tag
        Expect(TokLiteral);
        if (acceptedText != elementName and
            ErrorOnMismatchedTags)
        {
            throw Exception("Mismatched open/close tags at line %d: "
                            "expected '%s' but got '%s'",
                            scanner->GetCurrentLine(), elementName.c_str(),
                            acceptedText);
        }
        Expect(TokClose);
        endElement(elementName);
    }

    return true;
}

void XMLParser::Initialize(istream &input)
{
    if (scanner)
        delete scanner;

    // init
    savedInput = &input;
    token = TokNone;
    buff1[0] = 0;
    buff2[0] = 0;
    currentText  = buff1;
    acceptedText = buff2;
    scanner = new XMLScanner(input);
    GetNextToken();
}

void XMLParser::ParseSingleEntity()
{
    while (!Accept(TokEOF))
    {
        Expect(TokOpen);
        bool got_entity = ParseXMLNestingAfterOpenBracket();
        if (got_entity)
            break;
    }
}

void XMLParser::ParseAllEntities()
{
    while (!Accept(TokEOF))
    {
        Expect(TokOpen);
        ParseXMLNestingAfterOpenBracket();
    }
}

XMLParser::~XMLParser()
{
    // clean up
    if (scanner)
        delete scanner;
}


XMLWriter::XMLWriter()
{
    out = NULL;
    stillBeginningElement = false;
}

void XMLWriter::Indent()
{
    for (unsigned int i=0; i<elementStack.size(); i++)
        (*out) << "   ";
}

void XMLWriter::Open(ostream &output)
{
    out = &output;
    stillBeginningElement = false;
}

void XMLWriter::BeginElement(const std::string &name)
{
    if (stillBeginningElement)
        (*out) << ">" << endl;
    Indent();
    (*out) << "<"<< name;
    elementStack.push_back(name);
    stillBeginningElement = true;
}

void XMLWriter::AddAttribute(const XMLAttribute &att)
{
    if (!stillBeginningElement)
        throw Exception("Can't add an attribute anymore");
    (*out) << " "<<att.type<<"=\""<<att.value<<"\"";
}

void XMLWriter::AddData(const std::string &text)
{
    if (stillBeginningElement)
        (*out) << ">" << endl;
    stillBeginningElement = false;
    Indent();

    int n = text.length();
    (*out) << "\"";
    for (int i=0; i<n; i++)
    {
        if (text[i] == '\"')
            (*out) << "\\\"";
        else if (text[i] == '\\')
            (*out) << "\\\\";
        else
            (*out) << text[i];
    }
    (*out) << "\"" << endl;
}

void XMLWriter::BeginAddData()
{
    if (stillBeginningElement)
        (*out) << ">" << endl;
    stillBeginningElement = false;
    Indent();
}

void XMLWriter::EndAddData()
{
    (*out) << endl;
}


void XMLWriter::AddData(bool val)
{
    BeginAddData();
    (*out) << (val?"true":"false");
    EndAddData();
}

void XMLWriter::AddData(int val)
{
    BeginAddData();
    (*out) << val;
    EndAddData();
}

void XMLWriter::AddData(long long val)
{
    BeginAddData();
    (*out) << val;
    EndAddData();
}

void XMLWriter::AddData(float val)
{
    BeginAddData();
    char text[256];
    sprintf(text, "%.7g", val);
    (*out) << text;
    EndAddData();
}

void XMLWriter::AddData(double val)
{
    BeginAddData();
    char text[256];
    sprintf(text, "%.15g", val);
    (*out) << text;
    EndAddData();
}

void XMLWriter::EndElement(bool allowSingleLine)
{
    if (elementStack.size() < 1)
        throw Exception("No element to close");

    string name = elementStack.back();
    elementStack.pop_back();

    if (stillBeginningElement)
    {
        if (allowSingleLine)
            (*out) << ">";
        else
            (*out) << ">" << endl;
    }
    if (!allowSingleLine || !stillBeginningElement)
    {
        Indent();
    }
    stillBeginningElement = false;
    (*out) << "</"<<name << ">" << endl;
}

void XMLWriter::Close()
{
    if (elementStack.size() > 0)
        throw Exception("Closed file while elements were still open");
}


