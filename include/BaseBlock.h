#ifndef BASEBLOCK_H
#define BASEBLOCK_H

#include <cstdlib>
#include <vector>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <string>

using namespace std;

typedef int DataType;

class DataInterface;
class DataPinOut;
class DataPinIn;

#define GetPin(pin,index) ((*pin)[index])

class Data
{
    friend class DataPinOut;
private:
    void*m_ptr;
    DataPinOut*m_parent;
    int m_refcnt;
    int m_type;
    int m_size;
    int TypeSizeLookup();
public:
    Data(DataPinOut*parent,int type,int size=0);
    int& Size(){return m_size;}
    ~Data();
    unsigned char* Get(){return (unsigned char*)m_ptr;}
    void Delete();
};

class BaseBlock;

class DataPin
{
public:
    DataPin(BaseBlock*parent, string&name, int type);
    int GetType(){return m_type;}
    const string& GetName(){return m_name;}
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
    DataPinOut(BaseBlock*interfac, std::string&name, int type);
    ~DataPinOut();

    int Connect(DataPinIn*target);
    //void SetData(Data* data);
    void Ready();
    Data* GetData(){return m_data;}

    Data* AllocData(int Count);
    void FreeData(){m_data->Delete();};
protected:
    bool Exist(DataPinIn*ptr);
    vector<DataPinIn*> m_target;

    Data* m_data;
};



class DataPinIn: public DataPin
{
    friend class DataPinOut;
    friend class BaseBlock;
public:
    DataPinIn(BaseBlock*interfac, string&name, int type);
    ~DataPinIn();

    bool Available(){return m_valid;}
    void Ready();
    void UnReady(){m_valid=0;}
    void FreeData(){m_data_dup->Delete();}
    int Connect(DataPinOut*target);
    Data* GetData(){return m_data_dup;}
protected:
    bool m_valid;
    Data* m_data_dup;
    DataPinOut* m_target;
};

typedef initializer_list<string> GateDescription;

class BaseBlock
{
    friend class DataPinOut;
    friend class DataPinIn;
    friend void Connect(BaseBlock&Out,int OutPin,BaseBlock&In,int InPin);
    friend void Connect(BaseBlock&Out,string OutPin,BaseBlock&In,string InPin);
    public:
        BaseBlock(GateDescription In,GateDescription Out);
        virtual ~BaseBlock();
        int FindInPort(const string PortName);
        int FindOutPort(const string PortName);
        void Ready();
        void Send();
    protected:
        thread* m_thread;

        mutex m_condmutex;
        mutex m_datamutex;
        std::unique_lock<std::mutex> m_lock;
        condition_variable m_event;

        void m_worker();
        int m_valid;
        vector<DataPinIn*> m_in_ports;
        vector<DataPinOut*> m_out_ports;
        virtual int Work(vector<DataPinIn*>*In,vector<DataPinOut*>*Out)=0;
        int Wrapper();

    private:
};



#endif // BASEBLOCK_H
