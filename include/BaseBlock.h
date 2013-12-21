#ifndef BASEBLOCK_H
#define BASEBLOCK_H

#include <cstdlib>
#include <vector>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <string>
#include "manual_reset_event.h"

using namespace std;

typedef int DataType;

class DataInterface;
class DataPinOut;
class DataPinIn;

#define GetPin(pin,index) (pin[index])

class Data
{
    friend class DataPinOut;
private:
    unsigned char*m_ptr;
    DataPinOut*m_parent;
    int m_refcnt;
    int m_type;
    int m_size;
    std::mutex m_mutex;
    int TypeSizeLookup();

    static int m_alloctime;
    static int m_freetime;
public:
    Data(DataPinOut*parent,int type,int size=0);

    void operator++(int){lock_guard<mutex>lock(m_mutex);m_refcnt++;}
    void operator--(int){lock_guard<mutex>lock(m_mutex);m_refcnt--;}
	operator void*(){ return (void*)Get(); }
	operator unsigned char*(){ return Get(); }
    void Addref(int Count){lock_guard<mutex>lock(m_mutex);m_refcnt+=Count;}
    int& Size(){return m_size;}
    ~Data();
    unsigned char* Get(){return m_ptr;}
    //void Preserve(){m_refcnt++;}
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
    void SetData(Data* data);
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
	void FreeData(){ m_data_dup->Delete(); m_target->m_data = NULL; }
    int Connect(DataPinOut*target);
    Data*& GetData(){std::lock_guard<std::mutex>lock(m_mutex);return m_data_dup;}
protected:
    bool m_valid;
    mutex m_mutex;
    Data* m_data_dup;
    DataPinOut* m_target;
};

typedef initializer_list<string> GateDescription;
typedef vector<DataPinIn*>& INPINS;
typedef vector<DataPinOut*>& OUTPINS;


class BaseBlock
{
    friend class DataPinOut;
    friend class DataPinIn;
    friend void Connect(BaseBlock&Out,int OutPin,BaseBlock&In,int InPin);
    friend void Connect(BaseBlock&Out,string OutPin,BaseBlock&In,string InPin);
    public:
        BaseBlock(GateDescription In,GateDescription Out);
        virtual ~BaseBlock();

        static void Run();
    protected:
        bool m_ready;
        thread* m_thread;

        mutex m_worker_input_mutex;
        mutex m_datamutex;
        condition_variable m_event;

        static mutex m_worker_src_mutex;
        //unique_lock<mutex> m_src_lock;
        static condition_variable m_start_event;
        static int m_src_num;
        static int m_src_ready_num;

        void m_worker();
        int m_valid;
        vector<DataPinIn*> m_in_ports;
        vector<DataPinOut*> m_out_ports;
        virtual int Work(vector<DataPinIn*>&In,vector<DataPinOut*>&Out)=0;
        int Wrapper();
        void Send();
        void DataReady();
        void Ready();
        int FindInPort(const string PortName);
        int FindOutPort(const string PortName);
    private:
};



#endif // BASEBLOCK_H
