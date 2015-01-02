#include "stdafx.h"
#include "../NReflect.h"



namespace NUI
{
    namespace Base
    {
        NReflect::NReflect()
        {
            ;
        }

        NReflect::~NReflect()
        {
            ;
        }

        NReflect& NReflect::GetInstance()
        {
            static NReflect instance;
            return instance;
        }

        bool NReflect::AddReflect(LPCTSTR szNamespace, LPCTSTR szClassName, ObjectCreator objCreator, ReflectFlag flag)
        {
#ifdef _DEBUG
            NAssertError(!IsClassExists(szNamespace, szClassName), TEXT("Class has been added already"));
#endif

            NamespaceInfoMap::iterator iteInfo = m_mapNamespaceInfo.find(szNamespace);
            ClassData classData;
            classData.creator = objCreator;
            classData.flag = flag;
            classData.existObj = NULL;
            if(iteInfo == m_mapNamespaceInfo.end())
            {
                ClassDataMap classDataMap;
                classDataMap.insert(std::make_pair(szClassName, classData));
                m_mapNamespaceInfo.insert(std::make_pair(szNamespace, classDataMap));
            }
            else
            {
                iteInfo->second.insert(std::make_pair(szClassName, classData));
            }
            return true;
        }

        bool NReflect::RemoveReflect(LPCTSTR szNamespace, LPCTSTR szClassName)
        {
            NamespaceInfoMap::iterator iteInfo = m_mapNamespaceInfo.find(szNamespace);
            if(iteInfo == m_mapNamespaceInfo.end())
                return false;

            ClassDataMap::iterator iteCreator = iteInfo->second.find(szClassName);
            if(iteCreator == iteInfo->second.end())
                return false;

            iteInfo->second.erase(iteCreator);
            return true;
        }

        bool NReflect::RemoveAllReflect(LPCTSTR szNamespace)
        {
            NamespaceInfoMap::iterator iteInfo = m_mapNamespaceInfo.find(szNamespace);
            if(iteInfo == m_mapNamespaceInfo.end())
                return false;
            m_mapNamespaceInfo.erase(iteInfo);
            return true;
        }

        bool NReflect::IsClassExists(LPCTSTR szNamespace, LPCTSTR szClassName)
        {
            ClassData* pClassData;
            return GetClassData(szNamespace, szClassName, pClassData);
        }

        NBaseObj* NReflect::Create(LPCTSTR szNamespace, LPCTSTR szClassName, LPCSTR filePath, int line)
        {
            ClassData* pClassData;
            if(!GetClassData(szNamespace, szClassName, pClassData))
                return NULL;
            if(pClassData->flag == None)
                return pClassData->creator(filePath, line);
            if(pClassData->flag == Singleton)
            {
                if(pClassData->existObj == NULL)
                {
                    pClassData->existObj = pClassData->creator(filePath, line);
                }
                if(pClassData->existObj != NULL)
                    pClassData->existObj->AddRef();
                return pClassData->existObj;
            }
            NAssertError(false, _T("Invalid data in NReflect::Create"));
            return NULL;
        }

        void NReflect::ReleaseData()
        {
            NamespaceInfoMap::iterator iteNamespace = m_mapNamespaceInfo.begin();
            for(; iteNamespace != m_mapNamespaceInfo.end(); ++ iteNamespace)
            {
                ClassDataMap::iterator iteClass = iteNamespace->second.begin();
                ClassDataMap::iterator iteClassEnd = iteNamespace->second.end();
                for(; iteClass != iteClassEnd; ++ iteClass)
                {
                    ClassData& data = iteClass->second;
                    if(data.existObj != NULL)
                    {
                        int refCount = data.existObj->Release();
                        NAssertError(refCount == 0, _T("Singleton Object Leak"));
                    }
                }
            }
        }

        bool NReflect::GetClassData(LPCTSTR szNamespace, LPCTSTR szClassName, ClassData*& pClassData)
        {
            NamespaceInfoMap::iterator iteInfo = m_mapNamespaceInfo.find(szNamespace);
            if(iteInfo == m_mapNamespaceInfo.end())
                return false;

            ClassDataMap& creatorMap = iteInfo->second;
            ClassDataMap::iterator iteCreator = creatorMap.find(szClassName);
            if(iteCreator == iteInfo->second.end())
                return false;

            pClassData = &iteCreator->second;
            return true;
        }
    }
}