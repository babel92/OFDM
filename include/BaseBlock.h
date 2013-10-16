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
class BaseBlock;

class Data
{
private:
    void*m_ptr;
    int m_refcnt;
    int m_type;
    int m_size;
public:
    Data(int size,int type);
    ~Data();
    void Delete();
};


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

class DataPinOut: public DataPin
{
    friend class DataPinIn;
public:
    DataPinOut(DataInterface*interface, int type, DataPin* target):DataPin(interface,type){}
    ~DataPinOut();

    int Connect(DataPinIn*target);
    Data* GetData(){return m_data;}
    void FreeData(){m_data->Delete();};
protected:
    bool Exist(DataPinIn*ptr);
    vector<DataPinIn*> m_target;
    Data* m_data;
};

class DataPinIn: public DataPin
{
    friend class DataPinOut;
public:
    DataPinIn(DataInterface*interface, int type, DataPin* target):DataPin(interface,type),m_target(NULL){}
    ~DataPinIn();

    int Connect(DataPinOut*target);
    Data* GetData(){return m_target->m_data;}
protected:
    DataPinOut* m_target;
};

class DataInterface
{
public:
    void FreeData();
protected:
    BaseBlock* m_parent;
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
        int Wrapper();
    private:
};



#endif // BASEBLOCK_H
