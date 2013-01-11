// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "Attribute.h"
#include "XMLTools.h"
#include <cassert>
#include <cstring>

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// BasicTypes.cpp
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct Creators
{
    string           specificTypeName;
    AttCreatorFn     attCreator;
    PrimCreatorFn    primCreator;
    PrimArrCreatorFn primArrCreator;
    PrimVecCreatorFn primVecCreator;
    Creators(const string &stn = "")
    {
        specificTypeName = stn;
        attCreator = NULL;
        primCreator = NULL;
        primArrCreator = NULL;
        primVecCreator = NULL;
    }
};

vector<Creators> allCreators(1, Creators());
map<string,SpecificType>       mapSpecificTypeNameToSpecificType;
map<string,BasicType>          mapSpecificTypeNameToBasicType;


map<string,BasicType> stringToTypeMap;
map<BasicType,const char *> typeToStringMap;

BasicType   StringToBasicType(const string &type)
{
    if (type=="Unset")              return TypeUnset;
    if (type=="DynamicPtr")         return TypeDynamicPtr;
    if (type=="DynamicPtrArray")    return TypeDynamicPtrArray;
    if (type=="DynamicPtrVector")   return TypeDynamicPtrVector;
    if (type=="AttributeObj")       return TypeAttributeObj;
    if (type=="AttributeObjArray")  return TypeAttributeObjArray;
    if (type=="AttributeObjVector") return TypeAttributeObjVector;
    if (type=="AttributePtr")       return TypeAttributePtr;
    if (type=="AttributePtrArray")  return TypeAttributePtrArray;
    if (type=="AttributePtrVector") return TypeAttributePtrVector;
    if (type=="Bool")               return TypeBool;
    if (type=="BoolArray")          return TypeBoolArray;
    if (type=="BoolVector")         return TypeBoolVector;
    if (type=="Byte")               return TypeByte;
    if (type=="ByteArray")          return TypeByteArray;
    if (type=="ByteVector")         return TypeByteVector;
    if (type=="Int32")              return TypeInt32;
    if (type=="Int32Array")         return TypeInt32Array;
    if (type=="Int32Vector")        return TypeInt32Vector;
    if (type=="Int64")              return TypeInt64;
    if (type=="Int64Array")         return TypeInt64Array;
    if (type=="Int64Vector")        return TypeInt64Vector;
    if (type=="Float")              return TypeFloat;
    if (type=="FloatArray")         return TypeFloatArray;
    if (type=="FloatVector")        return TypeFloatVector;
    if (type=="Double")             return TypeDouble;
    if (type=="DoubleArray")        return TypeDoubleArray;
    if (type=="DoubleVector")       return TypeDoubleVector;
    if (type=="String")             return TypeString;
    if (type=="StringArray")        return TypeStringArray;
    if (type=="StringVector")       return TypeStringVector;
    if (type=="Primitive")          return TypePrimitive;
    if (type=="PrimitiveArray")     return TypePrimitiveArray;
    if (type=="PrimitiveVector")    return TypePrimitiveVector;
    if (mapSpecificTypeNameToBasicType.count(type) != 0)
        return mapSpecificTypeNameToBasicType[type];
    return TypeUnknown;
}

SpecificType StringToSpecificType(const string &type)
{
    SpecificType st = mapSpecificTypeNameToSpecificType[type];
    return st;
}

const char *TypeToString(BasicType type, SpecificType st)
{
    switch (type)
    {
      case TypeUnset:              return "Unset";
      case TypeDynamicPtr:         return "DynamicPtr";
      case TypeDynamicPtrArray:    return "DynamicPtrArray";
      case TypeDynamicPtrVector:   return "DynamicPtrVector";
      case TypeAttributeObj:       return "AttributeObj";
      case TypeAttributeObjArray:  return "AttributeObjArray";
      case TypeAttributeObjVector: return "AttributeObjVector";
      case TypeAttributePtr:       return "AttributePtr";
      case TypeAttributePtrArray:  return "AttributePtrArray";
      case TypeAttributePtrVector: return "AttributePtrVector";
      case TypeBool:               return "Bool";
      case TypeBoolArray:          return "BoolArray";
      case TypeBoolVector:         return "BoolVector";
      case TypeByte:               return "Byte";
      case TypeByteArray:          return "ByteArray";
      case TypeByteVector:         return "ByteVector";
      case TypeInt32:              return "Int32";
      case TypeInt32Array:         return "Int32Array";
      case TypeInt32Vector:        return "Int32Vector";
      case TypeInt64:              return "Int64";
      case TypeInt64Array:         return "Int64Array";
      case TypeInt64Vector:        return "Int64Vector";
      case TypeFloat:              return "Float";
      case TypeFloatArray:         return "FloatArray";
      case TypeFloatVector:        return "FloatVector";
      case TypeDouble:             return "Double";
      case TypeDoubleArray:        return "DoubleArray";
      case TypeDoubleVector:       return "DoubleVector";
      case TypeString:             return "String";
      case TypeStringArray:        return "StringArray";
      case TypeStringVector:       return "StringVector";
      case TypeUnknown:            return "Unknown";

        // TODO: inefficient if we want const char * for primitives
      case TypePrimitive:
        {
            const string &objname = allCreators[st].specificTypeName;
            string newname = objname;
            return strdup(newname.c_str());
        }
        break;

      case TypePrimitiveArray:
        {
            const string &objname = allCreators[st].specificTypeName;
            string newname = objname + "Array";
            return strdup(newname.c_str());
        }
        break;

      case TypePrimitiveVector:
        {
            const string &objname = allCreators[st].specificTypeName;
            string newname = objname + "Vector";
            return strdup(newname.c_str());
        }
        break;
    }
    throw Exception("Logic Error");
}

const char *TypeToBaseString(BasicType type, SpecificType st)
{
    switch (type)
    {
      case TypeUnset:
        return "Unset";
      case TypeDynamicPtr:
      case TypeDynamicPtrArray:
      case TypeDynamicPtrVector:
      case TypeAttributeObj:
      case TypeAttributeObjArray:
      case TypeAttributeObjVector:
      case TypeAttributePtr:
      case TypeAttributePtrArray:
      case TypeAttributePtrVector:
        return "Attribute";
      case TypeBool:
      case TypeBoolArray:
      case TypeBoolVector:
        return "Bool";
      case TypeByte:
      case TypeByteArray:
      case TypeByteVector:
        return "Byte";
      case TypeInt32:
      case TypeInt32Array:
      case TypeInt32Vector:
        return "Int32";
      case TypeInt64:
      case TypeInt64Array:
      case TypeInt64Vector:
        return "Int64";
      case TypeFloat:
      case TypeFloatArray:
      case TypeFloatVector:
        return "Float";
      case TypeDouble:
      case TypeDoubleArray:
      case TypeDoubleVector:
        return "Double";
      case TypeString:
      case TypeStringArray:
      case TypeStringVector:
        return "String";
      case TypeUnknown:
        return "Unknown";

        // TODO: inefficient if we want const char * for primitives
      case TypePrimitive:
      case TypePrimitiveArray:
      case TypePrimitiveVector:
        return allCreators[st].specificTypeName.c_str();
    }

    throw Exception("Logic Error");
}

bool IsPrimitive(BasicType t)
{
    switch (t)
    {
      case TypeBool:
      case TypeByte:
      case TypeInt32:
      case TypeInt64:
      case TypeFloat:
      case TypeDouble:
      case TypeString:
      case TypeBoolVector:
      case TypeByteVector:
      case TypeInt32Vector:
      case TypeInt64Vector:
      case TypeFloatVector:
      case TypeDoubleVector:
      case TypeStringVector:
      case TypeBoolArray:
      case TypeByteArray:
      case TypeInt32Array:
      case TypeInt64Array:
      case TypeFloatArray:
      case TypeDoubleArray:
      case TypeStringArray:
        return true;

      case TypePrimitive:
      case TypePrimitiveArray:
      case TypePrimitiveVector:
        return true;

      case TypeAttributeObj:
      case TypeAttributeObjArray:
      case TypeAttributeObjVector:
      case TypeAttributePtr:
      case TypeAttributePtrArray:
      case TypeAttributePtrVector:
      case TypeDynamicPtr:
      case TypeDynamicPtrArray:
      case TypeDynamicPtrVector:
        return false;

      case TypeUnset:
      case TypeUnknown:
        throw Exception("Asked if an unsupported type was primitve.");
    }
    throw Exception("Logic Error");
}

BasicTypeCategory TypeCategory(BasicType t)
{
    switch (t)
    {
      case TypeBool:
      case TypeBoolVector:
      case TypeBoolArray:
        return CategoryBoolean;

      case TypeByte:
      case TypeInt32:
      case TypeInt64:
      case TypeByteVector:
      case TypeInt32Vector:
      case TypeInt64Vector:
      case TypeByteArray:
      case TypeInt32Array:
      case TypeInt64Array:
        return CategoryIntegral;

      case TypeFloat:
      case TypeDouble:
      case TypeFloatVector:
      case TypeDoubleVector:
      case TypeFloatArray:
      case TypeDoubleArray:
        return CategoryReal;

      case TypeString:
      case TypeStringVector:
      case TypeStringArray:
        return CategoryString;

      case TypePrimitive:
      case TypePrimitiveArray:
      case TypePrimitiveVector:
        return CategoryPrimitive;

      case TypeAttributeObj:
      case TypeAttributeObjArray:
      case TypeAttributeObjVector:
      case TypeAttributePtr:
      case TypeAttributePtrArray:
      case TypeAttributePtrVector:
      case TypeDynamicPtr:
      case TypeDynamicPtrArray:
      case TypeDynamicPtrVector:
        return CategoryAttribute;

      case TypeUnset:
      case TypeUnknown:
        throw Exception("Asked if an unsupported type was primitve.");
    }
    throw Exception("Logic Error");
}

bool IsNonPrimitive(BasicType t)
{
    if (t==TypeUnset || t==TypeUnknown)
        return false;

    return !IsPrimitive(t);
}

bool IsUnsupported(BasicType t)
{
    return (t==TypeUnknown || t==TypeUnset);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
SpecificType RegisterPrimitiveCreators(const std::string name,
                                       PrimCreatorFn pc,
                                       PrimArrCreatorFn pac,
                                       PrimVecCreatorFn pvc)
{
    Creators c(name);
    c.primCreator = pc;
    c.primArrCreator = pac;
    c.primVecCreator = pvc;

    SpecificType st = allCreators.size();
    allCreators.push_back(c);

    string shortObj = name;
    string shortArr = name + "Array";
    string shortVec = name + "Vector";

    mapSpecificTypeNameToSpecificType[shortObj] = st;
    mapSpecificTypeNameToSpecificType[shortArr] = st;
    mapSpecificTypeNameToSpecificType[shortVec] = st;

    mapSpecificTypeNameToBasicType[shortObj] = TypePrimitive;
    mapSpecificTypeNameToBasicType[shortArr] = TypePrimitiveArray;
    mapSpecificTypeNameToBasicType[shortVec] = TypePrimitiveVector;

    return st;
}

SpecificType RegisterAttributeCreator(const std::string &name,
                                      AttCreatorFn ac)
{
    Creators c(name);
    c.attCreator = ac;

    SpecificType st = allCreators.size();
    allCreators.push_back(c);

    mapSpecificTypeNameToSpecificType[name] = st;

    return st;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
struct AttributeIndex
{
    unsigned int         nfields;
    map<string, int>     fieldmap;

    vector<BasicType>    types;
    vector<SpecificType> subtypes;
    vector<int>          lengths;
    vector<string>       names;

    AttributeIndex();
    void FreePointers(const vector<void*> &pointers,
                      const vector<bool> &pointerOwned);
    void Copy(vector<void*> &lhs, const vector<void*> &rhs);
    void AddField(const string &n, int l, BasicType t, SpecificType st=0);
    void AddGenericField(const string &name, int len, BasicType type, SpecificType st=0);
};


void Attribute::AddField(const string &n, void *v, int l,
                                BasicType t, SpecificType st)
{
    if (populatingClassIndex)
        populatingClassIndex->AddField(n,l,t,st);

    pointers.push_back(v);
    pointerOwned.push_back(false);
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class QuotedString
{
  private:
    string s;
  public:
             QuotedString()                           { }
             QuotedString(const string &rhs) : s(rhs) { }
    void     operator=(const string &rhs)             { s = rhs; }
    operator string()                                 { return s; }

    friend ostream &operator<<(ostream &out, const QuotedString &s);
    friend istream &operator>>(istream &in, QuotedString &s);
};

ostream &operator<<(ostream &out, const QuotedString &s)
{
    int n = s.s.length();
    out << "\"";
    for (int i=0; i<n; i++)
    {
        if (s.s[i] == '\"')
            out << "\\\"";
        else if (s.s[i] == '\\')
            out << "\\\\";
        else
            out << s.s[i];
    }
    out << "\"";
    return out;
}

istream &operator>>(istream &in, QuotedString &s)
{
    s.s.clear();
    char c = '\0';
    while (in && c != '\"')
    {
        in.get(c);
    }
    if (!in || c != '\"')
        throw Exception("Didn't find leading quote for string");
    while (in)
    {
        in.get(c);
        if (c == '\"')
            break;
        if (c == '\\')
            in.get(c);
        s.s += c;
    }
    if (!in)
        throw Exception("Didn't find ending quote for string");
    return in;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Primitive.cpp
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
map<PrimCreatorFn,SpecificType> Primitive::mapPrimCreatorToSpecificType;

PrimitiveVectorBase::PrimitiveVectorBase()
{
}

PrimitiveVectorBase::~PrimitiveVectorBase()
{
}


PrimitiveArrayBase::PrimitiveArrayBase()
{
}



PrimitiveArrayBase::~PrimitiveArrayBase()
{
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// XMLSerializer.h
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class XMLSerializer : public XMLWriter
{
  public:
    XMLSerializer();
    ~XMLSerializer();

    void Write(Attribute *att);
   
  private:
    void WriteContents(Attribute *att);
};


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// XMLSerializer.cpp
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
template <class T, class ST>
static void XMLSerializePrimitive(XMLWriter &writer,
                      void *pointer)
{
    T *ptr = (T*)(pointer);

    ST tmp = *ptr;
    writer.AddData(tmp);
}

template <class T, class ST>
static void XMLSerializePrimitiveArray(XMLWriter &writer,
                              void *pointer,
                              int length)
{
    T *ptr = (T*)(pointer);

    ST tmp;
    for (int j=0; j<length; j++)
    {
        tmp = ptr[j];
        writer.AddData(tmp);
    }
}


template <class T, class ST>
static void XMLSerializePrimitiveVector(XMLWriter &writer,
                               void *pointer,
                               int length)
{
    vector<T> *ptr = (vector<T>*)(pointer);

    ST tmp;
    for (int j=0; j<length; j++)
    {
        tmp = ptr->operator[](j);
        writer.AddData(tmp);
    }
}

static void XMLSerializeNULLObject(XMLWriter &writer)
{
    writer.BeginElement("Attribute");
    writer.AddAttribute(XMLAttribute("type","NULL"));
    writer.EndElement(true);
}

// ----------------------------------------------------------------------------

XMLSerializer::XMLSerializer() : XMLWriter()
{
}

XMLSerializer::~XMLSerializer()
{
}

void XMLSerializer::Write(Attribute *att)
{
    BeginElement("Attribute");
    AddAttribute(XMLAttribute("type",att->GetType()));
    WriteContents(att);
    EndElement();
}

void XMLSerializer::WriteContents(Attribute *att)
{
    //att->classIndex->XMLSerialize(this, att->pointers);

    const vector<void*> &pointers = att->pointers;
    AttributeIndex *ci = att->classIndex;
    unsigned int nfields = ci->nfields;
    const vector<BasicType> &types   = ci->types;
    const vector<SpecificType> &subtypes   = ci->subtypes;
    const vector<int>       &lengths = ci->lengths;
    const vector<string>    &names   = ci->names;

    for (unsigned int i=0; i<nfields; i++)
    {
        int length = att->GetFieldLength(i);

        if (length == -1)
        {
            throw Exception("ClassIndex::Serialize: found length "
                            "of -1 for item '%s::%s'; probably a "
                            "vector that didn't get caught.",
                            att->GetType(), names[i].c_str());
        }

        BeginElement("Field");
        AddAttribute(XMLAttribute("type",TypeToString(types[i],subtypes[i])));
        AddAttribute(XMLAttribute("name",names[i]));
        AddAttribute(XMLAttribute("length",length));

        switch (types[i])
        {
          case TypeBool:         XMLSerializePrimitive<bool,int>(*this,pointers[i]);                           break;
          case TypeBoolArray:    XMLSerializePrimitiveArray<bool,int>(*this,pointers[i],length);             break;
          case TypeBoolVector:   XMLSerializePrimitiveVector<bool,int>(*this,pointers[i],length);            break;
          case TypeByte:         XMLSerializePrimitive<byte,int>(*this,pointers[i]);                         break;
          case TypeByteArray:    XMLSerializePrimitiveArray<byte,int>(*this,pointers[i],length);             break;
          case TypeByteVector:   XMLSerializePrimitiveVector<byte,int>(*this,pointers[i],length);            break;
          case TypeInt32:        XMLSerializePrimitive<int32,int32>(*this,pointers[i]);                      break;
          case TypeInt32Array:   XMLSerializePrimitiveArray<int32,int32>(*this,pointers[i],length);          break;
          case TypeInt32Vector:  XMLSerializePrimitiveVector<int32,int32>(*this,pointers[i],length);         break;
          case TypeInt64:        XMLSerializePrimitive<int64,int64>(*this,pointers[i]);                      break;
          case TypeInt64Array:   XMLSerializePrimitiveArray<int64,int64>(*this,pointers[i],length);          break;
          case TypeInt64Vector:  XMLSerializePrimitiveVector<int64,int64>(*this,pointers[i],length);         break;
          case TypeFloat:        XMLSerializePrimitive<float,float>(*this,pointers[i]);                      break;
          case TypeFloatArray:   XMLSerializePrimitiveArray<float,float>(*this,pointers[i],length);          break;
          case TypeFloatVector:  XMLSerializePrimitiveVector<float,float>(*this,pointers[i],length);         break;
          case TypeDouble:       XMLSerializePrimitive<double,double>(*this,pointers[i]);                    break;
          case TypeDoubleArray:  XMLSerializePrimitiveArray<double,double>(*this,pointers[i],length);        break;
          case TypeDoubleVector: XMLSerializePrimitiveVector<double,double>(*this,pointers[i],length);       break;
          case TypeString:       XMLSerializePrimitive<string,QuotedString>(*this,pointers[i]);              break;
          case TypeStringArray:  XMLSerializePrimitiveArray<string,QuotedString>(*this,pointers[i],length);  break;
          case TypeStringVector: XMLSerializePrimitiveVector<string,QuotedString>(*this,pointers[i],length); break;

          // attr obj/attr ptr/dynamic ptr
          case TypeAttributeObj:
            {
                ((Attribute*)pointers[i])->XMLSerialize(this);
            }
            break;

          case TypeAttributePtr:
          case TypeDynamicPtr:
            {
                if ((*((Attribute**)(pointers[i]))))
                    (*((Attribute**)(pointers[i])))->XMLSerialize(this);
                else
                    XMLSerializeNULLObject(*this);
            }
            break;

          case TypeAttributeObjArray:
          case TypeAttributePtrArray:
          case TypeDynamicPtrArray:
              {
                AttributeArrayBase *a = 
                    (AttributeArrayBase*)(pointers[i]);
                for (int j=0; j<lengths[i]; j++)
                {
                    if (a->GetAttributeAtIndex(j))
                        a->GetAttributeAtIndex(j)->XMLSerialize(this);
                    else
                        XMLSerializeNULLObject(*this);
                }
              }
            break;

          case TypeAttributeObjVector:
          case TypeAttributePtrVector:
          case TypeDynamicPtrVector:
            {
                AttributeVectorBase *v = 
                    (AttributeVectorBase*)(pointers[i]);
                for (int j=0; j<v->GetLength(); j++)
                {
                    if (v->GetAttributeAtIndex(j))
                        v->GetAttributeAtIndex(j)->XMLSerialize(this);
                    else
                        XMLSerializeNULLObject(*this);
                }
            }
            break;

          case TypeUnknown:
            throw Exception("unknown field type case in ClassIndex::XMLSerialize()");

          case TypeUnset:
            throw Exception("unset field type in ClassIndex::XMLSerialize()");

          case TypePrimitive:
            {
                Primitive *p = (Primitive*)pointers[i];
                int nf = p->NumFields();
                for (int k=0; k<nf; k++)
                {
                    BeginAddData();
                    p->XMLSerialize(*out, k);
                    EndAddData();
                }
            }
            break;

          case TypePrimitiveArray:
            {
                PrimitiveArrayBase *a =  (PrimitiveArrayBase*)(pointers[i]);
                for (int j=0; j<lengths[i]; j++)
                {
                    Primitive *p = a->GetPrimitiveAtIndex(j);
                    int nf = p->NumFields();
                    if (p)
                    {
                        for (int k=0; k<nf; k++)
                        {
                            BeginAddData();
                            p->XMLSerialize(*out, k);
                            EndAddData();
                        }
                    }
                    else
                        XMLSerializeNULLObject(*this);
                }
            }
            break;

          case TypePrimitiveVector:
            {
                PrimitiveVectorBase *v = 
                    (PrimitiveVectorBase*)(pointers[i]);
                for (int j=0; j<v->GetLength(); j++)
                {
                    Primitive *p = v->GetPrimitiveAtIndex(j);
                    int nf = p->NumFields();
                    if (p)
                    {
                        for (int k=0; k<nf; k++)
                        {
                            BeginAddData();
                            p->XMLSerialize(*out, k);
                            EndAddData();
                        }
                    }
                    else
                        XMLSerializeNULLObject(*this);
                }
            }
            break;
        }
        EndElement();
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// XMLUnserializer.h
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class XMLParseStackElement;
class XMLUnserializer : private XMLParser
{
  public:
                 XMLUnserializer();
                 XMLUnserializer(istream &input);
                 XMLUnserializer(const string &s);
    virtual      ~XMLUnserializer();

    void         Initialize(istream &input);
  protected:
    friend class Attribute;
    friend class GenericAttribute;
    void         ParseErroringIfWrongType(Attribute *att);
    string       ParseSkippingIfWrongType(Attribute *att);
    void         ParseSkippingEverything();
    Attribute   *ParseCreatingNeededType();
    string       ParseCreatingNeededFields(Attribute *att);

  private:
    void         beginAttribute(const string &element,
                                const XMLAttributes &atts);
    void         beginField(const string &element,
                            const XMLAttributes &atts);

  protected:
    virtual void beginElement(const string &element,
                              const XMLAttributes &atts);
    virtual void handleText(const string &text);
    virtual void endElement(const string &element);

 protected:
    vector<XMLParseStackElement*> stack;
    istream *tmp_istream;
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// XMLUnserializer.cpp
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
enum ParseMode
{
    FailIfWrongType,
    SkipIfWrongType,
    SkipEverything,
    CreateType,
    CreateFields,         // for GenericAttributes
    CreateFieldsAndIndex  // for GenericAttributes, first encounter
};

struct XMLParseStackElement
{
    struct StructureData
    {
        bool       initializedAttribute;
        Attribute *attribute;
        string     parsedType;
        ParseMode  parseMode;
    };

    struct FieldData
    {
        BasicType    type;
        SpecificType subtype;
        string       name;
        int          length;
        int          index;
        bool         skip;
        int          position;
    };

    StructureData sd;
    FieldData     fd;
};

template <class T>
void UnserializeSingleItem(const std::string &s, T &item);

inline void UnserializeSingleItem(const std::string &s, bool &item)
{
    item = (s=="true" ? true : false);
}
inline void UnserializeSingleItem(const std::string &s, byte &item)
{
    item = strtol(s.c_str(),NULL,0);
}
inline void UnserializeSingleItem(const std::string &s, int32 &item)
{
    item = strtol(s.c_str(),NULL,0);
}
inline void UnserializeSingleItem(const std::string &s, int64 &item)
{
    item = strtoll(s.c_str(),NULL,0);
}
inline void UnserializeSingleItem(const std::string &s, float &item)
{
    item = strtof(s.c_str(),NULL);
}
inline void UnserializeSingleItem(const std::string &s, double &item)
{
    item = strtod(s.c_str(),NULL);
}
inline void UnserializeSingleItem(const std::string &s, string &item)
{
    item = s;
}


template <class T, class ST>
static void XMLUnserializeItem(const string &in,
                               vector<void*> &pointers,
                               int index,
                               int position,
                               int)
{
    T *ptr = (T*)(pointers[index]);
    if (!ptr && position==0)
    {
        ptr = new T;
        pointers[index] = ptr;
    }
    ST tmp;
    UnserializeSingleItem(in,tmp);
    *ptr = tmp;
}

template <class T, class ST>
static void XMLUnserializeArrayItem(const string &in,
                                    vector<void*> &pointers,
                                    int index,
                                    int position,
                                    int length)
{
    T *ptr = (T*)(pointers[index]);
    if (!ptr && position==0)
    {
        ptr = new T[length];
        pointers[index] = ptr;
    }
    ST tmp;
    UnserializeSingleItem(in,tmp);
    ptr[position] = tmp;
}


template <class T, class ST>
static void XMLUnserializeVectorItem(const string &in,
                                     vector<void*> &pointers,
                                     int index,
                                     int position,
                                     int length)
{
    vector<T> *ptr = (vector<T>*)(pointers[index]);
    if (!ptr && position==0)
    {
        ptr = new vector<T>(length);
        pointers[index] = ptr;
    }
    ST tmp;
    UnserializeSingleItem(in,tmp);
    ptr->operator[](position) = tmp;
}


static void XMLUnserializeUserPrimitiveFieldItem(SpecificType st,
                                                 const string &in,
                                                 vector<void*> &pointers,
                                                 int index,
                                                 int position,
                                                 int length)
{
    Primitive *ptr = (Primitive*)(pointers[index]);
    if (!ptr && position==0)
    {
        ptr = allCreators[st].primCreator();
        pointers[index] = ptr;
    }
    int element = position / ptr->NumFields();
    int field = position % ptr->NumFields();
    if (element >= length)
    {
        // Too much data for this item!  It's okay, to be
        // robust we just ignore it.  Warning is most appropriate.
        return;
    }
    ptr->XMLUnserialize(in, field);
}

static void XMLUnserializeUserPrimitiveArrayFieldItem(SpecificType st,
                                                      const string &in,
                                                      vector<void*> &pointers,
                                                      int index,
                                                      int position,
                                                      int length)
{
    PrimitiveArrayBase *a = (PrimitiveArrayBase*)(pointers[index]);
    if (!a && position==0)
    {
        a = allCreators[st].primArrCreator(length);
        pointers[index] = a;
    }
    int element = position / a->GetPrimitiveAtIndex(0)->NumFields();
    int field = position % a->GetPrimitiveAtIndex(0)->NumFields();
    if (element >= length)
    {
        // Too much data for this item!  It's okay, to be
        // robust we just ignore it.  Warning is most appropriate.
        return;
    }
    a->GetPrimitiveAtIndex(element)->XMLUnserialize(in, field);
}

static void XMLUnserializeUserPrimitiveVectorFieldItem(SpecificType st,
                                                       const string &in,
                                                       vector<void*> &pointers,
                                                       int index,
                                                       int position,
                                                       int length)
{
    PrimitiveVectorBase *v = (PrimitiveVectorBase*)(pointers[index]);
    if (!v && position==0)
    {
        v = allCreators[st].primVecCreator(length);
        pointers[index] = v;
    }
    int element = position / v->GetPrimitiveAtIndex(0)->NumFields();
    int field = position % v->GetPrimitiveAtIndex(0)->NumFields();
    if (element >= length)
    {
        // Too much data for this item!  It's okay, to be
        // robust we just ignore it.  Warning is most appropriate.
        return;
    }
    v->GetPrimitiveAtIndex(element)->XMLUnserialize(in, field);
}

void XMLUnserializePrimitiveFieldItem(BasicType t,
                                      SpecificType st,
                                      const string &in,
                                      vector<void*> &pointers,
                                      int length,
                                      int index,
                                      int position)
{
    switch (t)
    {
      case TypeBool:         XMLUnserializeItem<bool,int>(in,pointers,index,position,length);              break;
      case TypeBoolArray:    XMLUnserializeArrayItem<bool,int>(in,pointers,index,position,length);       break;
      case TypeBoolVector:   XMLUnserializeVectorItem<bool,int>(in,pointers,index,position,length);      break;
      case TypeByte:         XMLUnserializeItem<byte,int>(in,pointers,index,position,length);              break;
      case TypeByteArray:    XMLUnserializeArrayItem<byte,int>(in,pointers,index,position,length);       break;
      case TypeByteVector:   XMLUnserializeVectorItem<byte,int>(in,pointers,index,position,length);      break;
      case TypeInt32:        XMLUnserializeItem<int32,int32>(in,pointers,index,position,length);           break;
      case TypeInt32Array:   XMLUnserializeArrayItem<int32,int32>(in,pointers,index,position,length);    break;
      case TypeInt32Vector:  XMLUnserializeVectorItem<int32,int32>(in,pointers,index,position,length);   break;
      case TypeInt64:        XMLUnserializeItem<int64,int64>(in,pointers,index,position,length);           break;
      case TypeInt64Array:   XMLUnserializeArrayItem<int64,int64>(in,pointers,index,position,length);    break;
      case TypeInt64Vector:  XMLUnserializeVectorItem<int64,int64>(in,pointers,index,position,length);   break;
      case TypeFloat:        XMLUnserializeItem<float,float>(in,pointers,index,position,length);           break;
      case TypeFloatArray:   XMLUnserializeArrayItem<float,float>(in,pointers,index,position,length);    break;
      case TypeFloatVector:  XMLUnserializeVectorItem<float,float>(in,pointers,index,position,length);   break;
      case TypeDouble:       XMLUnserializeItem<double,double>(in,pointers,index,position,length);         break;
      case TypeDoubleArray:  XMLUnserializeArrayItem<double,double>(in,pointers,index,position,length);  break;
      case TypeDoubleVector: XMLUnserializeVectorItem<double,double>(in,pointers,index,position,length); break;
      case TypeString:       XMLUnserializeItem<string,string>(in,pointers,index,position,length);         break;
      case TypeStringArray:  XMLUnserializeArrayItem<string,string>(in,pointers,index,position,length);  break;
      case TypeStringVector: XMLUnserializeVectorItem<string,string>(in,pointers,index,position,length); break;
      case TypePrimitive:
        XMLUnserializeUserPrimitiveFieldItem(st,
                                             in,
                                             pointers,
                                             index,
                                             position,
                                             length);
        break;

      case TypePrimitiveArray:
        XMLUnserializeUserPrimitiveArrayFieldItem(st,
                                                  in,
                                                  pointers,
                                                  index,
                                                  position,
                                                  length);
        break;

      case TypePrimitiveVector:
        XMLUnserializeUserPrimitiveVectorFieldItem(st,
                                                   in,
                                                   pointers,
                                                   index,
                                                   position,
                                                   length);
        break;

      default:
        throw Exception("Logic Error");
    }
}


XMLUnserializer::XMLUnserializer()
{
}

XMLUnserializer::XMLUnserializer(const std::string &s)
{
    tmp_istream = new istringstream(s);
    Initialize(*tmp_istream);
}

XMLUnserializer::XMLUnserializer(istream &is)
{
    tmp_istream = NULL;
    Initialize(is);
}

void XMLUnserializer::Initialize(istream &input)
{
    tmp_istream = NULL;
    XMLParser::Initialize(input);
}

XMLUnserializer::~XMLUnserializer()
{
    if (tmp_istream)
        delete tmp_istream;
}

void XMLUnserializer::ParseErroringIfWrongType(Attribute *att)
{
    XMLParseStackElement *el = new XMLParseStackElement;
    el->sd.attribute = att;
    el->sd.attribute->EnsureIndexCreated();
    el->sd.initializedAttribute = false;
    el->sd.parseMode = FailIfWrongType;
    stack.push_back(el);
    try {
        XMLParser::ParseSingleEntity();
    }
    catch (...)
    {
        stack.pop_back();
        delete el;
        throw;
    }
    stack.pop_back();
    delete el;
    return;
}

string XMLUnserializer::ParseSkippingIfWrongType(Attribute *att)
{
    XMLParseStackElement *el = new XMLParseStackElement;
    el->sd.attribute = att;
    el->sd.attribute->EnsureIndexCreated();
    el->sd.initializedAttribute = false;
    el->sd.parseMode = SkipIfWrongType;
    stack.push_back(el);
    try {
        XMLParser::ParseSingleEntity();
    }
    catch (...)
    {
        stack.pop_back();
        delete el;
        throw;
    }
    stack.pop_back();
    string parsedType = el->sd.parsedType;
    delete el;
    return parsedType;
}

void XMLUnserializer::ParseSkippingEverything()
{
    XMLParseStackElement *el = new XMLParseStackElement;
    el->sd.initializedAttribute = false;
    el->sd.parseMode = SkipEverything;
    stack.push_back(el);
    try {
        XMLParser::ParseSingleEntity();
    }
    catch (...)
    {
        stack.pop_back();
        delete el;
        throw;
    }
    stack.pop_back();
    delete el;
    return;
}

Attribute *XMLUnserializer::ParseCreatingNeededType()
{
    XMLParseStackElement *el = new XMLParseStackElement;
    el->sd.attribute = NULL;
    el->sd.initializedAttribute = false;
    el->sd.parseMode = CreateType;
    stack.push_back(el);
    try {
        XMLParser::ParseSingleEntity();
    }
    catch (...)
    {
        stack.pop_back();
        delete el;
        throw;
    }
    stack.pop_back();
    Attribute *attribute = el->sd.attribute;
    delete el;
    return attribute;
}

string XMLUnserializer::ParseCreatingNeededFields(Attribute *att)
{
    XMLParseStackElement *el = new XMLParseStackElement;
    el->sd.attribute = att;
    el->sd.initializedAttribute = false;
    el->sd.parseMode = CreateFields;
    el->fd.index = -1;
    stack.push_back(el);
    try {
        XMLParser::ParseSingleEntity();
    }
    catch (...)
    {
        stack.pop_back();
        delete el;
        throw;
    }
    stack.pop_back();
    string parsedType = el->sd.parsedType;
    delete el;
    return parsedType;
}

void XMLUnserializer::beginAttribute(const string &element,
                                     const XMLAttributes &atts)
{
    //cerr << "XMLUnserializer::beginAttribute "<<element<<endl;
    XMLParseStackElement *el = stack.back();
#define ASSERT(test) { if (!(test)) throw Exception("Assertion error (%s) %s:%d",#test,__FILE__,__LINE__); }
    ASSERT(element=="Attribute");
    ASSERT(atts.size() == 1);
    ASSERT(atts[0].type == "type");
    el->sd.parsedType = atts[0].value;
    el->sd.initializedAttribute = true;
    el->fd.skip = false;
    if (el->sd.parseMode == CreateType)
    {
        if (el->sd.parsedType != "NULL")
        {
            el->sd.attribute = Attribute::CreateAttribute(el->sd.parsedType);
            el->sd.attribute->EnsureIndexCreated();
        }
    }
    else if (el->sd.parseMode == FailIfWrongType)
    {
        bool matches = el->sd.parsedType == el->sd.attribute->GetType();
        if (!matches)
        {
            throw Exception("XMLUnserialize: given type '%s' "
                            "incompatible with current type '%s'",
                            el->sd.parsedType.c_str(),
                            el->sd.attribute->GetType());
        }
    }
    else if (el->sd.parseMode == SkipIfWrongType)
    {
        bool matches = el->sd.parsedType == el->sd.attribute->GetType();
        if (!matches)
        {
            //cerr << "Skipping "<<el->sd.attribute->GetType()<<" element because "
            //     << "nested type "<<el->sd.parsedType<<" didn't match expected type\n";
            el->sd.parseMode = SkipEverything;
        }
    }
    else if (el->sd.parseMode == CreateFields)
    {
        ((GenericAttribute*)(el->sd.attribute))->type = el->sd.parsedType;
        if (el->sd.attribute->classIndex ||
            el->sd.attribute->pointers.size()>0)
        {
            // we may want to allow this in the future, but I'm not going to
            // think about it right now
            throw Exception("XMLUnserialize: wasn't empty");
        }

        if (Attribute::allClassIndex.count(el->sd.parsedType))
        {
            //cerr << "Found existing class index for type '"<<el->sd.parsedType<<"'\n";
            el->sd.attribute->classIndex = Attribute::allClassIndex[el->sd.parsedType];
        }
        else
        {
            //cerr << "Creating new class index for type '"<<el->sd.parsedType<<"'\n";
            el->sd.parseMode = CreateFieldsAndIndex;
            el->sd.attribute->classIndex = new AttributeIndex();
            Attribute::allClassIndex[el->sd.parsedType] = el->sd.attribute->classIndex;
        }

    }
}


void XMLUnserializer::beginField(const string &element,
                                 const XMLAttributes &atts)
{
    XMLParseStackElement *el = stack.back();
    ASSERT(element=="Field");
    el->fd.type = StringToBasicType(atts.GetValue("type"));
    el->fd.subtype = StringToSpecificType(atts.GetValue("type"));
    //cerr << "XMLUnserializer::beginField("<<element<<"), mode="<<el->sd.parseMode<<" "<<element<<" type="<<el->fd.type<<endl;
    el->fd.name = atts.GetValue("name"); 
    el->fd.length = atoi(atts.GetValue("length").c_str());
    if (el->sd.parseMode == CreateFields)
        el->fd.index++;
    else
        el->fd.index = -1;
    el->fd.position = 0;
    el->fd.skip = false;
    if (el->sd.parseMode == SkipEverything)
    {
        el->fd.skip = true;
    }
    else if (el->sd.attribute->classIndex->fieldmap.count(el->fd.name) == 0)
    {
        if (el->sd.parseMode == CreateFieldsAndIndex)
        {
            el->fd.index = el->sd.attribute->classIndex->nfields;
            el->sd.attribute->classIndex->AddGenericField(el->fd.name, el->fd.length, el->fd.type, el->fd.subtype);
        }
        else if (el->sd.parseMode == CreateFields)
        {
            throw Exception("detected unknown field %s in "
                            "generic unserialization", el->fd.name.c_str());
        }
        else
        {
            //cerr << "Skipping "<<el->fd.name<<" because field didn't exist\n";
            //cerr << "Trying to parse into attribute of type "<<el->sd.attribute->GetType()<<endl;
            el->fd.skip = true;
        }
    }
    else
    {
        if (el->sd.parseMode == CreateFields &&
            el->fd.index != el->sd.attribute->classIndex->fieldmap[el->fd.name])
        {
            throw Exception("detected name/index mismatch in "
                            "generic unserialization (%s)",el->fd.name.c_str());
        }
        else
        {
            el->fd.index = el->sd.attribute->classIndex->fieldmap[el->fd.name];
        }
        if (el->fd.type != el->sd.attribute->classIndex->types[el->fd.index])
        {
            if (el->sd.parseMode == CreateFields)
            {
                throw Exception("detected name/type mismatch in "
                                "generic unserialization (%s)",el->fd.name.c_str());
            }
            else
            {
                //cerr << "Skipping "<<el->fd.name<<" because types didn't match\n";
                el->fd.skip = true;
            }
        }
        else if (el->sd.attribute->classIndex->lengths[el->fd.index] != -1 &&
                 el->sd.attribute->classIndex->lengths[el->fd.index] != el->fd.length)
        {
            if (el->sd.parseMode == CreateFields)
            {
                throw Exception("detected name/length mismatch in "
                                "generic unserialization (%s)",el->fd.name.c_str());
            }
            else
            {
                //cerr << "Skipping "<<el->fd.name<<" because lengths didn't match\n";
                el->fd.skip = true;
            }
        }
    }

    if (el->sd.parseMode == CreateFields ||
        el->sd.parseMode == CreateFieldsAndIndex)
    {
        el->sd.attribute->pointers.resize(el->sd.attribute->classIndex->nfields,NULL);
        el->sd.attribute->pointerOwned.resize(el->sd.attribute->classIndex->nfields,true);
    }

    // resize the vectors (but not if they're part of a generic
    // attribute; we'll create them as the right size later)
    if (el->fd.index >= 0 &&
        el->sd.parseMode != CreateFields &&
        el->sd.parseMode != CreateFieldsAndIndex)
    {
        //cerr << "skip="<<el->fd.skip<<": about to resize "<<el->fd.name<<" to length "<<el->fd.length<<endl;
        int newVectorSize = el->fd.skip ? 0 : el->fd.length;
        switch (el->fd.type)
        {
          case TypeBoolVector:
            ((vector<bool> *)el->sd.attribute->pointers[el->fd.index])->resize(newVectorSize);
            break;
          case TypeByteVector:
            ((vector<byte> *)el->sd.attribute->pointers[el->fd.index])->resize(newVectorSize);
            break;
          case TypeInt32Vector:
            ((vector<int32> *)el->sd.attribute->pointers[el->fd.index])->resize(newVectorSize);
            break;
          case TypeInt64Vector:
            ((vector<int64> *)el->sd.attribute->pointers[el->fd.index])->resize(newVectorSize);
            break;
          case TypeFloatVector:
            ((vector<float> *)el->sd.attribute->pointers[el->fd.index])->resize(newVectorSize);
            break;
          case TypeDoubleVector:
            ((vector<double> *)el->sd.attribute->pointers[el->fd.index])->resize(newVectorSize);
            break;
          case TypeStringVector:
            ((vector<string> *)el->sd.attribute->pointers[el->fd.index])->resize(newVectorSize);
            break;
          case TypeAttributeObjVector:
            ((AttributeVectorBase*)(el->sd.attribute->pointers[el->fd.index]))->SetLength(newVectorSize);
            break;
          case TypeAttributePtrVector:
            ((AttributeVectorBase*)(el->sd.attribute->pointers[el->fd.index]))->EraseAll();
            ((AttributeVectorBase*)(el->sd.attribute->pointers[el->fd.index]))->SetLength(newVectorSize);
            break;
          case TypeDynamicPtrVector:
            ((AttributeVectorBase*)(el->sd.attribute->pointers[el->fd.index]))->EraseAll();
            ((AttributeVectorBase*)(el->sd.attribute->pointers[el->fd.index]))->SetLength(newVectorSize);
            break;
          case TypePrimitiveVector:
            ((PrimitiveVectorBase*)(el->sd.attribute->pointers[el->fd.index]))->SetLength(newVectorSize);
            break;

          default:
            // Not a vector; nothing to resize
            break;
        }
    }

    if (IsPrimitive(el->fd.type))
    {
        // Skipping or not, these are handled by the handleText call
        return;
    }

    if (el->fd.skip)
    {
        // Can't ignore it ourselves
        for (int k=0; k<el->fd.length; k++)
        {
            ParseSkippingEverything();
        }
        return;
    }


    // handle the attribute objects
    if (el->sd.parseMode == CreateFields ||
        el->sd.parseMode == CreateFieldsAndIndex)
    {
        switch (el->fd.type)
        {
            // attr obj
          case TypeAttributeObj:
              {
                  GenericAttribute *g = new GenericAttribute;
                  el->sd.attribute->pointers[el->fd.index] = g;
                  ParseCreatingNeededFields(g);
              }
              break;

          case TypeAttributeObjArray:
              {
                  GenericAttribute *g = new GenericAttribute[el->fd.length];
                  AttributeArrayBase *a =
                      new AttributeObjectArray<GenericAttribute>(g);
                  el->sd.attribute->pointers[el->fd.index] = a;
                  for (int j=0; j<el->fd.length; j++)
                      ParseCreatingNeededFields(a->GetAttributeAtIndex(j));
              }
              break;

          case TypeAttributeObjVector:
              {
                  vector<GenericAttribute> *g = new vector<GenericAttribute>(el->fd.length);
                  AttributeVectorBase *v = 
                      new AttributeObjectVector<GenericAttribute>(*g);
                  el->sd.attribute->pointers[el->fd.index] = v;
                  for (int j=0; j<el->fd.length; j++)
                      ParseCreatingNeededFields(v->GetAttributeAtIndex(j));
              }
              break;

              // attr ptr
              // dynamic ptr
              //   NOTE: at the point where we're unserializing a generic
              //   attribute, there's little point in attempting to construct
              //   a non-generic version of the class.  It should probably
              //   be fine even if there is a non-generic version,
              //   but I suppose that should be tested just in case.
          case TypeAttributePtr:
          case TypeDynamicPtr:
              {
                  GenericAttribute **g = new GenericAttribute*;
                  *g = new GenericAttribute;
                  el->sd.attribute->pointers[el->fd.index] = g;
                  ParseCreatingNeededFields(*g);
              }
              break;

          case TypeAttributePtrArray:
          case TypeDynamicPtrArray:
              {
                  GenericAttribute **g = new GenericAttribute*[el->fd.length];
                  AttributeArrayBase *a =
                      new AttributePointerArray<GenericAttribute>(g);
                  el->sd.attribute->pointers[el->fd.index] = a;
                  for (int j=0; j<el->fd.length; j++)
                  {
                      GenericAttribute *att = new GenericAttribute;
                      a->SetAttributeAtIndex(j,att);
                      ParseCreatingNeededFields(att);
                  }
              }
              break;

          case TypeAttributePtrVector:
          case TypeDynamicPtrVector:
              {
                  vector<GenericAttribute*> *g = new vector<GenericAttribute*>(el->fd.length);
                  AttributeVectorBase *v = 
                      new AttributePointerVector<GenericAttribute*>(*g);
                  el->sd.attribute->pointers[el->fd.index] = v;
                  for (int j=0; j<el->fd.length; j++)
                  {
                      GenericAttribute *att = new GenericAttribute;
                      v->SetAttributeAtIndex(j,att);
                      ParseCreatingNeededFields(att);
                  }
              }
              break;

          default:
            throw Exception("Logic Error");
        }
    }
    else
    {
        switch (el->fd.type)
        {
          case TypeAttributeObj:
              {
                  ParseSkippingIfWrongType((Attribute*)(el->sd.attribute->pointers[el->fd.index]));
              }
              break;

          case TypeAttributeObjArray:
              {
                  AttributeArrayBase *a = 
                      (AttributeArrayBase*)(el->sd.attribute->pointers[el->fd.index]);
                  for (int j=0; j<el->sd.attribute->classIndex->lengths[el->fd.index]; j++)
                  {
                      ParseSkippingIfWrongType(a->GetAttributeAtIndex(j));
                  }
              }
              break;

          case TypeAttributeObjVector:
              {
                  AttributeVectorBase *v = 
                      (AttributeVectorBase*)(el->sd.attribute->pointers[el->fd.index]);
                  v->SetLength(el->fd.length);
                  bool hadError = false;
                  for (int j=0; j<v->GetLength(); j++)
                  {
                      Attribute *att = v->GetAttributeAtIndex(j);
                      string pt = ParseSkippingIfWrongType(att);
                      if (pt != att->GetType())
                      {
                          hadError = true;
                          //cerr << "Had error parsing "<<att->GetType()<<" got "<<pt<<endl;
                      }
                  }
                  if (hadError)
                  {
                      // maybe? v->EraseAll();
                      v->SetLength(0);
                  }
              }
              break;

              // attr ptr
          case TypeAttributePtr:
              {
                  if (*((Attribute**)(el->sd.attribute->pointers[el->fd.index])))
                      delete *((Attribute**)(el->sd.attribute->pointers[el->fd.index]));

                  Attribute *att = NULL;
                  att = Attribute::CreateAttribute(el->sd.attribute->classIndex->subtypes[el->fd.index]);
                  string pt = ParseSkippingIfWrongType(att);
                  if (pt == "NULL")
                  {
                      delete att;
                      att = NULL;
                  }
                  *((Attribute**)el->sd.attribute->pointers[el->fd.index]) = att;
              }
              break;

          case TypeAttributePtrArray:
              {
                  AttributeArrayBase *a = 
                      (AttributeArrayBase*)(el->sd.attribute->pointers[el->fd.index]);
                  a->EraseAll(el->sd.attribute->classIndex->lengths[el->fd.index]);
                  for (int j=0; j<el->sd.attribute->classIndex->lengths[el->fd.index]; j++)
                  {
                      Attribute *att = NULL;
                      att = Attribute::CreateAttribute(el->sd.attribute->classIndex->subtypes[el->fd.index]);
                      string pt = ParseSkippingIfWrongType(att);
                      if (pt == "NULL")
                      {
                          delete att;
                          att = NULL;
                      }
                      a->SetAttributeAtIndex(j, att);
                  }
              }
              break;

          case TypeAttributePtrVector:
              {
                  AttributeVectorBase *v = 
                      (AttributeVectorBase*)(el->sd.attribute->pointers[el->fd.index]);
                  v->EraseAll();
                  v->SetLength(el->fd.length);
                  for (int j=0; j<v->GetLength(); j++)
                  {
                      Attribute *att = NULL;
                      att = Attribute::CreateAttribute(el->sd.attribute->classIndex->subtypes[el->fd.index]);
                      string pt = ParseSkippingIfWrongType(att);
                      if (pt == "NULL")
                      {
                          delete att;
                          att = NULL;
                      }
                      v->SetAttributeAtIndex(j, att);
                  }
              }
              break;


          case TypeDynamicPtr:
              {
                  if (*((Attribute**)(el->sd.attribute->pointers[el->fd.index])))
                      delete *((Attribute**)(el->sd.attribute->pointers[el->fd.index]));

                  Attribute *att = ParseCreatingNeededType();
                  *((Attribute**)el->sd.attribute->pointers[el->fd.index]) = att;
              }
              break;

          case TypeDynamicPtrArray:
              {
                  AttributeArrayBase *a = 
                      (AttributeArrayBase*)(el->sd.attribute->pointers[el->fd.index]);
                  a->EraseAll(el->sd.attribute->classIndex->lengths[el->fd.index]);
                  for (int j=0; j<el->sd.attribute->classIndex->lengths[el->fd.index]; j++)
                  {
                      Attribute *att = ParseCreatingNeededType();
                      a->SetAttributeAtIndex(j, att);
                  }
              }
              break;

          case TypeDynamicPtrVector:
              {
                  AttributeVectorBase *v = 
                      (AttributeVectorBase*)(el->sd.attribute->pointers[el->fd.index]);
                  v->EraseAll();
                  v->SetLength(el->fd.length);
                  for (int j=0; j<v->GetLength(); j++)
                  {
                      Attribute *att = ParseCreatingNeededType();
                      v->SetAttributeAtIndex(j, att);
                  }
              }
              break;

          default:
            throw Exception("Logic Error");
        }
    }
}


void XMLUnserializer::beginElement(const string &element,
                                   const XMLAttributes &atts)
{
    XMLParseStackElement *el = stack.back();
    if (!el->sd.initializedAttribute)
    {
        beginAttribute(element, atts);
    }
    else
    {
        beginField(element, atts);
    }
}


void XMLUnserializer::handleText(const string &text)
{
    XMLParseStackElement *el = stack.back();
    if (el->fd.skip)
        return;

    assert(el->sd.initializedAttribute);
    XMLUnserializePrimitiveFieldItem(el->fd.type,
                                     el->fd.subtype,
                                     text,
                                     el->sd.attribute->pointers,
                                     el->fd.length,
                                     el->fd.index,
                                     el->fd.position);
    el->fd.position++;
}

void XMLUnserializer::endElement(const string &)
{
    // Actually, nothing to do here
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Attribute
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
map<string,AttributeIndex*> Attribute::allClassIndex;
map<AttCreatorFn,SpecificType> Attribute::mapAttCreatorToSpecificType;

Attribute::Attribute() : populatingClassIndex(NULL), classIndex(NULL)
{
}

Attribute::~Attribute()
{
    if (classIndex)
        classIndex->FreePointers(pointers,pointerOwned);
}

void Attribute::EnsureIndexCreated()
{
    if (pointers.size() > 0)
        return;
    if (classIndex && !populatingClassIndex)
        return;

    if (!allClassIndex.count(GetType()))
    {
        populatingClassIndex = new AttributeIndex();
        classIndex = populatingClassIndex;
    }
    else
    {
        classIndex = allClassIndex[GetType()];
    }
    AddFields();

#if defined(DEBUG)
    // Sanity check!
    if (!populatingClassIndex &&
        classIndex->nfields != pointers.size())
    {
        throw Exception("There appear to be two different "
                        "attributes called %s", GetType());
    }
#endif

    if (populatingClassIndex)
    {
        allClassIndex[GetType()] = populatingClassIndex;
        populatingClassIndex = NULL;
    }
}

void Attribute::XMLSerialize(XMLSerializer *writer)
{
    EnsureIndexCreated();
    writer->Write(this);
}

void Attribute::XMLSerialize(ostream &out)
{
    XMLSerializer writer;
    writer.Open(out);
    XMLSerialize(&writer);
    writer.Close();
}

string Attribute::XMLSerialize()
{
    ostringstream ostr;
    XMLSerialize(ostr);
    return ostr.str();
}

void Attribute::XMLUnserialize(XMLUnserializer *reader)
{
    EnsureIndexCreated();
    reader->ParseErroringIfWrongType(this);
}

void Attribute::XMLUnserialize(istream &in)
{
    XMLUnserializer reader;
    reader.Initialize(in);
    XMLUnserialize(&reader);
}

void Attribute::XMLUnserialize(const string &s)
{
    istringstream istr;
    istr.str(s);
    XMLUnserialize(istr);
}

XMLUnserializer *Attribute::CreateXMLUnserializer(const string &s)
{
    return new XMLUnserializer(s);
}

void Attribute::FreeXMLUnserializer(XMLUnserializer *unser)
{
    delete unser;
}

// bool
void Attribute::Add(const string &n, bool &v)
{
    AddField(n, &v, 1, TypeBool);
}

// bool array
void Attribute::Add(const string &n, bool *v, int l)
{
    AddField(n, v, l, TypeBoolArray);
}

// bool vector
void Attribute::Add(const string &n, vector<bool> &v)
{
    AddField(n, &v, -1, TypeBoolVector);
}

// byte
void Attribute::Add(const string &n, byte &v)
{
    AddField(n, &v, 1, TypeByte);
}

// byte array
void Attribute::Add(const string &n, byte *v, int l)
{
    AddField(n, v, l, TypeByteArray);
}

// byte vector
void Attribute::Add(const string &n, vector<byte> &v)
{
    AddField(n, &v, -1, TypeByteVector);
}

// int32
void Attribute::Add(const string &n, int32 &v)
{
    AddField(n, &v, 1, TypeInt32);
}

// int32 array
void Attribute::Add(const string &n, int32 *v, int l)
{
    AddField(n, v, l, TypeInt32Array);
}

// int32 vector
void Attribute::Add(const string &n, vector<int32> &v)
{
    AddField(n, &v, -1, TypeInt32Vector);
}

// int64
void Attribute::Add(const string &n, int64 &v)
{
    AddField(n, &v, 1, TypeInt64);
}

// int64 array
void Attribute::Add(const string &n, int64 *v, int l)
{
    AddField(n, v, l, TypeInt64Array);
}

// int64 vector
void Attribute::Add(const string &n, vector<int64> &v)
{
    AddField(n, &v, -1, TypeInt64Vector);
}

// float
void Attribute::Add(const string &n, float &v)
{
    AddField(n, &v, 1, TypeFloat);
}

// float array
void Attribute::Add(const string &n, float *v, int l)
{
    AddField(n, v, l, TypeFloatArray);
}

// float vector
void Attribute::Add(const string &n, vector<float> &v)
{
    AddField(n, &v, -1, TypeFloatVector);
}

// double
void Attribute::Add(const string &n, double &v)
{
    AddField(n, &v, 1, TypeDouble);
}

// double array
void Attribute::Add(const string &n, double *v, int l)
{
    AddField(n, v, l, TypeDoubleArray);
}

// double vector
void Attribute::Add(const string &n, vector<double> &v)
{
    AddField(n, &v, -1, TypeDoubleVector);
}

// string
void Attribute::Add(const string &n, string &v)
{
    AddField(n, &v, 1, TypeString);
}

// string array
void Attribute::Add(const string &n, string *v, int l)
{
    AddField(n, v, l, TypeStringArray);
}

// string vector
void Attribute::Add(const string &n, vector<string> &v)
{
    AddField(n, &v, -1, TypeStringVector);
}

// ----------------------------------------------------------------------------

int Attribute::GetNumFields()
{
    EnsureIndexCreated();
    return classIndex->nfields;
}

BasicType Attribute::GetFieldType(int i)
{
    EnsureIndexCreated();
    return classIndex->types[i];
}

string Attribute::GetFieldTypeName(int i)
{
    EnsureIndexCreated();
    return TypeToString(classIndex->types[i],classIndex->subtypes[i]);
}

string Attribute::GetFieldSimpleTypeName(int i)
{
    EnsureIndexCreated();
    return TypeToBaseString(classIndex->types[i],classIndex->subtypes[i]);
}

BasicTypeCategory Attribute::GetFieldTypeCategory(int i)
{
    EnsureIndexCreated();
    return TypeCategory(classIndex->types[i]);
}

SpecificType Attribute::GetFieldSubtype(int i)
{
    EnsureIndexCreated();
    return classIndex->subtypes[i];
}

std::string Attribute::GetFieldName(int i)
{
    EnsureIndexCreated();
    return classIndex->names[i];
}

int Attribute::GetFieldLength(int i)
{
    EnsureIndexCreated();
    int length = classIndex->lengths[i];
    BasicType t = classIndex->types[i];
    void *p = pointers[i];
    switch (t)
    {
      case TypeBoolVector:
        length = ((vector<bool> *)p)->size();
        break;
      case TypeByteVector:
        length = ((vector<byte> *)p)->size();
        break;
      case TypeInt32Vector:
        length = ((vector<int32> *)p)->size();
        break;
      case TypeInt64Vector:
        length = ((vector<int64> *)p)->size();
        break;
      case TypeFloatVector:
        length = ((vector<float> *)p)->size();
        break;
      case TypeDoubleVector:
        length = ((vector<double> *)p)->size();
        break;
      case TypeStringVector:
        length = ((vector<string> *)p)->size();
        break;
      case TypeAttributeObjVector:
        length = ((AttributeVectorBase*)(p))->GetLength();
        break;
      case TypeAttributePtrVector:
        length = ((AttributeVectorBase*)(p))->GetLength();
        break;
      case TypeDynamicPtrVector:
        length = ((AttributeVectorBase*)(p))->GetLength();
        break;
      case TypePrimitiveVector:
        length = ((PrimitiveVectorBase*)(p))->GetLength();
        break;
      default:
        // not a vector; the stored length is correct
        break;
    }
    return length;
}

void Attribute::SetFieldLength(int i, int len)
{
    EnsureIndexCreated();
    BasicType t = classIndex->types[i];
    void *p = pointers[i];
    switch (t)
    {
      case TypeBoolVector:
        ((vector<bool> *)p)->resize(len);
        break;
      case TypeByteVector:
        ((vector<byte> *)p)->resize(len);
        break;
      case TypeInt32Vector:
        ((vector<int32> *)p)->resize(len);
        break;
      case TypeInt64Vector:
        ((vector<int64> *)p)->resize(len);
        break;
      case TypeFloatVector:
        ((vector<float> *)p)->resize(len);
        break;
      case TypeDoubleVector:
        ((vector<double> *)p)->resize(len);
        break;
      case TypeStringVector:
        ((vector<string> *)p)->resize(len);
        break;
      case TypeAttributeObjVector:
        ((AttributeVectorBase*)(p))->SetLength(len);
        break;
      case TypeAttributePtrVector:
        ((AttributeVectorBase*)(p))->SetLength(len);
        break;
      case TypeDynamicPtrVector:
        ((AttributeVectorBase*)(p))->SetLength(len);
        break;
      case TypePrimitiveVector:
        ((PrimitiveVectorBase*)(p))->SetLength(len);
        break;
      default:
        throw "Error: can only set field length on vector types\n";
        break;
    }
}

long Attribute::GetFieldAsLong(int i, int si)
{
    EnsureIndexCreated();
    BasicType type = classIndex->types[i];
    switch (type)
    {
        // bool
      case TypeBool:
      case TypeBoolArray:
        return ((bool*)pointers[i])[si];

      case TypeBoolVector:
        return ((vector<bool> *)pointers[i])->operator[](si);

        // byte
      case TypeByte:
      case TypeByteArray:
        return ((byte*)pointers[i])[si];

      case TypeByteVector:
        return ((vector<byte> *)pointers[i])->operator[](si);

        // int
      case TypeInt32:
      case TypeInt32Array:
        return ((int32*)pointers[i])[si];

      case TypeInt32Vector:
        return ((vector<int32> *)pointers[i])->operator[](si);

        // int64
      case TypeInt64:
      case TypeInt64Array:
        return ((int64*)pointers[i])[si];

      case TypeInt64Vector:
        return ((vector<int64> *)pointers[i])->operator[](si);

        // float
      case TypeFloat:
      case TypeFloatArray:
        return ((float*)pointers[i])[si];

      case TypeFloatVector:
        return ((vector<float> *)pointers[i])->operator[](si);

        // double
      case TypeDouble:
      case TypeDoubleArray:
        return ((double*)pointers[i])[si];

      case TypeDoubleVector:
        return ((vector<double> *)pointers[i])->operator[](si);

        // string
      case TypeString:
      case TypeStringArray:
      case TypeStringVector:
        throw Exception("strings not supported in Attribute::GetFieldAsLong");

        // attr obj
        // attr ptr
        // dynamic ptr
      case TypeAttributeObj:
      case TypeAttributeObjArray:
      case TypeAttributeObjVector:
      case TypeAttributePtr:
      case TypeAttributePtrArray:
      case TypeAttributePtrVector:
      case TypeDynamicPtr:
      case TypeDynamicPtrArray:
      case TypeDynamicPtrVector:
        throw Exception("non-concrete type in Attribute::GetFieldAsLong");

      case TypeUnknown:
        throw Exception("unknown field type case in Attribute::GetFieldAsLong");

      case TypeUnset:
        throw Exception("unset field type in Attribute::GetFieldAsLong");

        // primitives
      case TypePrimitive:
      case TypePrimitiveArray:
      case TypePrimitiveVector:
        throw Exception("primitives not supported in Attribute::GetFieldAsLong");
    }
    throw Exception("logic error: uncaught type");
}

double Attribute::GetFieldAsDouble(int i, int si)
{
    EnsureIndexCreated();
    BasicType type = classIndex->types[i];
    switch (type)
    {
        // bool
      case TypeBool:
      case TypeBoolArray:
        return ((bool*)pointers[i])[si];

      case TypeBoolVector:
        return ((vector<bool> *)pointers[i])->operator[](si);

        // byte
      case TypeByte:
      case TypeByteArray:
        return ((byte*)pointers[i])[si];

      case TypeByteVector:
        return ((vector<byte> *)pointers[i])->operator[](si);

        // int
      case TypeInt32:
      case TypeInt32Array:
        return ((int32*)pointers[i])[si];

      case TypeInt32Vector:
        return ((vector<int32> *)pointers[i])->operator[](si);

        // int64
      case TypeInt64:
      case TypeInt64Array:
        return ((int64*)pointers[i])[si];

      case TypeInt64Vector:
        return ((vector<int64> *)pointers[i])->operator[](si);

        // float
      case TypeFloat:
      case TypeFloatArray:
        return ((float*)pointers[i])[si];

      case TypeFloatVector:
        return ((vector<float> *)pointers[i])->operator[](si);

        // double
      case TypeDouble:
      case TypeDoubleArray:
        return ((double*)pointers[i])[si];

      case TypeDoubleVector:
        return ((vector<double> *)pointers[i])->operator[](si);

        // string
      case TypeString:
      case TypeStringArray:
      case TypeStringVector:
        throw Exception("strings not supported in Attribute::GetFieldAsDouble");

        // attr obj
        // attr ptr
        // dynamic ptr
      case TypeAttributeObj:
      case TypeAttributeObjArray:
      case TypeAttributeObjVector:
      case TypeAttributePtr:
      case TypeAttributePtrArray:
      case TypeAttributePtrVector:
      case TypeDynamicPtr:
      case TypeDynamicPtrArray:
      case TypeDynamicPtrVector:
        throw Exception("non-concrete type in Attribute::GetFieldAsDouble");

      case TypeUnknown:
        throw Exception("unknown field type case in Attribute::GetFieldAsDouble");

      case TypeUnset:
        throw Exception("unset field type in Attribute::GetFieldAsDouble");

        // primitives
      case TypePrimitive:
      case TypePrimitiveArray:
      case TypePrimitiveVector:
        throw Exception("primitives not supported in Attribute::GetFieldAsDouble");
    }
    throw Exception("logic error: uncaught type");
}


string Attribute::GetFieldAsString(int i, int si)
{
    EnsureIndexCreated();
    BasicType type = classIndex->types[i];
    switch (type)
    {
      case TypeBool:
      case TypeBoolArray:
      case TypeBoolVector:
      case TypeByte:
      case TypeByteArray:
      case TypeByteVector:
      case TypeInt32:
      case TypeInt32Array:
      case TypeInt32Vector:
      case TypeInt64:
      case TypeInt64Array:
      case TypeInt64Vector:
      case TypeFloat:
      case TypeFloatArray:
      case TypeFloatVector:
      case TypeDouble:
      case TypeDoubleArray:
      case TypeDoubleVector:
        throw Exception("numerical types not supported in Attribute::GetFieldAsString");

        // string
      case TypeString:
      case TypeStringArray:
        return ((string*)pointers[i])[si];

      case TypeStringVector:
        return ((vector<string> *)pointers[i])->operator[](si);

        // attr obj
        // attr ptr
        // dynamic ptr
      case TypeAttributeObj:
      case TypeAttributeObjArray:
      case TypeAttributeObjVector:
      case TypeAttributePtr:
      case TypeAttributePtrArray:
      case TypeAttributePtrVector:
      case TypeDynamicPtr:
      case TypeDynamicPtrArray:
      case TypeDynamicPtrVector:
        throw Exception("non-concrete type in Attribute::GetFieldAsString");

      case TypeUnknown:
        throw Exception("unknown field type case in Attribute::GetFieldAsString");

      case TypeUnset:
        throw Exception("unset field type in Attribute::GetFieldAsString");

        // primitives
      case TypePrimitive:
      case TypePrimitiveArray:
      case TypePrimitiveVector:
        throw Exception("primitives not supported in Attribute::GetFieldAsString");
    }
    throw Exception("logic error: uncaught type");
}

void Attribute::SetFieldFromLong(long v, int i, int si)
{
    EnsureIndexCreated();
    BasicType type = classIndex->types[i];
    switch (type)
    {
        // bool
      case TypeBool:
      case TypeBoolArray:
        ((bool*)pointers[i])[si] = v;
        break;

      case TypeBoolVector:
        ((vector<bool> *)pointers[i])->operator[](si) = v;
        break;

        // byte
      case TypeByte:
      case TypeByteArray:
        ((byte*)pointers[i])[si] = v;
        break;

      case TypeByteVector:
        ((vector<byte> *)pointers[i])->operator[](si) = v;
        break;

        // int
      case TypeInt32:
      case TypeInt32Array:
        ((int32*)pointers[i])[si] = v;
        break;

      case TypeInt32Vector:
        ((vector<int32> *)pointers[i])->operator[](si) = v;
        break;

        // int64
      case TypeInt64:
      case TypeInt64Array:
        ((int64*)pointers[i])[si] = v;
        break;

      case TypeInt64Vector:
        ((vector<int64> *)pointers[i])->operator[](si) = v;
        break;

        // float
      case TypeFloat:
      case TypeFloatArray:
        ((float*)pointers[i])[si] = v;
        break;

      case TypeFloatVector:
        ((vector<float> *)pointers[i])->operator[](si) = v;
        break;

        // double
      case TypeDouble:
      case TypeDoubleArray:
        ((double*)pointers[i])[si] = v;
        break;

      case TypeDoubleVector:
        ((vector<double> *)pointers[i])->operator[](si) = v;
        break;

        // string
      case TypeString:
      case TypeStringArray:
      case TypeStringVector:
        throw Exception("strings not supported in Attribute::GetFieldAsLong");

        // attr obj
        // attr ptr
        // dynamic ptr
      case TypeAttributeObj:
      case TypeAttributeObjArray:
      case TypeAttributeObjVector:
      case TypeAttributePtr:
      case TypeAttributePtrArray:
      case TypeAttributePtrVector:
      case TypeDynamicPtr:
      case TypeDynamicPtrArray:
      case TypeDynamicPtrVector:
        throw Exception("non-concrete type in Attribute::GetFieldAsLong");

      case TypeUnknown:
        throw Exception("unknown field type case in Attribute::GetFieldAsLong");

      case TypeUnset:
        throw Exception("unset field type in Attribute::GetFieldAsLong");

        // primitives
      case TypePrimitive:
      case TypePrimitiveArray:
      case TypePrimitiveVector:
        throw Exception("primitives not supported in Attribute::GetFieldAsLong");
    }
}

void Attribute::SetFieldFromDouble(double v, int i, int si)
{
    EnsureIndexCreated();
    BasicType type = classIndex->types[i];
    switch (type)
    {
        // bool
      case TypeBool:
      case TypeBoolArray:
        ((bool*)pointers[i])[si] = v;
        break;

      case TypeBoolVector:
        ((vector<bool> *)pointers[i])->operator[](si) = v;
        break;

        // byte
      case TypeByte:
      case TypeByteArray:
        ((byte*)pointers[i])[si] = v;
        break;

      case TypeByteVector:
        ((vector<byte> *)pointers[i])->operator[](si) = v;
        break;

        // int
      case TypeInt32:
      case TypeInt32Array:
        ((int32*)pointers[i])[si] = v;
        break;

      case TypeInt32Vector:
        ((vector<int32> *)pointers[i])->operator[](si) = v;
        break;

        // int64
      case TypeInt64:
      case TypeInt64Array:
        ((int64*)pointers[i])[si] = v;
        break;

      case TypeInt64Vector:
        ((vector<int64> *)pointers[i])->operator[](si) = v;
        break;

        // float
      case TypeFloat:
      case TypeFloatArray:
        ((float*)pointers[i])[si] = v;
        break;

      case TypeFloatVector:
        ((vector<float> *)pointers[i])->operator[](si) = v;
        break;

        // double
      case TypeDouble:
      case TypeDoubleArray:
        ((double*)pointers[i])[si] = v;
        break;

      case TypeDoubleVector:
        ((vector<double> *)pointers[i])->operator[](si) = v;
        break;

        // string
      case TypeString:
      case TypeStringArray:
      case TypeStringVector:
        throw Exception("strings not supported in Attribute::GetFieldAsDouble");

        // attr obj
        // attr ptr
        // dynamic ptr
      case TypeAttributeObj:
      case TypeAttributeObjArray:
      case TypeAttributeObjVector:
      case TypeAttributePtr:
      case TypeAttributePtrArray:
      case TypeAttributePtrVector:
      case TypeDynamicPtr:
      case TypeDynamicPtrArray:
      case TypeDynamicPtrVector:
        throw Exception("non-concrete type in Attribute::GetFieldAsDouble");

      case TypeUnknown:
        throw Exception("unknown field type case in Attribute::GetFieldAsDouble");

      case TypeUnset:
        throw Exception("unset field type in Attribute::GetFieldAsDouble");

        // primitives
      case TypePrimitive:
      case TypePrimitiveArray:
      case TypePrimitiveVector:
        throw Exception("primitives not supported in Attribute::GetFieldAsDouble");
    }
}


void Attribute::SetFieldFromString(string v, int i, int si)
{
    EnsureIndexCreated();
    BasicType type = classIndex->types[i];
    switch (type)
    {
      case TypeBool:
      case TypeBoolArray:
      case TypeBoolVector:
      case TypeByte:
      case TypeByteArray:
      case TypeByteVector:
      case TypeInt32:
      case TypeInt32Array:
      case TypeInt32Vector:
      case TypeInt64:
      case TypeInt64Array:
      case TypeInt64Vector:
      case TypeFloat:
      case TypeFloatArray:
      case TypeFloatVector:
      case TypeDouble:
      case TypeDoubleArray:
      case TypeDoubleVector:
        throw Exception("numerical types not supported in Attribute::GetFieldAsString");

        // string
      case TypeString:
      case TypeStringArray:
        ((string*)pointers[i])[si] = v;
        break;

      case TypeStringVector:
        ((vector<string> *)pointers[i])->operator[](si) = v;
        break;

        // attr obj
        // attr ptr
        // dynamic ptr
      case TypeAttributeObj:
      case TypeAttributeObjArray:
      case TypeAttributeObjVector:
      case TypeAttributePtr:
      case TypeAttributePtrArray:
      case TypeAttributePtrVector:
      case TypeDynamicPtr:
      case TypeDynamicPtrArray:
      case TypeDynamicPtrVector:
        throw Exception("non-concrete type in Attribute::GetFieldAsString");

      case TypeUnknown:
        throw Exception("unknown field type case in Attribute::GetFieldAsString");

      case TypeUnset:
        throw Exception("unset field type in Attribute::GetFieldAsString");

        // primitives
      case TypePrimitive:
      case TypePrimitiveArray:
      case TypePrimitiveVector:
        throw Exception("primitives not supported in Attribute::GetFieldAsString");
    }
}

Attribute *Attribute::GetFieldAsAttribute(int i, int si)
{
    EnsureIndexCreated();
    BasicType type = classIndex->types[i];
    switch (type)
    {
      case TypeBool:
      case TypeBoolArray:
      case TypeBoolVector:
      case TypeByte:
      case TypeByteArray:
      case TypeByteVector:
      case TypeInt32:
      case TypeInt32Array:
      case TypeInt32Vector:
      case TypeInt64:
      case TypeInt64Array:
      case TypeInt64Vector:
      case TypeFloat:
      case TypeFloatArray:
      case TypeFloatVector:
      case TypeDouble:
      case TypeDoubleArray:
      case TypeDoubleVector:
      case TypeString:
      case TypeStringArray:
      case TypeStringVector:
        throw Exception("simple types not supported in Attribute::GetFieldAsAttribute");

        // attr obj
      case TypeAttributeObj:
        return &(((Attribute*)(pointers[i]))[si]);

        // attr ptr
        // dynamic ptr
      case TypeAttributePtr:
      case TypeDynamicPtr:
        return ((Attribute**)(pointers[i]))[si];

        // attr obj
        // attr ptr
        // dynamic ptr
      case TypeAttributeObjArray:
      case TypeAttributePtrArray:
      case TypeDynamicPtrArray:
        return ((AttributeArrayBase*)(pointers[i]))->GetAttributeAtIndex(si);

      case TypeAttributeObjVector:
      case TypeAttributePtrVector:
      case TypeDynamicPtrVector:
        return ((AttributeVectorBase*)(pointers[i]))->GetAttributeAtIndex(si);

      case TypeUnknown:
        throw Exception("unknown field type case in Attribute::GetFieldAsAttribute");

      case TypeUnset:
        throw Exception("unset field type in Attribute::GetFieldAsAttribute");

        // primitives
      case TypePrimitive:
      case TypePrimitiveArray:
      case TypePrimitiveVector:
        throw Exception("primitives not supported in Attribute::GetFieldAsAttribute");
    }
    throw Exception("logic error: uncaught type");
}

Primitive *Attribute::GetFieldAsPrimitive(int i, int si)
{
    EnsureIndexCreated();
    BasicType type = classIndex->types[i];
    switch (type)
    {
      case TypeBool:
      case TypeBoolArray:
      case TypeBoolVector:
      case TypeByte:
      case TypeByteArray:
      case TypeByteVector:
      case TypeInt32:
      case TypeInt32Array:
      case TypeInt32Vector:
      case TypeInt64:
      case TypeInt64Array:
      case TypeInt64Vector:
      case TypeFloat:
      case TypeFloatArray:
      case TypeFloatVector:
      case TypeDouble:
      case TypeDoubleArray:
      case TypeDoubleVector:
      case TypeString:
      case TypeStringArray:
      case TypeStringVector:
        throw Exception("simple types not supported in Attribute::GetFieldAsPrimitive");

      case TypeAttributeObj:
      case TypeAttributePtr:
      case TypeDynamicPtr:
      case TypeAttributeObjArray:
      case TypeAttributePtrArray:
      case TypeDynamicPtrArray:
      case TypeAttributeObjVector:
      case TypeAttributePtrVector:
      case TypeDynamicPtrVector:
        throw Exception("attributes not supported in Attribute::GetFieldAsPrimitive");

      case TypeUnknown:
        throw Exception("unknown field type case in Attribute::GetFieldAsPrimitive");

      case TypeUnset:
        throw Exception("unset field type in Attribute::GetFieldAsPrimitive");

        // primitives
      case TypePrimitive:
        return &(((Primitive*)pointers[i])[si]);
        break;

      case TypePrimitiveArray:
        return ((PrimitiveArrayBase*)(pointers[i]))->GetPrimitiveAtIndex(si);
        break;

      case TypePrimitiveVector:
        return ((PrimitiveVectorBase*)(pointers[i]))->GetPrimitiveAtIndex(si);
        break;
    }
    throw Exception("logic error: uncaught type");
}

// ----------------------------------------------------------------------------

void Attribute::CopyFrom(Attribute &a)
{
    EnsureIndexCreated();
    a.EnsureIndexCreated();

    if (classIndex != a.classIndex)
        throw Exception("Tried to assign an attribute %s to "
                        "incompatible type %s",
                        a.GetType(), GetType());

    classIndex->Copy(pointers, a.pointers);
}

Attribute *Attribute::CreateAttribute(const string &type)
{
    SpecificType st = mapSpecificTypeNameToSpecificType[type];
    if (st != 0)
    {
        return allCreators[st].attCreator();
    }
    else
    {
        throw Exception("Attribute::Create(): type %s wasn't registered",
                        type.c_str());
    }
}

Attribute *Attribute::CreateAttribute(SpecificType st)
{
    if (st != 0)
    {
        return allCreators[st].attCreator();
    }
    else
    {
        throw Exception("Attribute::Create(): Tried to create attribute "
                        "with SpecificType of 0");
    }
}

// ----------------------------------------------------------------------------

AttributeIndex::AttributeIndex()
{
    nfields = 0;
}

void AttributeIndex::FreePointers(const vector<void*> &pointers,
                                    const vector<bool>  &pointerOwned)
{
    // TODO: set pointerOwned to false;

    for (unsigned int i=0; i<nfields; i++)
    {
        if (pointers.size() <= i)
            continue;

        if (pointerOwned[i] && pointers[i])
        {
            switch (types[i])
            {
                // bool
              case TypeBool:
                delete ((bool*)pointers[i]);
                break;

              case TypeBoolArray:
                delete[] (bool*)pointers[i];
                break;

              case TypeBoolVector:
                delete ((vector<bool> *)pointers[i]);
                break;

                // byte
              case TypeByte:
                delete ((byte*)pointers[i]);
                break;

              case TypeByteArray:
                delete[] (byte*)pointers[i];
                break;

              case TypeByteVector:
                delete ((vector<byte> *)pointers[i]);
                break;

                // int
              case TypeInt32:
                delete ((int32*)pointers[i]);
                break;

              case TypeInt32Array:
                delete[] (int32*)pointers[i];
                break;

              case TypeInt32Vector:
                delete ((vector<int32> *)pointers[i]);
                break;

                // int64
              case TypeInt64:
                delete ((int64*)pointers[i]);
                break;

              case TypeInt64Array:
                delete[] (int64*)pointers[i];
                break;

              case TypeInt64Vector:
                delete ((vector<int64> *)pointers[i]);
                break;

                // float
              case TypeFloat:
                delete ((float*)pointers[i]);
                break;

              case TypeFloatArray:
                delete[] (float*)pointers[i];
                break;

              case TypeFloatVector:
                delete ((vector<float> *)pointers[i]);
                break;

                // double
              case TypeDouble:
                delete ((double*)pointers[i]);
                break;

              case TypeDoubleArray:
                delete[] (double*)pointers[i];
                break;

              case TypeDoubleVector:
                delete ((vector<double> *)pointers[i]);
                break;

                // string
              case TypeString:
                delete ((string*)pointers[i]);
                break;

              case TypeStringArray:
                delete[] (string*)pointers[i];
                break;

              case TypeStringVector:
                delete ((vector<string> *)pointers[i]);
                break;

                // attr obj
              case TypeAttributeObj:
                delete ((Attribute*)pointers[i]);
                break;

              case TypeAttributeObjArray:
                ((AttributeArrayBase*)(pointers[i]))->FreeContainer();
                break;

              case TypeAttributeObjVector:
                ((AttributeVectorBase*)(pointers[i]))->FreeContainer();
                break;

                // attr ptr
              case TypeAttributePtr:
                delete ((Attribute**)(pointers[i]));
                break;

              case TypeAttributePtrArray:
                ((AttributeArrayBase*)(pointers[i]))->FreeContainer();
                break;

              case TypeAttributePtrVector:
                ((AttributeVectorBase*)(pointers[i]))->FreeContainer();
                break;

                // dynamic ptr
              case TypeDynamicPtr:
                delete ((Attribute**)(pointers[i]));
                break;

              case TypeDynamicPtrArray:
                ((AttributeArrayBase*)(pointers[i]))->FreeContainer();
                break;

              case TypeDynamicPtrVector:
                ((AttributeVectorBase*)(pointers[i]))->FreeContainer();
                break;

              case TypeUnknown:
                throw Exception("unknown field type case in ClassIndex::FreePointers()");

              case TypeUnset:
                throw Exception("unset field type in ClassIndex::FreePointers()");

                // primitives
              case TypePrimitive:
                delete ((Primitive*)pointers[i]);
                break;

              case TypePrimitiveArray:
                ((PrimitiveArrayBase*)(pointers[i]))->FreeContainer();
                break;

              case TypePrimitiveVector:
                ((PrimitiveVectorBase*)(pointers[i]))->FreeContainer();
                break;
            }
        }

        if (pointers[i])
        {
            switch (types[i])
            {
              case TypeAttributeObjArray:
              case TypeAttributePtrArray:
              case TypeDynamicPtrArray:
                delete ((AttributeArrayBase*)(pointers[i]));
                break;
              case TypeAttributeObjVector:
              case TypeAttributePtrVector:
              case TypeDynamicPtrVector:
                delete ((AttributeVectorBase*)(pointers[i]));
                break;
              case TypePrimitiveArray:
                delete ((PrimitiveArrayBase*)(pointers[i]));
                break;
              case TypePrimitiveVector:
                delete ((PrimitiveVectorBase*)(pointers[i]));
                break;
              default:
                // not a vector to free
                break;
            }
        }
    }
}

void AttributeIndex::Copy(vector<void*> &, const vector<void*> &)
{
    ///\todo: can we just serialize and unserialize to be lazy?
    throw Exception("Unimplemented: AttributeIndex::Copy.");
}

void AttributeIndex::AddField(const string &n, int l,
                          BasicType t, SpecificType st)
{
    fieldmap[n] = nfields;
    nfields++;
    types.push_back(t);
    lengths.push_back(l);
    names.push_back(n);
    subtypes.push_back(st);
}

void AttributeIndex::AddGenericField(const string &name, int length, BasicType type, SpecificType st)
{
    switch (type)
    {
      case TypeUnset:
        throw Exception("AttributeIndex::AddGenericField: TypeUnset received");

      case TypeBool:
      case TypeByte:
      case TypeInt32:
      case TypeInt64:
      case TypeFloat:
      case TypeDouble:
      case TypeString:
      case TypeAttributeObj:
      case TypeAttributePtr:
      case TypeDynamicPtr:
      case TypePrimitive:
        AddField(name, 1, type, st);
        break;

      case TypeBoolArray:
      case TypeByteArray:
      case TypeInt32Array:
      case TypeInt64Array:
      case TypeFloatArray:
      case TypeDoubleArray:
      case TypeStringArray:
      case TypeAttributeObjArray:
      case TypeAttributePtrArray:
      case TypeDynamicPtrArray:
      case TypePrimitiveArray:
        AddField(name, length, type, st);
        break;

      case TypeBoolVector:
      case TypeByteVector:
      case TypeInt32Vector:
      case TypeInt64Vector:
      case TypeFloatVector:
      case TypeDoubleVector:
      case TypeStringVector:
      case TypeAttributeObjVector:
      case TypeAttributePtrVector:
      case TypeDynamicPtrVector:
      case TypePrimitiveVector:
        AddField(name, -1, type, st);
        break;

      case TypeUnknown:
        throw Exception("AttributeIndex::AddGenericField: Unknown type");

    }
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// GenericAttribute.cpp
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
GenericAttribute::GenericAttribute() : Attribute(), type("Generic")
{
}

GenericAttribute::~GenericAttribute()
{
}

void GenericAttribute::EnsureIndexCreated()
{
    if (pointers.size() > 0)
        return;
    if (classIndex && !populatingClassIndex)
        return;

    if (allClassIndex.count(type))
    {
        classIndex = allClassIndex[type];
    }
}

GenericAttribute::GenericAttribute(const GenericAttribute &g)
    : Attribute()
{
    type = g.type;
    // NEED TO MAKE USE OF THE ACTUAL GENERIC (unimplemented
    // except for the stub) COPY() routine in ClassIndex
}

const GenericAttribute &GenericAttribute::operator=(const GenericAttribute &g)
{
    type = g.type;
    throw Exception("Don't know how to copy GenericAttribute's yet");
    // NEED TO MAKE USE OF THE ACTUAL GENERIC (unimplemented
    // except for the stub) COPY() routine in ClassIndex
}


void GenericAttribute::XMLUnserialize(XMLUnserializer *reader)
{
    EnsureIndexCreated();
    reader->ParseCreatingNeededFields(this);
}

void GenericAttribute::SetFieldLength(int, int)
{
    throw Exception("Can't resize vectors in a GenericAttribute; we can "
                    "only create top-level structure in a GenericAttribute "
                    "when unserializing them.  Otherwise, it's best to "
                    "change at most only the contents of a GenericAttribute.");
}
