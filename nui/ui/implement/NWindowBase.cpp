#include "stdafx.h"
#include "../NWindowBase.h"


#include "WindowDef.h"
#include "WindowUtil.h"
#include "WindowMap.h"

#include "../NMsgLoop.h"

namespace nui
{
    namespace Ui
    {
        NWindowBase::NWindowBase()
        {
            window_ = NULL;
            layered_ = false;
            mouseTracking_ = false;
        }

        NWindowBase::~NWindowBase()
        {
            NAssertError(window_ == NULL, _T("window_ isn't Null in ~WindowBase"));
        }

        void NWindowBase::SetMsgFilterCallback(MsgFilterCallback callback)
        {
            msgFilterCallback_ = callback;
        }

        bool NWindowBase::Create(HWND parentWindow)
        {
            return Create(parentWindow, WindowStyle::Top);
        }

        bool NWindowBase::Create(HWND parentWindow, DWORD styleValue)
        {
            NAssertError(window_ == NULL, _T("Window already exists"));

            bool result = Util::EnsureWindowClass(SKIN_WINDOW_NAME, &NWindowBase::_staticWndProc);
            NAssertError(result, _T("Failed to register class"));
            if(!result)
                return false;

            DWORD style, exStyle;
            GetStyle(styleValue, style, exStyle);

            window_ = ::CreateWindowEx(exStyle,
                SKIN_WINDOW_NAME,
                _T(""),
                style,
                0, 0,
                CW_USEDEFAULT, CW_USEDEFAULT,
                parentWindow,
                NULL,
                nui::Data::NModule::GetInst().GetNUIModule(),
                static_cast<LPVOID>(this));
            NAssertError(window_ != NULL, _T("Failed to create window"));

            if(window_)
            {
                OnCreate();
            }

            return (window_ != NULL);
        }

        bool NWindowBase::DoModal(HWND parentWindow)
        {
            NAssertError(window_ == NULL, _T("window_ isn't Null in DoModal"));
            if(parentWindow != NULL)
            {
                ::EnableWindow(parentWindow, FALSE);
            }

            bool result = Create(parentWindow);
            if(result)
            {
                nui::Ui::NMsgLoop loop;
                result = loop.Loop(window_);
            }

            if(parentWindow != NULL)
            {
                ::EnableWindow(parentWindow, TRUE);
            }
            return result;
        }

        void NWindowBase::Destroy()
        {
            NAssertError(window_ != NULL && ::IsWindow(window_), _T("Invalid window in WindowBase::Destroy"));
            if(window_ != NULL)
            {
                if(::IsWindow(window_))
                    ::DestroyWindow(window_);
                window_ = NULL;
                ::PostThreadMessage(::GetCurrentThreadId(), WM_NULL, 0, 0);
            }
        }

        void NWindowBase::SetVisible(BOOL visible)
        {
            NAssertError(window_ != NULL && ::IsWindow(window_), _T("Invalid window in WindowBase::ShowWindow"));
            if(window_ != NULL)
            {
                if(visible)
                {
                    ::ShowWindow(window_, SW_SHOW);
                    if(::IsIconic(window_) || ::GetForegroundWindow() != window_)
                        ::SwitchToThisWindow(window_, TRUE);
                }
                else
                {
                    ::ShowWindow(window_, SW_HIDE);
                }
            }
        }

        bool NWindowBase::GetRect(nui::Base::NRect& rect)
        {
            NAssertError(window_ != NULL && ::IsWindow(window_), _T("Invalid window in WindowBase::GetRect"));
            if(window_ != NULL)
                return !!::GetWindowRect(window_, rect);
            return false;
        }

        void NWindowBase::SetSize(int width, int height)
        {
            NAssertError(window_ != NULL && ::IsWindow(window_), _T("Invalid window in WindowBase::SetSize"));
            if(window_ != NULL)
                ::SetWindowPos(window_, NULL, 0, 0, width, height, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
        }

        void NWindowBase::CenterWindow(HWND relativeWindow)
        {
            NAssertError(window_ != NULL && ::IsWindow(window_), _T("Invalid window in WindowBase::CenterWindow"));
            if(window_ == NULL)
                return;

            BOOL bDeskWnd = FALSE;
            DWORD dwStyle = ::GetWindowLongPtr(window_, GWL_STYLE);
            // Find correct window
            if(relativeWindow == NULL)
            {
                if (dwStyle & WS_CHILD)
                    relativeWindow = ::GetParent(window_);
                else
                    relativeWindow = ::GetWindow(window_, GW_OWNER);
                if(relativeWindow == NULL)
                {
                    bDeskWnd = TRUE;
                    relativeWindow = ::GetDesktopWindow();
                }
            }

            // Calc correct position
            RECT rcWnd = {0};
            RECT rcWndRelative = {0};
            ::GetWindowRect(window_, &rcWnd);

            if(IsIconic(relativeWindow))
            {
                // 最小化的时候，取Restored的大小
                WINDOWPLACEMENT wndPlacement = {0};
                wndPlacement.length = sizeof(wndPlacement);
                ::GetWindowPlacement(relativeWindow, &wndPlacement);
                memcpy(&rcWndRelative, &wndPlacement.rcNormalPosition, sizeof(rcWndRelative));
            }
            else
            {
                if(bDeskWnd)
                    ::SystemParametersInfo(SPI_GETWORKAREA, 0, (PVOID)&rcWndRelative, FALSE);
                else
                    ::GetWindowRect(relativeWindow, &rcWndRelative);
            }
            int nTop = (rcWndRelative.bottom - rcWndRelative.top) - (rcWnd.bottom - rcWnd.top);
            int nLeft = (rcWndRelative.right - rcWndRelative.left) - (rcWnd.right - rcWnd.left);

            nLeft /= 2;
            nLeft += rcWndRelative.left;
            nTop /= 2;
            nTop += rcWndRelative.top;

            ::SetWindowPos(window_, NULL, nLeft, nTop, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
        }

        void NWindowBase::SetRect(const Base::NRect& rect)
        {
            NAssertError(window_ != NULL && ::IsWindow(window_), _T("Invalid window in WindowBase::SetRect"));
            if(window_ != NULL)
                ::SetWindowPos(window_, NULL, rect.Left, rect.Top, rect.Width(), rect.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
        }

        void NWindowBase::Invalidate()
        {
            NAssertError(window_ != NULL && ::IsWindow(window_), _T("Invalid window in WindowBase::SetRect"));
            if(window_ != NULL)
            {
                if(IsLayered())
                    ::PostMessage(window_, WM_PAINT, 0, 0);
                else
                    ::InvalidateRect(window_, NULL, TRUE);
            }
        }

        void NWindowBase::InvalidateRect(const Base::NRect& rect)
        {
            NAssertError(window_ != NULL && ::IsWindow(window_), _T("Invalid window in WindowBase::SetRect"));
            if(window_ != NULL)
            {
                if(IsLayered())
                    ::PostMessage(window_, WM_PAINT, 0, 0);
                else
                    ::InvalidateRect(window_, rect, TRUE);
            }
        }

        void NWindowBase::SetText(LPCTSTR text)
        {
            NAssertError(window_ != NULL && ::IsWindow(window_), _T("Invalid window in WindowBase::SetText"));
            if(window_ != NULL)
                ::SetWindowText(window_, text);
        }

        HWND NWindowBase::GetNative() const
        {
            return window_;
        }

        LRESULT NWindowBase::DoDefault(UINT message, WPARAM wParam, LPARAM lParam)
        {
            if(window_ == NULL)
                return 0;
            NAssertError(!!::IsWindow(window_), _T("Invalid window in WindowBase::DoDefault"));
            if(window_ != NULL)
                return ::CallWindowProc(::DefWindowProc, window_, message, wParam, lParam);
            return 0;
        }

        LRESULT NWindowBase::_staticWndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
        {
            NWindowBase* pThis = nui::Ui::Util::WindowMap::GetInst().GetWindow(window);
            if(message == WM_NCCREATE)
            {
                CREATESTRUCT *cs = reinterpret_cast<CREATESTRUCT*>(lParam);
                pThis = static_cast<NWindowBase*>(cs->lpCreateParams);
                pThis->window_ = window;
                nui::Ui::Util::WindowMap::GetInst().AddWindow(window, pThis);
            }

            LRESULT lResult = 0;
            if(pThis == NULL)
                lResult = ::CallWindowProc(::DefWindowProc, window, message, wParam, lParam);
            else if(!pThis->OnMessage(message, wParam, lParam, lResult))
                lResult = pThis->DoDefault(message, wParam, lParam);

            if(message == WM_NCDESTROY && pThis != NULL)
            {
                pThis->window_ = NULL;
                nui::Ui::Util::WindowMap::GetInst().RemoveWindow(window);
            }
            return lResult;
        }

        bool NWindowBase::OnMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
        {
            if(!mouseTracking_ && message == WM_MOUSEMOVE)
            {
                TRACKMOUSEEVENT _Event = {0};
                _Event.cbSize = sizeof(_Event);
                _Event.dwFlags = TME_LEAVE;
                _Event.hwndTrack = window_;
                if(::TrackMouseEvent(&_Event))
                {
                    mouseTracking_ = true;
                }
            }
            else if(message == WM_MOUSELEAVE)
            {
                mouseTracking_ = false;
            }

            if(msgFilterCallback_ && msgFilterCallback_(this, message, wParam, lParam, lResult))
                return true;

            lResult = 0;
            return false;
        }

        void NWindowBase::OnCreate()
        {
            layered_ = ((::GetWindowLongPtr(window_, GWL_EXSTYLE) & WS_EX_LAYERED) == WS_EX_LAYERED);
            Invalidate();
        }

        bool NWindowBase::IsLayered() const
        {
            return layered_;
        }

        void NWindowBase::GetStyle(DWORD styleValue, DWORD& style, DWORD& exStyle) const
        {
            style = 0;
            exStyle = 0;
            if(styleValue & WindowStyle::Child)
            {
                style |= WS_CHILD;
            }
            if(styleValue & WindowStyle::Layered)
            {
                exStyle |= WS_EX_LAYERED;
            }
            if(styleValue & WindowStyle::Sizable)
            {
                style |= WS_SIZEBOX | WS_OVERLAPPED;
            }
            if(styleValue & WindowStyle::Transparent)
            {
                exStyle |= WS_EX_TRANSPARENT;
            }
            if((styleValue & WindowStyle::Top) || styleValue == 0)
            {
                style |= WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_OVERLAPPED | WS_MAXIMIZEBOX;
            }
        }
    }
}
