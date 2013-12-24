#ifndef BASEBLOCK_H
#define BASEBLOCK_H

#include <cstdlib>
#include <vector>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <string>
#include <memory>
#include "manual_reset_event.h"

using namespace std;

namespace jsdsp{

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
		//int m_refcnt;
		int m_type;
		int m_size;
		std::mutex m_mutex;
		int TypeSizeLookup();

		static int m_alloctime;
		static int m_freetime;
	public:
		Data(DataPinOut*parent, int type, int size = 0);

		//void operator++(int){lock_guard<mutex>lock(m_mutex);m_refcnt++;}
		//void operator--(int){lock_guard<mutex>lock(m_mutex);m_refcnt--;}
		operator void*(){ return (void*)Get(); }
		operator unsigned char*(){ return Get(); }
		//void Addref(int Count){lock_guard<mutex>lock(m_mutex);m_refcnt+=Count;}
		int& Size(){ return m_size; }
		~Data();
		unsigned char* Get(){ return m_ptr; }
		//void Preserve(){m_refcnt++;}
		//void Delete();
	};

	typedef std::shared_ptr<Data> DataPtr;

	class BaseBlock;

	class DataPin
	{
		friend class BaseBlock;
	public:
		DataPin(BaseBlock*parent, string&name, int type);
		int GetType(){ return m_type; }
		const string& GetName(){ return m_name; }
		virtual ~DataPin();
	protected:
		string m_name;
		BaseBlock* m_parent;
		int m_type;
		//vector<DataPin*> m_target;
	};

	class DataPinOut : public DataPin
	{
		friend class DataPinIn;
		friend class Data;
		friend class BaseBlock;
	public:
		DataPinOut(BaseBlock*interfac, std::string&name, int type);
		~DataPinOut();
		void Ready();
		int Connect(DataPinIn*target);
		void SetData(DataPtr Data);
		DataPtr GetData(){ return m_data; }
		DataPtr DetachData(){ DataPtr ret = m_data; m_data.reset(); return ret; }

		DataPtr AllocData(int Count);
		DataPtr DataPinOut::AllocDataOnly(int Count);
		//void FreeData(){m_data->Delete();};

		void Lock(){ m_dataprotect.lock(); }
		void Unlock(){ m_dataprotect.unlock(); }
	protected:
		bool Exist(DataPinIn*ptr);
		vector<DataPinIn*> m_target;
		std::mutex m_dataprotect;
		std::mutex m_evntmu;
		std::condition_variable m_dataevent;
		DataPtr m_data;
	};



	class DataPinIn : public DataPin
	{
		friend class DataPinOut;
		friend class BaseBlock;
	public:
		DataPinIn(BaseBlock*interfac, string&name, int type);
		~DataPinIn();

		bool Available(){ return m_valid; }
		void Ready();
		void UnReady()
		{
			// ensure atomic
			m_target->Lock();
			m_data_dup.reset();
			// This is causing troubles
			//m_target->m_data.reset();
			m_valid = 0;
			{
				lock_guard<mutex> lock(m_target->m_evntmu);
				m_target->m_dataevent.notify_one();
			}
			m_target->Unlock();
		}
		//void FreeData(){ std::lock_guard<std::mutex>lock(m_mutex); m_data_dup = NULL; m_target->FreeData(); }
		int Connect(DataPinOut*target);
		DataPtr& GetData(){ return m_data_dup; }


		void Lock(){ m_target->m_dataprotect.lock(); }
		void Unlock(){ m_target->m_dataprotect.unlock(); }
	protected:
		bool m_valid;
		mutex m_mutex;
		DataPtr m_data_dup;
		DataPinOut* m_target;
	};

	typedef initializer_list<string> GateDescription;
	typedef vector<DataPinIn*>& INPINS;
	typedef vector<DataPinOut*>& OUTPINS;


	class BaseBlock
	{
		friend class DataPinOut;
		friend class DataPinIn;
		friend void Connect(BaseBlock&Out, int OutPin, BaseBlock&In, int InPin);
		friend void Connect(BaseBlock&Out, string OutPin, BaseBlock&In, string InPin);
	public:
		BaseBlock(GateDescription In, GateDescription Out);
		virtual ~BaseBlock();

		static void Run();
	protected:
		int m_worktime;
		volatile bool m_ready;
		thread* m_thread;

		mutex m_worker_input_mutex;
		mutex m_datamutex;
		condition_variable m_event;
		condition_variable m_dataresetevent;
		unique_lock<mutex>* datalockptr;

		static mutex m_worker_src_mutex;
		//unique_lock<mutex> m_src_lock;
		static condition_variable m_start_event;
		static int m_src_num;
		static int m_src_ready_num;

		void m_worker();
		int m_valid;
		vector<DataPinIn*> m_in_ports;
		vector<DataPinOut*> m_out_ports;
		virtual int Work(vector<DataPinIn*>&In, vector<DataPinOut*>&Out) = 0;
		int Wrapper();
		void Send();
		void DataReady();
		void Ready();
		int FindInPort(const string PortName);
		int FindOutPort(const string PortName);
	private:
	};

}

#endif // BASEBLOCK_H
