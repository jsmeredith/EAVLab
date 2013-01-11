// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include "STL.h"
#include "Exception.h"

class Attribute;
class AttributeIndex;
class Primitive;
class PrimitiveArrayBase;
class PrimitiveVectorBase;
class XMLUnserializer;
class XMLSerializer;

// ****************************************************************************
// ----------------------------------------------------------------------------
// Structures and methods for labeling field and attribute types in Attributes
// ----------------------------------------------------------------------------
// ****************************************************************************
typedef unsigned char  byte;
typedef int            int32;
typedef long long      int64;


enum BasicType
{
    TypeUnset,
    TypeDynamicPtr,   TypeDynamicPtrArray,   TypeDynamicPtrVector,
    TypeAttributeObj, TypeAttributeObjArray, TypeAttributeObjVector,
    TypeAttributePtr, TypeAttributePtrArray, TypeAttributePtrVector,
    TypeBool,         TypeBoolArray,         TypeBoolVector,
    TypeByte,         TypeByteArray,         TypeByteVector,
    TypeInt32,        TypeInt32Array,        TypeInt32Vector,
    TypeInt64,        TypeInt64Array,        TypeInt64Vector,
    TypeFloat,        TypeFloatArray,        TypeFloatVector,
    TypeDouble,       TypeDoubleArray,       TypeDoubleVector,
    TypeString,       TypeStringArray,       TypeStringVector,
    TypePrimitive,    TypePrimitiveArray,    TypePrimitiveVector,
    TypeUnknown
};

enum BasicTypeCategory
{
    CategoryBoolean,
    CategoryIntegral,
    CategoryReal,
    CategoryString,
    CategoryAttribute,
    CategoryPrimitive
};

typedef int64 SpecificType;

typedef Attribute *(*AttCreatorFn)(void);
typedef Primitive *(*PrimCreatorFn)(void);
typedef PrimitiveArrayBase *(*PrimArrCreatorFn)(int);
typedef PrimitiveVectorBase *(*PrimVecCreatorFn)(int);


// ****************************************************************************
// Class: Attribute 
//
// Purpose:
///   A base class to make an object serializable and introspectable.
///   To use it, you must:
///   - Derive from Attribute
///   - Implement virtual const char *GetType() which returns a unique
///     name for your attribute, preferably simply the class name.
///   - Implement AddFields, which calls AddField(name, member [,length]) for
///     each data member; note that for C arrays you must pass length
///   - If you plan to use your Attribute within another Attribute, you
///     must also implement the static Attribute *::Create method which
///     returns a pointer to a newly created instance of your attribute.
///
///   Note that your data members can not only be simple types, they can
///   be other attributes; if you plan to use a base class (e.g. Attribute*)
///   as your data member and fill it with a concrete attribute at runtime,
///   this is fine and will be unserialized properly, but you must
///   call Attribute<yourclass>::Register() for each concrete attribute that
///   may be used in this way.
///
///   Note that you can also unserialize into an Attribute even for an 
///   Attribute type which is not defined in your program; see
///   GenericAttribute below.
///
///   Note that you can register new simple primitive types which are
///   more efficient in both usage, memory, and  than attributes.  See
///   the Primitive class below.
//
// Programmer:  Jeremy Meredith
// Creation:    August 13, 2012
//
// Modifications:
// ****************************************************************************
class Attribute
{
  public:
    Attribute();
    virtual ~Attribute();
    virtual const char *GetType() = 0;

    // introspection calls
    int          GetNumFields();
    BasicType    GetFieldType(int i);
    string       GetFieldTypeName(int i);
    string       GetFieldSimpleTypeName(int i);
    SpecificType GetFieldSubtype(int i);
    BasicTypeCategory GetFieldTypeCategory(int i);
    string       GetFieldName(int i);
    int          GetFieldLength(int i);

    long         GetFieldAsLong(int i, int si=0);
    double       GetFieldAsDouble(int i, int si=0);
    string       GetFieldAsString(int i, int si=0);

    virtual void SetFieldLength(int i, int l); // only for vectors!

    void         SetFieldFromLong(long v, int i, int si=0);
    void         SetFieldFromDouble(double v, int i, int si=0);
    void         SetFieldFromString(string v, int i, int si=0);

    Attribute   *GetFieldAsAttribute(int i, int si=0);
    Primitive   *GetFieldAsPrimitive(int i, int si=0);

    // serialization routines
    void         XMLSerialize(XMLSerializer *writer);
    void         XMLSerialize(ostream &out);
    string       XMLSerialize();
    virtual void XMLUnserialize(XMLUnserializer *reader);
    void         XMLUnserialize(istream &in);
    void         XMLUnserialize(const string &s);

    // to unserialize more than one in a row, you need a persisitent
    // unserializer; these two static functions accomplish that
    static XMLUnserializer *CreateXMLUnserializer(const string &s);
    static void             FreeXMLUnserializer(XMLUnserializer *reader);

    void CopyFrom(Attribute&);

    // static methods for creating/copying/analyzing attributes by typename
    static Attribute *CreateAttribute(const string &);
    static Attribute *CreateAttribute(SpecificType);
    template <class T> static SpecificType Register();

    // necessary evils to distinguish attributes from primitives
    static const char *GetPrimitiveType() { return "Attribute"; }
    static bool         IsPrimitive()  { return false; }

  protected:
    virtual void AddFields() = 0;

    // bool
    void Add(const string &n, bool &v);
    void Add(const string &n, bool *v, int l);
    void Add(const string &n, vector<bool> &v);
    // byte
    void Add(const string &n, byte &v);
    void Add(const string &n, byte *v, int l);
    void Add(const string &n, vector<byte> &v);
    // int32
    void Add(const string &n, int32 &v);
    void Add(const string &n, int32 *v, int l);
    void Add(const string &n, vector<int32> &v);
    // int64
    void Add(const string &n, int64 &v);
    void Add(const string &n, int64 *v, int l);
    void Add(const string &n, vector<int64> &v);
    // float
    void Add(const string &n, float &v);
    void Add(const string &n, float *v, int l);
    void Add(const string &n, vector<float> &v);
    // double
    void Add(const string &n, double &v);
    void Add(const string &n, double *v, int l);
    void Add(const string &n, vector<double> &v);
    // string
    void Add(const string &n, string &v);
    void Add(const string &n, string *v, int l);
    void Add(const string &n, vector<string> &v);
    // attribute obj
    template <class T> void Add(const string &n, T &v);
    template <class T> void Add(const string &n, T *v, int l);
    template <class T> void Add(const string &n, vector<T> &v);
    // attribute ptr
    template <class T> void Add(const string &n, T *&v);
    template <class T> void Add(const string &n, T **v, int l);
    template <class T> void Add(const string &n, vector<T*> &v);

  private:
    friend class XMLUnserializer;
    friend class XMLSerializer;

    void AddField(const string &n,void *v,int l,BasicType t,SpecificType st=0);
    Attribute(const Attribute&);
    const Attribute &operator=(Attribute&);

  protected:
    static map<string,AttributeIndex*>             allClassIndex;
    static map<AttCreatorFn,SpecificType> mapAttCreatorToSpecificType;

    vector<void*>                         pointers;
    vector<bool>                          pointerOwned;
    AttributeIndex                                *populatingClassIndex;
    AttributeIndex                                *classIndex;

    virtual void EnsureIndexCreated();

};

// ****************************************************************************
// Class:  Primitive
//
// Purpose:
///   A base class lightweight object which can be contained within attributes.
///   It is cleaner and faster than using a nested attribute.
///
///   For example, suppose you created a ColorControlPoint Primitive (with a
///   float for position and 3 integers, one each for its red, green, and blue
///   values) and used it in a ColorTable Attribute as ColorControlPoint[100].
///   This is simpler and faster than if ColorControlPoint were an Attribute,
///   because (a) we make optimizations based on it not having individually
///   addressible fields or containing other attributes, and (b) when it is
///   serialized, it will be a flattened array of 400 values (float int int
///   int float int int int, etc.)  removing all the space and time overhead
///   of having to parse a header for each of those 100 instances, as well
///   as being much simpler for a human and other readers to understand.
///
///   Note that a GenericAttribute will correctly handle Primitives
///   as long as those primitives are registered first.
///
///   An example usage: while some Attribute types might only exist in some
///   processes of a distributed program, each Primitive is intended to
///   be properly declared and registered in every process.
//
// Programmer:  Jeremy Meredith
// Creation:    August 13, 2012
//
// Modifications:
// ****************************************************************************
class Primitive
{
  private:
    static map<PrimCreatorFn,SpecificType> mapPrimCreatorToSpecificType;

  public:
    static bool         IsPrimitive()  { return true; }

    virtual void XMLSerialize(ostream &out, int field)      = 0;
    virtual void XMLUnserialize(const string &s, int field) = 0;
    virtual int  NumFields()                                = 0;

    operator const Attribute*() const { throw; }

    template <class T> static SpecificType Register();
    template <class T> static SpecificType GetSpecificType();
  private:
};

// ****************************************************************************
// Class:  GenericAttribute
//
// Purpose:
///   A GenericAttribute is a concrete Attribute that can be unserialized
///   from any valid attribute and then introspected as if it were
///   the original type.
///
///   For example, suppose you are a GUI talking to a remote process over a
///   network connection, and that remote process sends you a serialized
///   Attribute from a plug-in, e.g. HDF5ReaderOptions.  Though your GUI
///   process has no HDF5ReaderOptions concrete type in its source code
///   base, you can unserialize it into a GenericAttribute and introspet
///   the generic type, e.g. create widgets based on its data members.
///
///   Note that GenericAttribute will correctly create Primitives and
///   any dynamically registered attributes if those are contained
///   in the serialized object.
///
///   Also, note that a GenericAttribute has the same type name as the
///   concrete attribute it was serialized from; if you try to unserialize
///   from an object with the same type name but a different structure,
///   it will detect that things changed and generate errors or worse, cause
///   other bad things to happen.  So don't do that.
//
// Programmer:  Jeremy Meredith
// Creation:    August 13, 2012
//
// Modifications:
// ****************************************************************************
class GenericAttribute : public Attribute
{
  private:
    friend class XMLUnserializer;
  private:
    string type;
  public:
    virtual const char *GetType() {return type.c_str();}
    GenericAttribute();
    GenericAttribute(const GenericAttribute &g);
    const GenericAttribute &operator=(const GenericAttribute &g);
    virtual ~GenericAttribute();
    static Attribute *Create() { return new GenericAttribute; }
    virtual void AddFields() { }
    virtual void XMLUnserialize(XMLUnserializer *reader);
    using Attribute::XMLUnserialize;
    virtual void EnsureIndexCreated();
    virtual void SetFieldLength(int i, int l); // unsupported; fail
};


// ****************************************************************************
// ----------------------------------------------------------------------------
// Container types for Primitive arrays and vectors; for internal use only
// ----------------------------------------------------------------------------
// ****************************************************************************
class PrimitiveVectorBase
{
  public:
    PrimitiveVectorBase();
    virtual ~PrimitiveVectorBase();
    virtual int  GetLength() = 0;
    virtual void SetLength(int) = 0;
    virtual Primitive *GetPrimitiveAtIndex(int) = 0;
    virtual void EraseAll() { }
    virtual void FreeContainer() = 0;
};

class PrimitiveArrayBase
{
  public:
    PrimitiveArrayBase();
    virtual ~PrimitiveArrayBase();
    virtual Primitive *GetPrimitiveAtIndex(int)=0;
    virtual void EraseAll(int) { }
    virtual void FreeContainer() = 0;
};

template<class T>
class PrimitiveVector : public PrimitiveVectorBase
{
  private:
    vector<T> *vec;
  public:
    PrimitiveVector<T>(vector<T> &v) : PrimitiveVectorBase(), vec(&v) { }
    virtual ~PrimitiveVector<T>() { }
    virtual int GetLength() { return vec->size(); }
    virtual void SetLength(int l) { vec->resize(l); }
    virtual void Clear() { vec->clear(); }
    virtual Primitive *GetPrimitiveAtIndex(int i) { return dynamic_cast<Primitive*>(&((*vec)[i])); }
    static PrimitiveVectorBase *Create(int len) { vector<T> *v = new vector<T>(len); return new PrimitiveVector<T>(*v); }
    virtual void FreeContainer() { delete vec; }
};

template<class T>
class PrimitiveArray : public PrimitiveArrayBase
{
  private:
    T *arr;
  public:
    PrimitiveArray<T>(T *a) : PrimitiveArrayBase(), arr(a) { }
    virtual ~PrimitiveArray<T>() { }
    virtual Primitive *GetPrimitiveAtIndex(int i) { return dynamic_cast<Primitive*>(&(arr[i])); }
    static PrimitiveArrayBase *Create(int len) { T *a = new T[len]; return new PrimitiveArray<T>(a); }
    virtual void FreeContainer() { delete[] arr; }
};

// ****************************************************************************
// ----------------------------------------------------------------------------
// Container types for Attribute arrays and vectors; for internal use only
// ----------------------------------------------------------------------------
// ****************************************************************************
class AttributeVectorBase
{
  public:
    AttributeVectorBase() { }
    virtual ~AttributeVectorBase() { }
    virtual int  GetLength() = 0;
    virtual void SetLength(int) = 0;
    virtual Attribute *GetAttributeAtIndex(int) = 0;
    virtual void SetAttributeAtIndex(int,Attribute*) {}
    virtual void EraseAll() { }
    virtual void FreeContainer() = 0;
};

class AttributeArrayBase
{
  public:
    AttributeArrayBase() { }
    virtual ~AttributeArrayBase() { }
    virtual Attribute *GetAttributeAtIndex(int) = 0;
    virtual void SetAttributeAtIndex(int,Attribute*) {}
    virtual void EraseAll(int) { }
    virtual void FreeContainer() = 0;
};

template<class T>
class AttributeObjectVector : public AttributeVectorBase
{
  private:
    vector<T> *vec;
  public:
    AttributeObjectVector<T>(vector<T> &v) : AttributeVectorBase(), vec(&v) { }
    virtual ~AttributeObjectVector<T>() { }
    virtual int GetLength() { return vec->size(); }
    virtual void SetLength(int l) { vec->resize(l); }
    virtual void Clear() { vec->clear(); }
    virtual Attribute *GetAttributeAtIndex(int i) { return dynamic_cast<Attribute*>(&((*vec)[i])); }
    virtual void FreeContainer() { delete vec; }
};

template<class T>
class AttributeObjectArray : public AttributeArrayBase
{
  private:
    T *arr;
  public:
    AttributeObjectArray<T>(T *a) : AttributeArrayBase(), arr(a) { }
    virtual ~AttributeObjectArray<T>() { }
    virtual Attribute *GetAttributeAtIndex(int i) { return dynamic_cast<Attribute*>(&(arr[i])); }
    virtual void FreeContainer() { delete[] arr; }
};

template<class T>
class AttributePointerVector : public AttributeVectorBase
{
  private:
    vector<T> *vec;
  public:
    AttributePointerVector<T>(vector<T> &v) : AttributeVectorBase(), vec(&v) { }
    ~AttributePointerVector<T>() { }
    int GetLength() { return vec->size(); }
    void SetLength(int l) { vec->resize(l, NULL); }
    void EraseAll(int l) { for (int i=0; i<l; i++) if ((*vec)[i]) delete (*vec)[i]; }
    void EraseAll() { EraseAll(vec->size()); vec->clear(); }
    Attribute *GetAttributeAtIndex(int i) { return (*vec)[i]; }
    void SetAttributeAtIndex(int i, Attribute *a) { (*vec)[i] = (T)a; }
    void FreeContainer() { delete vec; }
};

template<class T>
class AttributePointerArray : public AttributeArrayBase
{
  private:
    T **arr;
  public:
    AttributePointerArray<T>(T **a) : AttributeArrayBase(), arr(a) { }
    ~AttributePointerArray<T>() { }
    void EraseAll(int length) { for (int i=0; i<length; i++) if (arr[i]) delete arr[i]; }
    Attribute *GetAttributeAtIndex(int i) { return arr[i]; }
    void SetAttributeAtIndex(int i, Attribute *a) { arr[i] = (T*)a; }
    void FreeContainer() { delete[] arr; }
};


// ****************************************************************************
// ----------------------------------------------------------------------------
//                         inline functions
// ----------------------------------------------------------------------------
// ****************************************************************************
SpecificType RegisterPrimitiveCreators(const std::string n,
                                       PrimCreatorFn pc,
                                       PrimArrCreatorFn pac,
                                       PrimVecCreatorFn pvc);
SpecificType RegisterAttributeCreator(const std::string &n,
                                      AttCreatorFn ac);

// register creator for primitives
template <class T>
SpecificType Primitive::Register()
{
    // This is not type-safe, and...
    PrimCreatorFn pc = reinterpret_cast<PrimCreatorFn>(T::Create);

    SpecificType st = Primitive::mapPrimCreatorToSpecificType[pc];
    if (st == 0)
    {
        st = RegisterPrimitiveCreators(T::GetPrimitiveType(),
                                       pc,
                                       PrimitiveArray<T>::Create,
                                       PrimitiveVector<T>::Create);
        Primitive::mapPrimCreatorToSpecificType[pc] = st;
    }
    return st;
}

// get the primitive specific type without registering it
template <class T>
SpecificType Primitive::GetSpecificType()
{
    // This is not type-safe, and...
    PrimCreatorFn pc = reinterpret_cast<PrimCreatorFn>(T::Create);
    SpecificType st = Primitive::mapPrimCreatorToSpecificType[pc];
    if (st == 0)
        throw Exception("Unregistered primitive %s",
                        T::GetPrimitiveType());
    return st;
}

// register creator for dynamic attributes
template <class T>
SpecificType Attribute::Register()
{
    SpecificType st = Attribute::mapAttCreatorToSpecificType[T::Create];
    if (st == 0)
    {
        T *a = new T;
        st = RegisterAttributeCreator(a->GetType(),
                                      T::Create);
        Attribute::mapAttCreatorToSpecificType[T::Create] = st;
        delete a;
    }
    return st;
}


// attribute obj
template <class T>
void Attribute::Add(const string &n, T &v)
{
    if (T::IsPrimitive())
    {
        SpecificType st = Primitive::GetSpecificType<T>();
        AddField(n, &v, 1, TypePrimitive, st);
    }
    else
        AddField(n, &v, 1, TypeAttributeObj);
}

// attribute obj array
template <class T>
void Attribute::Add(const string &n, T *v, int l)
{
    if (T::IsPrimitive())
    {
        SpecificType st = Primitive::GetSpecificType<T>();
        PrimitiveArray<T> *npa = new PrimitiveArray<T>(v);
        AddField(n, npa, l, TypePrimitiveArray, st);
    }
    else
        AddField(n, new AttributeObjectArray<T>(v), l, TypeAttributeObjArray);
}

// attribute obj vector
template <class T>
void Attribute::Add(const string &n, vector<T> &v)
{
    if (T::IsPrimitive())
    {
        SpecificType st = Primitive::GetSpecificType<T>();
        PrimitiveVector<T> *npv = new PrimitiveVector<T>(v);
        AddField(n, npv, -1, TypePrimitiveVector, st);
    }
    else
        AddField(n, new AttributeObjectVector<T>(v), -1, TypeAttributeObjVector);
}

// attribute ptr
template <class T>
void Attribute::Add(const string &n, T *&v)
{
    SpecificType st = Attribute::Register<T>();
    AddField(n, &v, 1, TypeAttributePtr, st);
}

// attribute ptr array
template <class T>
void Attribute::Add(const string &n, T **v, int l)
{
    SpecificType st = Attribute::Register<T>();
    AddField(n, new AttributePointerArray<T>(v), l, TypeAttributePtrArray, st);
}

// attribute ptr vector
template <class T>
void Attribute::Add(const string &n, vector<T*> &v)
{
    SpecificType st = Attribute::Register<T>();
    AddField(n, new AttributePointerVector<T*>(v), -1, TypeAttributePtrVector, st);
}

// attribute ptr, DYNAMIC ATTRIBUTE POINTER
template <>
inline void Attribute::Add(const string &n, Attribute *&v)
{
    AddField(n, &v, 1, TypeDynamicPtr);
}

// attribute ptr array, DYNAMIC ATTRIBUTE POINTER
template <>
inline void Attribute::Add(const string &n, Attribute **v, int l)
{
    AddField(n, new AttributePointerArray<Attribute>(v), l, TypeDynamicPtrArray);
}

// attribute ptr vector, DYNAMIC ATTRIBUTE POINTER
template <>
inline void Attribute::Add(const string &n, vector<Attribute*> &v)
{
    AddField(n, new AttributePointerVector<Attribute*>(v), -1, TypeDynamicPtrVector);
}
#endif
