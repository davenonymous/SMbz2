#include <IThreader.h>
#include <IExtensionSys.h>
#include <IForwardSys.h>
#include <sm_globals.h>

#define	BZ_IO_ERROR_INPUT	(-101)
#define	BZ_IO_ERROR_OUTPUT	(-102)

class BZThread : public IThread
{
private:
	int iCompressionLevel;
	bool bCompress;
	char sInputPath[PLATFORM_MAX_PATH];
	char sInputFile[PLATFORM_MAX_PATH];
	char sOutputPath[PLATFORM_MAX_PATH];
	char sOutputFile[PLATFORM_MAX_PATH];
	IPluginContext *g_pCtx;
	IChangeableForward *g_pAsyncCallback;
	funcid_t myfunc;
public: //IThread
	void RunThread(IThreadHandle *pThread);
	void OnTerminate(IThreadHandle *pThread, bool cancel);
public:
	BZThread(bool compress, int iCompLevel, char inputPath[], char inputFile[], char outputPath[], char outputFile[], IPluginContext *pCtx, IChangeableForward *pAsyncCallback, funcid_t);
	~BZThread();
};