#pragma once



#include "../base/BaseObj.h"
#include "NImage.h"
#include "NShape.h"
#include "NText.h"
#include "NFont.h"

namespace nui
{
    namespace Ui
    {
        class NUI_INTF NResourceLoader : public Base::NBaseObj
        {
        public:
            virtual NImage* LoadImage(LPCTSTR filePath) = 0;
            virtual NShape* CreateShape(LPCSTR filePath, int line) = 0;
            virtual NText* CreateText(LPCTSTR text, LPCSTR filePath, int line) = 0;
            virtual NFont* CreateFont(int fontSize, LPCSTR filePath, int line) = 0;
        };
    }
}
