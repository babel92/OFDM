#ifndef BASEBLOCK_H
#define BASEBLOCK_H

#include <cstdlib>
#include <vector>
#include <algorithm>

using namespace std;

typedef int DataType;

class DataInterface;
class DataPinOut;
class DataPinIn;

class DataPin
{
public:
    DataPin(DataInterface*interface, int type);
    int GetType(){return m_type;}
    virtual ~DataPin();
protected:
    DataInterface* m_parent;
    int m_type;
    //vector<DataPin*> m_target;
};



class DataPinIn: public DataPin
{
    friend class DataPinOut;
public:
    DataPinIn(DataInterface*interface, int type, DataPin* target):DataPin(interface,type),m_target(NULL){}
    ~DataPinIn();

    int Connect(DataPinOut*target);
protected:
    DataPin* m_target;
};

class DataPinOut: public DataPin
{
    friend class DataPinIn;
public:
    DataPinOut(DataInterface*interface, int type, DataPin* target):DataPin(interface,type){}
    ~DataPinOut();

    int Connect(DataPinIn*target);
protected:
    bool Exist(DataPinIn*ptr);
    vector<DataPin*> m_target;
};

class DataInterface
{
public:

protected:
    vector<DataPin*> m_pin;
};

class BaseBlock
{
    public:
        BaseBlock();
        virtual ~BaseBlock();
    protected:
        DataInterface m_in_ports;
        DataInterface m_out_ports;
        virtual int Work(DataInterface*In,DataInterface*Out)=0;
    private:
};

#endif // BASEBLOCK_H
