// AutoTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TestUtil.h"



// Base
#include "TestDataTypes.h"
#include "TestReflect.h"
#include "TestCore.h"

// Data
#include "TestString.h"
#include "TestXmlDataReader.h"
#include "TestModule.h"
#include "TestBuffer.h"
#include "TestFileMapping.h"
#include "TestZip.h"
#include "TestListT.h"
#include "TestArrayT.h"
#include "TestFileSystem.h"

// Util
#include "TestFileUtil.h"

// Ui
#include "TestFrame.h"
#include "TestMsgLoop.h"
#include "TestWindowBase.h"
#include "TestTimerSrv.h"


int _tmain(int argc, _TCHAR* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    NInstPtr<NCore> core(MemToolParam);
    core->DestroyCore();
    return result;
}

