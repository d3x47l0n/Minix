#pragma once

#include "raylib.h"
#include "TypeComponent.h"

namespace Engine
{
    class Draw2D : public ComponentBase<Draw2D>
    {
    public:
        float offx{0.0f};
        float offy{0.0f};
        float offa{0.0f};
        int drawOrder{0};

        virtual ~Draw2D() = default;
        virtual void Draw(float x, float y, float a) = 0;
    };

    class Draw2DRectangle : public Draw2D
    {
    public:
        int w;
        int h;
        Color color;

        explicit Draw2DRectangle(int w_, int h_, float offx_, float offy_, float offa_, Color color_, int drawOrder_)
            : w(w_), h(h_), color(color_)
        {
            this->offx = offx_;
            this->offy = offy_;
            this->offa = offa_;
            this->drawOrder = drawOrder_;
        }

        void Draw(float x, float y, float a) override
        {
            Rectangle rec{
                x + static_cast<float>(w) / 2.0f + offx,
                y + static_cast<float>(h) / 2.0f + offy,
                static_cast<float>(w),
                static_cast<float>(h)
            };

            Vector2 origin{
                static_cast<float>(w) / 2.0f,
                static_cast<float>(h) / 2.0f
            };

            DrawRectanglePro(rec, origin, a+offa, color);
        }
    };

    class Draw2DCircle : public Draw2D
    {
    public:
        float r;
        Color color;

        explicit Draw2DCircle(float r_, float offx_, float offy_, Color color_, int drawOrder_)
            : r(r_), color(color_)
        {
            this->offx = offx_;
            this->offy = offy_;
            this->drawOrder = drawOrder_;
        }

        void Draw(float x, float y, float a) override
        {
            DrawCircle(
                static_cast<int>(x + offx),
                static_cast<int>(y + offy),
                r,
                color
            );
        }
    };
}