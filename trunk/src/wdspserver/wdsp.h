#ifndef myexport
#define myexport


typedef unsigned int BOOLEAN;
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

//extern "C" {

void OpenChannel (int channel, int in_size, int dsp_size, int input_samplerate, int dsp_rate, int output_samplerate, int type, int state, double tdelayup, double tslewup, double tdelaydown, double tslewdown);

void CloseChannel (int channel);

void SetType (int channel, int type);

void SetInputBuffsize (int channel, int in_size);

void SetDSPBuffsize (int channel, int dsp_size);

void SetInputSamplerate  (int channel, int samplerate);

void SetDSPSamplerate (int channel, int samplerate);

void SetOutputSamplerate (int channel, int samplerate);

void SetAllRates (int channel, int in_rate, int dsp_rate, int out_rate);

void SetChannelState (int channel, int state, int dmode);

typedef enum _sdrmode
{
  LSB,  //  0
  USB,  //  1
  DSB,  //  2
  CWL,  //  3
  CWU,  //  4
  FM,   //  5
  AM,   //  6
  DIGU, //  7
  SPEC, //  8
  DIGL, //  9
  SAM,  // 10
  DRM   // 11
} SDRMODE;

// RXA
enum rxaMode
{
	RXA_LSB,
	RXA_USB,
	RXA_DSB,
	RXA_CWL,
	RXA_CWU,
	RXA_FM,
	RXA_AM,
	RXA_DIGU,
	RXA_SPEC,
	RXA_DIGL,
	RXA_SAM,
	RXA_DRM
};

enum MeterType
{
    SIGNAL_STRENGTH = 0,
    AVG_SIGNAL_STRENGTH,
    ADC_REAL,
    ADC_IMAG,
    AGC_GAIN,
    MIC,
    PWR,
    ALC,
    EQ,
    LEVELER,
    COMP,
    CPDR,
    ALC_G,
    LVL_G,
    MIC_PK,
    ALC_PK,
    EQ_PK,
    LEVELER_PK,
    COMP_PK,
    CPDR_PK,
};

enum rxaMeterType
{
	RXA_S_PK,
	RXA_S_AV,
	RXA_ADC_PK,
	RXA_ADC_AV,
	RXA_AGC_GAIN,
	RXA_AGC_PK,
	RXA_AGC_AV,
	RXA_METERTYPE_LAST
};

enum AGCMode
{
FIRST = -1,
FIXD,
LONG,
SLOW,
MED,
FAST,
CUSTOM,
LAST,
};

void SetRXAMode (int channel, int mode);

double GetRXAMeter (int channel, int mt);


// TXA
enum txaMode
{
	TXA_LSB,
	TXA_USB,
	TXA_DSB,
	TXA_CWL,
	TXA_CWU,
	TXA_FM,
	TXA_AM,
	TXA_DIGU,
	TXA_SPEC,
	TXA_DIGL,
	TXA_SAM,
	TXA_DRM
};

enum txaMeterType
{
	TXA_MIC_PK,
	TXA_MIC_AV,
	TXA_EQ_PK,
	TXA_EQ_AV,
	TXA_LVLR_PK,
	TXA_LVLR_AV,
	TXA_LVLR_GAIN,
	TXA_COMP_PK,
	TXA_COMP_AV,
	TXA_ALC_PK,
	TXA_ALC_AV,
	TXA_ALC_GAIN,
	TXA_OUT_PK,
	TXA_OUT_AV,
	TXA_METERTYPE_LAST
};


void SetTXAMode (int channel, int mode);

double GetTXAMeter (int channel, int mt);

// bandpass
void SetRXABandpassRun (int channel, int run);

void SetRXABandpassFreqs (int channel, double low, double high);

// TXA Prototypes

void SetTXABandpassRun (int channel, int run);

void SetTXABandpassFreqs (int channel, double low, double high);



void fexchange0 (int channel, double* in, double* out, int* error);

void fexchange2 (int channel, float *Iin, float *Qin, float *Iout, float *Qout, int* error);




void XCreateAnalyzer (int disp,
	int *success,   //writes '0' to success if all went well, <0 if mem alloc failed
	int m_size,     //maximum fft size to be used
	int m_LO,       //maximum number of LO positions per subspan
	int m_stitch,   //maximum number of subspans to be concatenated
	char *app_data_path
	);

void DestroyAnalyzer(int disp);


void SetAnalyzer(int disp,
	int n_fft,      //number of LO frequencies = number of ffts used in elimination
	int typ,//0 for real input data (I only); 1 for complex input data (I & Q)
	int *flp,       //vector with one elt for each LO frequency, 1 if high-side LO, 0 otherwise 
	int sz, //size of the fft, i.e., number of input samples
	int bf_sz,      //number of samples transferred for each OpenBuffer()/CloseBuffer()
	int win_type,   //integer specifying which window function to use
	double pi,      //PiAlpha parameter for Kaiser window
	int ovrlp,      //number of samples each fft (other than the first) is to re-use from the previous 
	int clp,//number of fft output bins to be clipped from EACH side of each sub-span
	int fscLin,     //number of bins to clip from low end of entire span
	int fscHin,     //number of bins to clip from high end of entire span
	int n_pix,      //number of pixel values to return.  may be either <= or > number of bins 
	int n_stch,     //number of sub-spans to concatenate to form a complete span 
	int av_m,       //averaging mode
	int n_av,       //number of spans to (moving) average for pixel result 
	double av_b,    //back multiplier for weighted averaging
	int calset,     //identifier of which set of calibration data to use 
	double fmin,    //frequency at first pixel value 
	double fmax,    //frequency at last pixel value
	int max_w       //max samples to hold in input ring buffers
	);


void Spectrum(int disp, int ss, int LO, float* i,float* q);

void Spectrum2(int disp, int ss, int LO, double* pbuff);

void GetPixels  (int disp,
	float *pix,  //if new pixel values avail, copies to pix and sets flag = 1
	int *flag       //else, returns 0 (try again later)
	);

void GetNAPixels  (int disp,
	float *pix,  //if new pixel values avail, copies to pix and sets flag = 1
	int *flag       //else, returns 0 (try again later)
	);


void create_divEXT(int id, int run, int nr, int size);
void destroy_divEXT(int id);
void create_eerEXT(int id, int run, int size, int rate, double mgain, double pgain, int rundelays, double mdelay, double pdelay, int amiq);
void WDSPwisdom(char* directory);
void destroy_eerEXT(int id);
void SetRXAShiftFreq(int channel, double freq);
void SetChannelState(int channel, int state, int dmode);
void SetRXAAMSQThreshold(int channel, double threshold);
void SetRXAAMSQRun(int channel, BOOLEAN run);
void SetRXAAGCFixed(int channel, double fixed_agc);
void SetRXAAGCTop(int channel, double max_agc);
void SetRXAPanelGain1(int channel, double gain);
void SetRXASpectrum(int channel, int flag, int disp, int ss, int LO);
void TXAGetSpecF1(int channel, float* results);
void SetRXAPanelPan(int channel, double value);

void SetRXAAGCMode(int channel, int mode);
void GetRXAAGCTop(int channel, double* max_agc);
void SetRXAAGCAttack(int channel, int attack);
void SetRXAAGCDecay(int channel, int decay);
void SetRXAAGCHang(int channel, int hang);
void SetRXAAGCSlope(int channel, int slope);
void GetRXAAGCHangThreshold(int channel, int* hangthreshold);
void SetRXAAGCHangThreshold(int channel, int hangthreshold);
void GetRXAAGCThresh(int channel, double* thresh, double size, double rate);
void SetRXAAGCThresh(int channel, double thresh, double size, double rate);
void GetRXAAGCHangLevel(int channel, double* hanglevel);
void SetRXAAGCHangLevel(int channel, double hanglevel);
void SetTXAALCDecay(int channel, int decay);
void SetRXAAMDSBMode(int channel, int sbmode);
void SetRXAAMDFadeLevel(int channel, int fadelevel);
void SetTXAAMSQRun(int channel, BOOLEAN run);
void SetTXAAMSQMutedGain(int channel, double dBlevel);
void SetTXAAMSQThreshold(int channel, double threshold);
void SetTXAAMCarrierLevel(int channel, double carrier);
void SetRXAANFRun(int channel, BOOLEAN run);
void SetRXAANFVals(int channel, int taps, int delay, double gain, double leak);
void SetRXAANFTaps(int channel, int taps);
void SetRXAANFDelay(int channel, int delay);
void SetRXAANFGain(int channel, double gain);
void SetRXAANFLeakage(int channel, double leakage);
void SetRXAANFPosition(int channel, int position);
void SetRXAANRRun(int channel, BOOLEAN run);
void SetRXAANRVals(int channel, int taps, int delay, double gain, double leak);
void SetRXAANRTaps(int channel, int taps);
void SetRXAANRDelay(int channel, int delay);
void SetRXAANRGain(int channel, double gain);
void SetRXAANRLeakage(int channel, double leakage);
void SetRXAANRPosition(int channel, int position);
void SetRXABandpassWindow(int channel, int wintype);
void SetTXABandpassWindow(int channel, int wintype);
void SetRXACBLRun(int channel, BOOLEAN run);
void SetTXACompressorRun(int channel, BOOLEAN run);
void SetTXACompressorGain(int channel, double gain);
void SetRXAEQRun(int channel, BOOLEAN run);
void SetTXAEQRun(int channel, BOOLEAN run);
void SetRXAGrphEQ(int channel, int* ptr);
void SetTXAGrphEQ(int channel, int* ptr);
void SetRXAGrphEQ10(int channel, int* ptr);
void SetTXAGrphEQ10(int channel, int* ptr);
void SetRXAFMDeviation(int channel, double deviation);
void SetRXAFMSQRun(int channel, BOOLEAN run);
void SetRXAFMSQThreshold(int channel, double threshold);
void SetTXAFMDeviation(int channel, double deviation);
void SetTXAFMEmphPosition(int channel, BOOLEAN position);
void SetTXACTCSSRun(int channel, BOOLEAN run);
void SetTXACTCSSFreq(int channel, double freq_hz);
void SetRXACTCSSRun(int channel, BOOLEAN run);
void SetRXACTCSSFreq(int channel, double freq_hz);
void SetTXALevelerTop(int channel, double maxgain);
void SetTXALevelerDecay(int channel, int decay);
void SetTXALevelerSt(int channel, BOOLEAN state);
void SetRXAPanelRun(int channel, BOOLEAN run);
void SetRXAPanelSelect(int channel, int select);
void SetRXAPanelBinaural(int channel, BOOLEAN bin);
void SetTXAPanelRun(int channel, BOOLEAN run);
void RXAGetaSipF(int channel, float* results, int size);
void RXAGetaSipF1(int channel, float* results, int size);
void TXAGetaSipF(int channel, float* results, int size);
void TXAGetaSipF1(int channel, float* results, int size);
void* create_resampleFV(int in_rate, int out_rate);
void xresampleFV(float* input, float* output, int numsamps, int* outsamps, void* ptr);
void destroy_resampleFV(void* ptr);
void SetRXAPreGenRun(int channel, int run);
void SetRXAPreGenMode(int channel, int mode);
void SetRXAPreGenToneMag(int channel, double mag);
void SetRXAPreGenToneFreq(int channel, double freq);
void SetRXAPreGenNoiseMag(int channel, double mag);
void SetRXAPreGenSweepMag(int channel, double mag);
void SetRXAPreGenSweepFreq(int channel, double freq1, double freq2);
void SetRXAPreGenSweepRate(int channel, double rate);
void SetTXAPreGenRun(int channel, int run);
void SetTXAPreGenMode(int channel, int mode);
void SetTXAPreGenToneMag(int channel, double mag);
void SetTXAPreGenToneFreq(int channel, double freq);
void SetTXAPreGenNoiseMag(int channel, double mag);
void SetTXAPreGenSweepMag(int channel, double mag);
void SetTXAPreGenSweepFreq(int channel, double freq1, double freq2);
void SetTXAPreGenSweepRate(int channel, double rate);
void SetTXAPreGenSawtoothMag(int channel, double mag);
void SetTXAPreGenSawtoothFreq(int channel, double freq);
void SetTXAPreGenTriangleMag(int channel, double mag);
void SetTXAPreGenTriangleFreq(int channel, double freq);
void SetTXAPreGenPulseMag(int channel, double mag);
void SetTXAPreGenPulseFreq(int channel, double freq);
void SetTXAPreGenPulseDutyCycle(int channel, double dc);
void SetTXAPreGenPulseToneFreq(int channel, double freq);
void SetTXAPreGenPulseTransition(int channel, double transtime);
void SetTXAPostGenRun(int channel, int run);
void SetTXAPostGenMode(int channel, int mode);
void SetTXAPostGenToneFreq(int channel, double freq);
void SetTXAPostGenToneMag(int channel, double mag);
void SetTXAPostGenTTMag(int channel, double mag1, double mag2);
void SetTXAPostGenTTFreq(int channel, double freq1, double freq2);
void SetTXAPostGenSweepMag(int channel, double mag);
void SetTXAPostGenSweepFreq(int channel, double freq1, double freq2);
void SetTXAPostGenSweepRate(int channel, double rate);

// diversity

void SetEXTDIVRun(int id, int run);
void SetEXTDIVNr (int id, int nr);
void SetEXTDIVOutput (int id, int output);
void SetEXTDIVRotate(int id, int nr, double* Irotate, double* Qrotate);

// eer

void xeerEXTF(int id, float* inI, float* inQ, float* outI, float* outQ, float* outM);
void SetEERRun(int id, BOOLEAN run);
void SetEERAMIQ(int id, BOOLEAN amiq);
void SetEERMgain (int id, double gain);
void SetEERPgain(int id, double gain);
void SetEERRunDelays(int id, BOOLEAN run);
void SetEERMdelay(int id, double delay);
void SetEERPdelay(int id, double delay);
void SetEERSize(int id, int size);
void SetEERSamplerate(int id, int rate);

// apf

void SetRXASPCWRun(int channel, BOOLEAN run);
void SetRXASPCWFreq(int channel, double freq);
void SetRXASPCWBandwidth(int channel, double bw);
void SetRXASPCWGain(int channel, double gain);

// dolly filter

void SetRXAmpeakRun(int channel, BOOLEAN run);
void SetRXAmpeakFilFreq(int channel, int fil, double freq);
void SetRXAmpeakFilBw(int channel, int fil, double bw);
void SetRXAmpeakFilGain(int channel, int fil, double gain);

// static PD

void SetTXAStaticPDRun(int channel, BOOLEAN run);
void SetTXAStaticPDGain(int channel, double* coefs);

void create_nobEXT(
    int id,
    int run,
    int mode,
    int buffsize,
    double samplerate,
    double tau,
    double hangtime,
    double advtime,
    double backtau,
    double threshold
    );
void destroy_nobEXT(int id);
void xnobEXTF(int id, float* I, float* Q);
void SetEXTNOBBuffsize(int id, int size);
void SetEXTNOBSamplerate(int id, int rate);
void SetEXTNOBTau(int id, double tau);
void SetEXTNOBHangtime(int id, double time);
void SetEXTNOBAdvtime(int id, double time);
void SetEXTNOBBacktau(int id, double tau);
void SetEXTNOBThreshold(int id, double thresh);
void SetEXTNOBMode(int id, int mode);
void create_anbEXT(
    int id,
    int run,
    int buffsize,
    double samplerate,
    double tau,
    double hangtime,
    double advtime,
    double backtau,
    double threshold
    );
void destroy_anbEXT(int id);
void xanbEXTF(int id, float* I, float* Q);
void SetEXTANBBuffsize(int id, int size);
void SetEXTANBSamplerate(int id, int rate);
void SetEXTANBTau(int id, double tau);
void SetEXTANBHangtime(int id, double time);
void SetEXTANBAdvtime(int id, double time);
void SetEXTANBBacktau(int id, double tau);
void SetEXTANBThreshold(int id, double thresh);

// PureSignal

void GetPSInfo(int channel, int* info);
void SetPSReset(int channel, int reset);
void SetPSMancal(int channel, int mancal);
void SetPSAutomode(int channel, int automode);
void SetPSTurnon(int channel, int turnon);
void SetPSControl(int channel, int reset, int mancal, int automode, int turnon);
void SetPSLoopDelay(int channel, double delay);
void SetPSMoxDelay(int channel, double delay);
double SetPSTXDelay(int channel, double delay);
void psccF(int channel, int size, float* Itxbuff, float* Qtxbuff, float* Irxbuff, float* Qrxbuff, BOOLEAN mox, BOOLEAN solidmox);
void PSSaveCorr(int channel, char* filename);
void PSRestoreCorr(int channel, char* filename);
void SetPSHWPeak(int channel, double peak);
void GetPSHWPeak(int channel, double* peak);
void GetPSMaxTX(int channel, double* maxtx);
void SetPSPtol(int channel, double ptol);
void GetPSDisp(int channel, int* x, int* ym, int* yc, int* ys, int* cm, int* cc, int* cs);

//}
#endif
