#include "BaseBlock.h"
#include <algorithm>
#include <iostream>
#include <chrono>
#include <thread>

struct TypeEntry
{
    string TypeName;
    int Size;
}TypeList[]=
{
    {"int",4},
    {"float",4},
    {"double",4},
    {"byte",1},
    {"short",2},
    {"char",1},
    {"any",1}
};

int TypeListSize=sizeof(TypeList)/sizeof(TypeEntry);

inline int Data::TypeSizeLookup()
{
    return TypeList[m_type].Size;
}

int Data::m_alloctime=0;
int Data::m_freetime=0;

Data::Data(DataPinOut*parent, int type, int size)
:m_ptr(NULL)
,m_parent(parent)
,m_refcnt(0)
,m_type(type)
,m_size(size)
{
    if(size>0)
        m_ptr=new unsigned char[TypeSizeLookup()*size];
    m_alloctime++;
}

Data::~Data()
{
    delete [] m_ptr;
    m_freetime++;
}

void Data::Delete()
{
    m_mutex.lock();

    if(m_refcnt>0)
    {
        m_refcnt--;
    }
    m_mutex.unlock();
    if(m_refcnt<=0)
    {
        delete this;
    }
}

/****************************************************
 *
 ****************************************************/

void Connect(BaseBlock&Out,string OutPin,BaseBlock&In,string InPin)
{
    Connect(Out,Out.FindOutPort(OutPin),In,In.FindInPort(InPin));
}

void Connect(BaseBlock&Out,int OutPin,BaseBlock&In,int InPin)
{
     Out.m_out_ports[OutPin]->Connect(In.m_in_ports[InPin]);
}

/****************************************************
 *
 ****************************************************/

DataPin::DataPin(BaseBlock*parent, string&name, int type)
:m_name(name)
,m_parent(parent)
,m_type(type)
{

}

DataPin::~DataPin()
{

}

/****************************************************
 *
 ****************************************************/

DataPinOut::DataPinOut(BaseBlock*parent, string&name, int type)
:DataPin(parent, name, type), m_data(NULL)
{

}

DataPinOut::~DataPinOut()
{

}

Data* DataPinOut::AllocData(int Count)
{
    // we don't need to worry about leak here
    m_data=new Data(this,m_type,Count);

    return m_data;
}

int DataPinOut::Connect(DataPinIn*target)
{
    if(Exist(target)||target->m_target==this)
    {
        return -1;
    }
    target->m_target=this;
    m_target.push_back(target);
    return m_target.size() - 1;
}

inline bool DataPinOut::Exist(DataPinIn*target)
{
    return find(m_target.begin(),m_target.end(),target)!=m_target.end();
}

void DataPinOut::Ready()
{
	if (!m_data)
		return;

    m_data->Addref(m_target.size());

    for(vector<DataPinIn*>::iterator it=m_target.begin();it<m_target.end();++it)
    {
        DataPinIn*dbg=*it;
        // if one target haven't done, just ignore it
        //if((*it)->m_data_dup!=NULL)
/*
        if(!(*it)->m_parent->m_condmutex.try_lock())
        {
            m_data->Delete();
            continue;
        }
*/
        // to make sure no packet is missed
        //(*it)->m_parent->m_condmutex.lock();
        // we might need to duplicate the data pointer to prevent race condition

        /** TODO: fix the fuck of this!!!*/
        while((*it)->GetData()!=NULL);
        (*it)->GetData()=m_data;
        //(*it)->m_parent->m_condmutex.unlock();
        (*it)->Ready();
    }
}
/*
void DataPinOut::SetData(Data* data)
{
    m_data=data;
    Ready();
}
*/

/****************************************************
 *
 ****************************************************/

DataPinIn::DataPinIn(BaseBlock*parent, string&name, int type)
:DataPin(parent,name,type)
,m_valid(0)
,m_data_dup(NULL)
,m_target(NULL)
{

}

DataPinIn::~DataPinIn()
{

}

int DataPinIn::Connect(DataPinOut*target)
{
    if(NULL!=m_target||target->Exist(this))
    {
        return -1;
    }
    m_target=target;
    target->m_target.push_back(this);
    return target->m_target.size() - 1;
}


void DataPinIn::Ready()
{/*
    if(!m_valid)
    {
        m_valid=1;
        m_parent->Ready();
    }*/
    m_parent->DataReady();
}

/****************************************************
 *
 ****************************************************/


mutex BaseBlock::m_worker_src_mutex;
//unique_lock<mutex>*BaseBlock::m_src_lock=NULL;
condition_variable BaseBlock::m_start_event;
int BaseBlock::m_src_num=0;
int BaseBlock::m_src_ready_num=0;

void BaseBlock::m_worker()
{
    // wait for derived ctor
	unique_lock<mutex> worker_input_lock(m_worker_input_mutex);
	//m_worker_src_mutex=new mutex;
	//m_src_lock=new unique_lock<mutex>(*m_worker_src_mutex);

    while(!m_ready);
    if(m_in_ports.size()==0)
    {
        {
            unique_lock<mutex> src_lock(m_worker_src_mutex);
            m_src_ready_num++;
            //cout<<"a src is entering wait status\n";
            m_start_event.wait(src_lock);
            //cout<<"a src finished waiting\n";
        }
        //should not return
        Work(*((vector<DataPinIn*>*)NULL),m_out_ports);
    }
    else
    {

        for(;;)
        {
            // wait for input notification
            // if any input pin doesn't have data, just wait
            //cout<<"a block is entering wait status\n";
            m_event.wait(worker_input_lock,[this](){
                for(DataPinIn* ptr:m_in_ports)
                    if(ptr->GetData()==NULL)
                        return false;
                return true;
            });
            //cout<<"a block finished waiting\n";
            // do work
            Work(m_in_ports,m_out_ports);

            // this might need a lock, we'll see
            for(DataPinIn* ptr:m_in_ports)
            {
                // reduce the data refcnt before we lose it
                ptr->FreeData();
                ptr->UnReady();
                ptr->GetData()=NULL;
            }

            for(DataPinOut* out:m_out_ports)
                out->Ready();
        }
    }
}

int BaseBlock::FindInPort(const string PortName)
{
    for(int i=0;i<m_in_ports.size();++i)
        if(PortName==m_in_ports[i]->GetName())
            return i;
    return -1;
}

int BaseBlock::FindOutPort(const string PortName)
{
    for(int i=0;i<m_out_ports.size();++i)
        if(PortName==m_out_ports[i]->GetName())
            return i;
    return -1;
}

int TypeLookup(const string& str)
{
    for(int i=0;i<TypeListSize;++i)
    {
        if(TypeList[i].TypeName==str)
            return i;
    }
    return -1;
}

void GateParser(string gate,int&type,string&name)
{
    int space=gate.find(' ');
    type=TypeLookup(gate.substr(0,space));
    name=gate.substr(space+1,gate.length()-space-1);
}

BaseBlock::BaseBlock(GateDescription In,GateDescription Out)
:
//,m_src_lock(*m_worker_src_mutex)
m_valid(0)
{
    //ctor
    int type;
    string name;
    m_ready=0;
    bool is_src=1;
    for(string InPorts:In)
    {
        is_src=0;
        GateParser(InPorts,type,name);
        m_in_ports.push_back(new DataPinIn(this,name,type));
    }
    if(is_src)
        m_src_num++;
    for(string OutPorts:Out)
    {
        GateParser(OutPorts,type,name);
        m_out_ports.push_back(new DataPinOut(this,name,type));
    }

    m_thread=new std::thread(&BaseBlock::m_worker,this);
    //m_thread->detach();
}

BaseBlock::~BaseBlock()
{
    //dtor
    for(auto InPin:m_in_ports)
    {
        delete InPin;
    }
    for(auto OutPin:m_out_ports)
    {
        delete OutPin;
    }
    delete m_thread;
    exit(0);
}

void BaseBlock::Send()
{
    for(DataPinOut* out:m_out_ports)
        out->Ready();
}

int BaseBlock::Wrapper()
{

    //test the return value
    //mem allocation for out ports should be done in Work()
    Work(m_in_ports,m_out_ports);


    //clean up memory by checking ref count
    for(auto it:m_in_ports)
        it->FreeData();
    return 0;
}

void BaseBlock::DataReady()
{
    /*
    if(m_valid<(int)m_in_ports.size()-1)
        m_valid++;
    else
    {
        m_valid=0;
        Wrapper();
    }*/
    m_ready=1;
    {
        unique_lock<mutex> lock(m_worker_input_mutex);
    }
    m_event.notify_all();
}

void BaseBlock::Ready()
{
    /*
    if(m_valid<(int)m_in_ports.size()-1)
        m_valid++;
    else
    {
        m_valid=0;
        Wrapper();
    }*/
    m_ready=1;
    m_event.notify_all();
}

void BaseBlock::Run()
{
    std::chrono::milliseconds dura( 2000 );

    //wait until all source blocks are ready
    while(m_src_ready_num<m_src_num);

    {
        unique_lock<mutex> lock(m_worker_src_mutex);
    }
    cout<<"Ready...\n";
    m_start_event.notify_all();


    while(1)
        std::this_thread::sleep_for( dura );;
}
