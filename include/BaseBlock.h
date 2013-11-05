#ifndef BASEBLOCK_H
#define BASEBLOCK_H

#include <cstdlib>
#include <vector>
#include <algorithm>
#include <thread>
#include <string>

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
    DataPin(BaseBlock*parent, string&name, int type);
    int GetType(){return m_type;}
    virtual ~DataPin();
protected:
    string m_name;
    BaseBlock* m_parent;
    int m_type;
    //vector<DataPin*> m_target;
};

class DataPinOut: public DataPin
{
    friend class DataPinIn;
public:
    DataPinOut(BaseBlock*interface, string&name, int type):DataPin(interface,name,type){}
    ~DataPinOut();

    int Connect(DataPinIn*target);
    void SetData(Data* data);
    void Ready();
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
    DataPinIn(BaseBlock*interface, string&name, int type);
    ~DataPinIn();

    bool Available(){return m_valid;}
    void Ready();
    void UnReady(){m_valid=0;}
    void FreeData(){m_target->FreeData();}
    int Connect(DataPinOut*target);
    Data* GetData(){return m_target->m_data;}
protected:
    bool m_valid;
    DataPinOut* m_target;
};

class DataInterface
{
public:
    DataInterface(BaseBlock*block);
    void FreeData();
    void Pass();
    void Ready();
protected:
    BaseBlock* m_parent;
    //vector<PinType*> m_pin;
    int m_valid;
};

typedef vector<string> GateDescription;

class BaseBlock
{
    friend class DataPinOut;
    friend class DataPinIn;
    public:
        BaseBlock(GateDescription In,GateDescription Out);
        virtual ~BaseBlock();
    protected:
        thread* m_thread;
        void m_worker();
        int m_valid;
        vector<DataPinIn*> m_in_ports;
        vector<DataPinOut*> m_out_ports;
        virtual int Work(vector<DataPinIn*>*In,vector<DataPinOut*>*Out)=0;
        int Wrapper();
        void Ready();
        virtual void SetupPort()=0;
    private:
};



#endif // BASEBLOCK_H
