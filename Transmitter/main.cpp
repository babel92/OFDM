#include <iostream>
#include <cassert>
#include <thread>
#include <queue>
#include <mutex>
#include <portaudio/portaudio.h>
#include <portaudio/pa_asio.h>
#include <fftw/fftw3.h>
#include <Windows.h>
#define ARMA_USE_CXX11
#include "armadillo"

using namespace arma;
using namespace std;

class playback_device
{
	double M_samplerate;
	double M_framesize;
	PaStream* M_stream;
public:
	playback_device(double SampleRate, double FrameSize)
	{
		M_samplerate = SampleRate;
		M_framesize = FrameSize;
		PaError err = Pa_Initialize();
		if (paNoError != err)
		{
			cerr << "PortAudio error: " << Pa_GetErrorText(err) << endl;
			exit(1);
		}
		err = Pa_OpenDefaultStream(&M_stream,
			0,          /* input channels */
			1,          /* stereo output */
			paFloat32,
			SampleRate,
			FrameSize,
			NULL,
			NULL);
	}

	~playback_device()
	{
		Pa_StopStream(M_stream);
		Pa_CloseStream(M_stream);
	}

	void start()
	{
		Pa_StartStream(M_stream);
	}

	void step(const mat& Array)
	{
		Pa_WriteStream(M_stream, conv_to<fmat>::from(Array).memptr(), Array.n_elem);
	}

	void step(const cx_mat& Array)
	{
		mat send = join_rows(real(Array), imag(Array));
		Pa_WriteStream(M_stream, conv_to<fmat>::from(send).memptr(), send.n_elem);
	}
};

cx_mat QPSKModulate(const mat& BitArray)
{
	const double amp = 0.70710678;
	if (BitArray.n_elem % 2)
		throw;
	cx_mat ret(1, BitArray.n_cols / 2);
	for (int i = 0; i < ret.n_elem; ++i)
	{
		if (BitArray[2 * i] == 0 && BitArray[2 * i + 1] == 0)
		{
			ret[i] = { -amp, -amp };
		}
		else if (BitArray[2 * i] == 0 && BitArray[2 * i + 1] == 1)
		{
			ret[i] = { -amp, amp };
		}
		else if (BitArray[2 * i] == 1 && BitArray[2 * i + 1] == 0)
		{
			ret[i] = { amp, -amp };
		}
		else
		{
			ret[i] = { amp, amp };
		}
	}
	return ret;
}

const mat idle_pattern1 = 
{ 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 
1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1 };

const mat idle_pattern2 =
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

const mat data_header =
{ 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1 };

static const double SAMPLE_RATE = 44100/4.;
static const double FRAME_SIZE = 512;
static const double QPSK_CARRIER_FREQ = SAMPLE_RATE / 4;
static const double QPSK_DEMOD_CO = 2 * 3.1415927 * QPSK_CARRIER_FREQ / SAMPLE_RATE;
static const int SAMPLE_PER_SYMBOL = 32;

bool quit = 0;

mat range(int a, int b, int step = 0)
{
	if (step == 0)
		step = b > a ? 1 : -1;
	mat ret(1, floor((b - a) / (double)step) + 1);
	for (int i = 0; i < ret.n_elem; ++i)
	{
		ret[i] = a + i * step;
	}
	return ret;
}

inline mat char2dbin(char ch)
{
	mat ret(1, 8);
	for (int i = 0; i < 8; ++i)
		ret[i] = (double)((ch&(1 << 7 - i))>0);
	return ret;
}

mat str2dbin(const char* str)
{
	int len = strlen(str);
	mat ret(1, len * 8);
	for (int i = 0; i < len; ++i)
		ret.cols(i * 8, i * 8 + 7) = char2dbin(str[i]);
	return ret;
}

mat short2dbin(unsigned short num)
{
	char* ptr = (char*)&num;
	// Little-endian
	return join_rows(char2dbin(ptr[1]), char2dbin(ptr[0]));
}

mat wrap_data(const mat& data)
{
	return join_rows(data_header, join_rows(short2dbin(data.n_elem), data));
}

mat zero_padding(const mat& data)
{
	return join_rows(zeros(1, 32 - (data.n_elem % 32)), data);
}

template <typename T>
T rep_row(const T& data, int time)
{
	if (time < 1)
		throw;
	T ret = data;
	for (int i = 0; i < time - 1; ++i)
	{
		ret = join_rows(data, ret);
	}
	return ret;
}

cx_mat fftw(const cx_mat& data)
{
	cx_mat ret(data.n_rows, data.n_cols);
	fftw_plan p;
	p = fftw_plan_dft_1d(data.n_elem, 
		(fftw_complex*)data.memptr(), (fftw_complex*)ret.memptr(), 
		FFTW_BACKWARD, FFTW_ESTIMATE);
	fftw_execute(p);
	return ret;
}

void info()
{
	cout << "OFDM Transmitter\n"
		<< "Sample rate: " << SAMPLE_RATE << endl
		<< "Carrier freq: " << QPSK_CARRIER_FREQ << endl;
}

class
{
private:
	queue<mat> M_sendqueue;
	mutex M_mutex;
public:
	void feed(const char* str)
	{
		lock_guard<mutex> lock(M_mutex);
		
		mat wrapped_data = zero_padding(wrap_data(str2dbin(str)));
		assert(wrapped_data.n_elem % 32 == 0);
		int line = wrapped_data.n_elem / 32;
		for (int i = 0; i < line; ++i)
		{
			M_sendqueue.push(wrapped_data.cols(32 * i, 32 * i + 31));
		}
	}

	mat step()
	{
		lock_guard<mutex> lock(M_mutex);
		if (M_sendqueue.size() > 0)
		{
			mat ret = M_sendqueue.front();
			M_sendqueue.pop();
			return ret;
		}
		else
			return idle_pattern1;
	}
}signal_source;

void input_thread()
{
	char buffer[128];
	cout << "Waiting for input\n";
	for (;;)
	{
		cout << "say> ";
		if (cin.getline(buffer, 127).fail())
			// Ctrl+C
			break;
		else if (strlen(buffer) == 0)
			continue;
		signal_source.feed(buffer);
	}
}

unsigned counter = 0;

BOOL CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
	case CTRL_C_EVENT:
		cout << "\nStopping...\n";
		quit = 1;
		return(TRUE);

	default:
		return FALSE;
	}
}


int main()
{
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
	playback_device audio(SAMPLE_RATE, FRAME_SIZE);
	system("cls");
	info();
	thread input(input_thread);

	cx_mat mod_idle_pattern = QPSKModulate(idle_pattern2);
	cx_mat mod_pattern256 = rep_row(mod_idle_pattern, 16);
	//cout << mod_pattern256;
	cx_mat ofdm_data = ifft(mod_pattern256);

	mat sync_padding(1, 512);
	sync_padding.cols(0, 1) = ones(1, 2);
	sync_padding.cols(255, 256) = ones(1, 2);
	audio.start();
	audio.step(sync_padding);
	while (!quit)
	{
#if 1
		mat send_data = signal_source.step();
		cx_mat mod_data = QPSKModulate(send_data);
		mat mulArray = range(counter, counter + SAMPLE_PER_SYMBOL*mod_data.n_elem - 1);
		mulArray *= QPSK_DEMOD_CO;
		mat I = cos(mulArray);
		mat Q = sin(mulArray);
		for (int i = 0; i < mod_data.n_elem; ++i)
		{
			span index(i*SAMPLE_PER_SYMBOL, (1 + i)*SAMPLE_PER_SYMBOL - 1);
			I.cols(index) = I.cols(index) * mod_data[i].real() + Q.cols(index) * mod_data[i].imag();
		}
		counter += SAMPLE_PER_SYMBOL*mod_data.n_elem;
		audio.step(I);
#else
		audio.step(ofdm_data);

#endif
	}
	input.join();

	return 0;
}