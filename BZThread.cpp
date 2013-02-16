#include "BZThread.h"
#include "bzip2/bzlib.h"
#include <string.h>

BZThread::~BZThread()
{
}

BZThread::BZThread(bool compress, int iCompLevel, char inputPath[], char inputFile[], char outputPath[], char outputFile[], IPluginContext *pCtx, funcid_t funcid, int32_t data):IThread()
{	
	bCompress = compress;
	iCompressionLevel = iCompLevel;	
	strcpy(sInputPath,inputPath);
	strcpy(sOutputPath,outputPath);
	strcpy(sOutputFile,outputFile);
	strcpy(sInputFile,inputFile);
	g_pCtx = pCtx;
	myfunc = funcid;
	anyData = data;
}

void BZThread::RunThread(IThreadHandle* pHandle)
{
	int		bzerror;
	bool	bIOOpened = true;
	FILE*	pFileIn;
	FILE*	pFileOut;

	if(bCompress) {
		// Compress
		pFileIn = fopen(sInputPath, "rb");
		pFileOut = fopen(sOutputPath, "wb");	
	
		int		nWritten = 0;
		if ( pFileIn == NULL) {
			bIOOpened = false;
			bzerror = BZ_IO_ERROR_INPUT;
		}

		if ( pFileOut == NULL) {
			bIOOpened = false;
			bzerror = BZ_IO_ERROR_OUTPUT;
		}

		if ( bIOOpened )
		{
			BZFILE* b;
			int     nBuf;
			char	buf[512];
		
			b = BZ2_bzWriteOpen(&bzerror, pFileOut, iCompressionLevel, 0, 30);

			if(bzerror == BZ_OK) {				
				while(true) {
					if(feof(pFileIn))break;
					nBuf = fread(buf, 1, sizeof(buf), pFileIn);
					if(nBuf > 0)BZ2_bzWrite(&bzerror, b, (void*)buf,nBuf);
					if(bzerror != BZ_OK) {						
						break;
					}
					nWritten += nBuf;
				}
			}
			BZ2_bzWriteClose ( &bzerror, b, 0, NULL, NULL);
		} 		
	} else {
		// Decompress
		pFileIn = fopen(sInputPath, "rb");
		pFileOut = fopen(sOutputPath, "wb");	
		
		if ( pFileIn == NULL) {
			bIOOpened = false;
			bzerror = BZ_IO_ERROR_INPUT;
		}

		if ( pFileOut == NULL) {
			bIOOpened = false;
			bzerror = BZ_IO_ERROR_OUTPUT;
		}

		if ( bIOOpened )
		{
			BZFILE* b;
			int     nBuf;
			char	buf[8192];			

			b = BZ2_bzReadOpen( &bzerror, pFileIn, 0, 0, NULL, 0);
			if(bzerror == BZ_OK) {				
				while ( bzerror == BZ_OK ) {
					nBuf = BZ2_bzRead ( &bzerror, b, buf, sizeof(buf));
					BZ2_bzerror(b, &bzerror);				
					if ( bzerror == BZ_OK ) {
						fwrite(buf, nBuf, 1, pFileOut);				
					}
				}
			}
			BZ2_bzReadClose ( &bzerror, b );
		} 
	}

	if( pFileIn != NULL) fclose ( pFileIn );
	if( pFileOut != NULL) fclose ( pFileOut );

	BZIPCallbackInfo *info = new BZIPCallbackInfo;
	info->g_pCtx = g_pCtx;
	info->myfunc = myfunc;
	info->bzerror = bzerror;
	info->sInputPath.assign(sInputPath);
	info->sOutputFile.assign(sOutputFile);
	info->anyData = anyData;
	
	smutils->AddFrameAction(SMBZ2::CallCallbackFinish, info);
}

void BZThread::OnTerminate(IThreadHandle* pHandle, bool cancel)
{
}
