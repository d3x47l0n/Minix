#pragma once

#include <vector>
#include <algorithm>

#include "raylib.h"
#include "TypeComponent.h"
#include "TypeCoords.h"

namespace Engine
{
    class Draw2D : public ComponentBase<Draw2D>, public Coords2D
    {
    public:
        int drawOrder{ 0 };

        explicit Draw2D(float offx, float offy, float offa, int drawOrder)
            : Coords2D(offx, offy, offa)
        {
            this->drawOrder = drawOrder;
            AddToDrawList();
            MarkDirty();
        }

        virtual ~Draw2D() = default;

        float GetOffX() { return GetX(); }
        float GetOffY() { return GetY(); }
        float GetOffA() { return GetA(); }

        int GetDrawOrder()
        {
            return this->drawOrder;
        }

        void SetDrawOrder(int drawOrder)
        {
            this->drawOrder = drawOrder;
            MarkDirty();
        }

        virtual void Draw(Coords2D& pos, float scale) = 0;

        void RemoveFromList() override
        {
            ComponentBase<Draw2D>::RemoveFromList();

            auto it = std::find(_drawList.begin(), _drawList.end(), this);
            if (it != _drawList.end())
                _drawList.erase(it);

            MarkDirty();
        }

        static void MarkDirty()
        {
            _drawListDirty = true;
        }

        static void SortIfNeeded()
        {
            if (!_drawListDirty) return;

            std::sort(_drawList.begin(), _drawList.end(),
                [](Draw2D* a, Draw2D* b)
                {
                    return a->drawOrder < b->drawOrder;
                });

            _drawListDirty = false;
        }

        static const std::vector<Draw2D*>& GetSortedList()
        {
            SortIfNeeded();
            return _drawList;
        }

    protected:
        void AddToDrawList()
        {
            _drawList.push_back(this);
        }

        inline static std::vector<Draw2D*> _drawList;
        inline static bool _drawListDirty = true;
    };

    class RectangleDraw2D : public Draw2D
    {
    public:
        float w;
        float h;
        Color color;

        explicit RectangleDraw2D(int w, int h, float offx, float offy, float offa, Color color, int drawOrder)
            : Draw2D(offx, offy, offa, drawOrder), w(w), h(h), color(color)
        {
        }

        void SetH(float h)
        {
            this->h = h;
        }

        float GetH()
        {
            return h;
        }

        void SetW(float w)
        {
            this->w = w;
        }

        float GetW()
        {
            return this->w;
        }

        void Draw(Coords2D& pos, float scale) override
        {
            (void)scale;

            Rectangle rec{
                pos.GetX() + GetOffX(),
                (pos.GetY() + GetOffY()) * (-1.0f),
                w,
                h
            };

            Vector2 origin{
                w / 2.0f,
                h / 2.0f
            };

            DrawRectanglePro(rec, origin, pos.GetA() + GetOffA(), color);
        }

        static void RegisterLuaBinding();
    };

    class CircleDraw2D : public Draw2D
    {
    public:
        float r;
        Color color;

        explicit CircleDraw2D(float r, float offx, float offy, Color color, int drawOrder)
            : Draw2D(offx, offy, 0.0f, drawOrder), r(r), color(color)
        {
        }

        void Draw(Coords2D& pos, float scale) override
        {
            (void)scale;

            DrawCircleV(
                Vector2{
                    pos.GetX() + GetOffX(),
                    (pos.GetY() + GetOffY()) * (-1.0f)
                },
                r,
                color
            );
        }

        void SetR(float r)
        {
            this->r = r;
        }

        float GetR()
        {
            return this->r;
        }

        static void RegisterLuaBinding();
    };
}