#pragma once


#include "../base/BaseObj.h"

namespace nui
{
    namespace Data
    {
        enum DataReaderType
        {
            ReaderUnknown,
            ReaderJson,
            ReaderXml,
        };

        class NUI_CLASS NDataReader : public nui::Base::NBaseObj
        {
        public:
            NDataReader(void){}
            virtual ~NDataReader(void){}

            virtual bool ParseUtf8(const char* data, size_t length) = 0;

            virtual bool ReadValue(LPCTSTR valueName, nui::Base::NString& value) = 0;

            virtual bool ReadPath(LPCTSTR path, LPCTSTR valueName, nui::Base::NString& value) = 0;
            virtual bool ReadNode(LPCTSTR nodeName, NDataReader*& value) = 0;
            virtual bool ReadNodeByPath(LPCTSTR path, NDataReader*& value) = 0;

            virtual bool ReadPath(size_t index, LPCTSTR path, LPCTSTR valueName, nui::Base::NString& value) = 0;
            virtual bool ReadNode(size_t index, LPCTSTR nodeName, NDataReader*& value) = 0;

            virtual bool ParseUtf8(const char* data);

            virtual bool ReadValue(LPCTSTR name, INT32& value);
            virtual bool ReadValue(LPCTSTR name, DWORD& value);
            virtual bool ReadValue(LPCTSTR name, INT64& value);
            virtual bool ReadValue(LPCTSTR name, bool& value);
            virtual bool ReadValue(LPCTSTR name, nui::Base::NPoint& value);
            virtual bool ReadValue(LPCTSTR name, nui::Base::NSize& value);
            virtual bool ReadValue(LPCTSTR name, nui::Base::NRect& value);
        };

        NUI_API NDataReader* CreateDataReader(DataReaderType type);

    }
}