#include "stdafx.h"
#include "BaseImage.h"


namespace nui
{
    namespace Ui
    {
        BaseImage::BaseImage(NResourceLoader* loader)
        {
            frameIndex_ = 0;
            frameCount_ = 0;
            horzCount_ = 1;
            vertCount_ = 1;
            horzIndex_ = 0;
            vertIndex_ = 0;
            loader_ = loader;
        }

        BaseImage::~BaseImage()
        {
            ;
        }

        INT BaseImage::Release()
        {
            if(RefCount() == 1)
            {
                Destroy();
            }
            return NImage::Release();
        }

        void BaseImage::NextFrame()
        {
            if(frameCount_ == 0)
                return;
            frameIndex_ = (++ frameIndex_) % frameCount_;
        }

        int BaseImage::GetFrameCount() const
        {
            return frameCount_;
        }

        const Base::NSize& BaseImage::GetSize() const
        {
            return size_;
        }

        bool BaseImage::SetCount(int horzCount, int vertCount)
        {
            if(horzCount_ == horzCount && vertCount_ == vertCount)
                return false;

            horzCount_ = horzCount;
            vertCount_ = vertCount;
            return true;
        }

        void BaseImage::GetCount(int& horzCount, int& vertCount) const
        {
            horzCount = horzCount_;
            vertCount = vertCount_;
        }

        bool BaseImage::SetIndex(int horzIndex, int vertIndex)
        {
            if(horzIndex_ == horzIndex && vertIndex_ == vertIndex)
                return false;

            horzIndex_ = horzIndex;
            vertIndex_ = vertIndex;
            return true;
        }

        void BaseImage::GetIndex(int& horzIndex, int& vertIndex) const
        {
            horzIndex = horzIndex_;
            vertIndex = vertIndex_;
        }

        void BaseImage::InitForDynamicImage(int frameCount, const Base::NSize& size)
        {
            horzCount_ = 1;
            vertCount_ = 1;
            horzIndex_ = 0;
            vertIndex_ = 0;
            frameIndex_ = 0;
            frameCount_ = frameCount;
            size_ = size;
        }

        void BaseImage::InitForStaticImage(int horzCount, int vertCount, const Base::NSize& size)
        {
            horzCount_ = horzCount;
            vertCount_ = vertCount;
            horzIndex_ = 0;
            vertIndex_ = 0;
            frameIndex_ = 0;
            frameCount_ = 1;
            size_ = size;
        }

        void BaseImage::Draw(NRender* render, const Base::NRect& rect)
        {
            if(!IsDrawValid())
                return;
            render->DrawImage(this, horzIndex_, vertIndex_, rect, frameIndex_);
        }
    }
}
