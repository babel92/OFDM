#include "BaseBlock.h"

Data::Data(int size, int type)
:m_refcnt(0)
,m_type(type)
,m_size(size)
{
    m_ptr=new char[size];
}

Data::~Data()
{
    delete [] (char*)m_ptr;
}

void Data::Delete()
{
    if(m_refcnt>1)
    {
        m_refcnt--;
    }
    else
        delete this;
}

/****************************************************
 *
 ****************************************************/

DataPin::DataPin(BaseBlock*interface, int type)
:m_parent(interface)
,m_type(type)
{

}

DataPin::~DataPin()
{

}

/****************************************************
 *
 ****************************************************/

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
    for(vector<DataPinIn*>::iterator it=m_target.begin();it<m_target.end();++it)
    {
        (*it)->Ready();
    }
}

void DataPinOut::SetData(Data* data)
{
    m_data=data;
    Ready();
}


/****************************************************
 *
 ****************************************************/

DataPinIn::DataPinIn(BaseBlock*interface, int type, DataPin* target)
:DataPin(interface,type)
,m_valid(0)
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
{
    if(!m_valid)
    {
        m_valid=1;
        m_parent->Ready();
    }
}

/****************************************************
 *
 ****************************************************/
/*
DataInterface::DataInterface(BaseBlock*block)
:m_parent(block)
,m_valid(0)
{

}

void DataInterface::FreeData()
{
    for(vector<PinType*>::iterator it=m_pin.begin();it<m_pin.end();++it)
    {
        (*it)->FreeData();
    }
}

void DataInterface::Ready()
{
    if(m_valid<(int)m_pin.size()-1)
        m_valid++;
    else
        m_parent->Wrapper();
}
*/
/****************************************************
 *
 ****************************************************/

BaseBlock::BaseBlock()
:m_valid(0)
{
    //ctor

}

BaseBlock::~BaseBlock()
{
    //dtor
}

int BaseBlock::Wrapper()
{

    //test the return value
    //mem allocation for out ports should be done in Work()
    Work(&m_in_ports,&m_out_ports);


    //clean up memory by checking ref count
    for(vector<DataPinIn*>::iterator it=m_in_ports.begin();it<m_in_ports.end();++it)
        (*it)->FreeData();
    return 0;
}

void BaseBlock::Ready()
{
    if(m_valid<(int)m_in_ports.size()-1)
        m_valid++;
    else
    {
        m_valid=0;
        Wrapper();
    }
}
