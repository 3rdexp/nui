#pragma once



namespace nui
{
    namespace Ui
    {
        class NUI_INTF NRender;
        class NUI_INTF NDraw : public nui::Base::NBaseObj
        {
        public:
            virtual void Draw(NRender* render, int horzIndex, int vertIndex, const Base::NRect& rect) = 0;
        };
    }
}
