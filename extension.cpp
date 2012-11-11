/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod Sample Extension
 * Copyright (C) 2004-2008 AlliedModders LLC.  All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * Version: $Id$
 */

#include "extension.h"
#include "BZThread.h"

IChangeableForward *g_pAsyncCallback = NULL;
IThreader *g_pThreader = NULL;
//static IThreadHandle *update_thread;

static cell_t BZ2_CompressFile(IPluginContext *pCtx, const cell_t *params)
{
	char *inputFile;
	pCtx->LocalToString(params[1], &inputFile);

	char *outputFile;
	pCtx->LocalToString(params[2], &outputFile);

	int compressionLevel = params[3];
	if(compressionLevel < 1)compressionLevel = 1;
	if(compressionLevel > 9)compressionLevel = 9;

	g_pAsyncCallback->AddFunction(pCtx, static_cast<funcid_t>(params[3]));

	char inputPath[PLATFORM_MAX_PATH];
	smutils->BuildPath(Path_Game, inputPath, sizeof(inputPath), "%s", inputFile);

	char outputPath[PLATFORM_MAX_PATH];
	smutils->BuildPath(Path_Game, outputPath, sizeof(outputPath), "%s", outputFile);
	
	BZThread* myThread = new BZThread(true,compressionLevel,inputPath,inputFile,outputPath,outputFile,pCtx,g_pAsyncCallback, static_cast<funcid_t>(params[4]), params[5]);
    ThreadParams threadparams;
    threadparams.flags = Thread_Default;
    threadparams.prio = ThreadPrio_Low;

	g_pThreader->MakeThread(myThread, &threadparams);
	
	return true;
}

static cell_t BZ2_DecompressFile(IPluginContext *pCtx, const cell_t *params)
{
	char *inputFile;
	pCtx->LocalToString(params[1], &inputFile);

	char *outputFile;
	pCtx->LocalToString(params[2], &outputFile);

	g_pAsyncCallback->AddFunction(pCtx, static_cast<funcid_t>(params[3]));

	char inputPath[PLATFORM_MAX_PATH];
	smutils->BuildPath(Path_Game, inputPath, sizeof(inputPath), "%s", inputFile);
	
	char outputPath[PLATFORM_MAX_PATH];
	smutils->BuildPath(Path_Game, outputPath, sizeof(outputPath), "%s", outputFile);

	BZThread* myThread = new BZThread(false,1,inputPath,inputFile,outputPath,outputFile,pCtx,g_pAsyncCallback, static_cast<funcid_t>(params[3]), params[4]);
    ThreadParams threadparams;
    threadparams.flags = Thread_Default;
    threadparams.prio = ThreadPrio_Low;

	g_pThreader->MakeThread(myThread, &threadparams);	

	return true;
}

bool SMBZ2::SDK_OnLoad(char *error, size_t maxlength, bool late)
{
	sharesys->AddNatives(myself, smbz2_natives);
	sharesys->RegisterLibrary(myself, "bzip2");

	g_pAsyncCallback = forwards->CreateForwardEx(NULL, ET_Ignore, 2, NULL, Param_Cell, Param_String, Param_String, Param_Cell);
	return true;
}

bool SMBZ2::QueryRunning(char* error, size_t maxlength)
{
	SM_CHECK_IFACE(THREADER, g_pThreader);
	return true;
}

void SMBZ2::SDK_OnAllLoaded()
{
	SM_GET_LATE_IFACE(THREADER, g_pThreader);    	
}

void SMBZ2::SDK_OnUnload()
{
}

const sp_nativeinfo_t smbz2_natives[] =
{
	//Documents
	{"BZ2_DecompressFile",	BZ2_DecompressFile},
	{"BZ2_CompressFile",	BZ2_CompressFile},
	{NULL,			NULL},
};

SMBZ2 g_Sample;		/**< Global singleton for extension's main interface */
SMEXT_LINK(&g_Sample);