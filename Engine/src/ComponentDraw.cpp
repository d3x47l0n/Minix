#include "ComponentDraw.h"

#include "LuaComponentBuilder.h"
#include "Registry.h"
#include "Gfx.h"

namespace Engine
{
    void RectangleDraw2D::RegisterLuaBinding()
    {
        LuaComponentBuilder<RectangleDraw2D>("rectangleDraw2D")
            .Group("draw2D")
            .Group2("rectangleDraw2D")
            .Creator([](Object* object) -> Component*
                {
                    if (auto* c = object->GetComponent<RectangleDraw2D>())
                        return c;

                    Component* created = Registry::CreateComponent<RectangleDraw2D>(
                        object,
                        32, 32,
                        0.0f, 0.0f, 0.0f,
                        WHITE,
                        0
                    );

                    return created;
                })
            .FieldNumber("x", &RectangleDraw2D::GetX, &RectangleDraw2D::SetX)
            .FieldNumber("y", &RectangleDraw2D::GetY, &RectangleDraw2D::SetY)
            .FieldNumber("a", &RectangleDraw2D::GetA, &RectangleDraw2D::SetA)
            .FieldNumber("w", &RectangleDraw2D::GetW, &RectangleDraw2D::SetW)
            .FieldNumber("h", &RectangleDraw2D::GetH, &RectangleDraw2D::SetH)
            .FieldInteger("drawOrder", &RectangleDraw2D::GetDrawOrder, &RectangleDraw2D::SetDrawOrder)
            .FieldColorMember("color", &RectangleDraw2D::color)
            .Register();
    }

    void CircleDraw2D::RegisterLuaBinding()
    {
        LuaComponentBuilder<CircleDraw2D>("circleDraw2D")
            .Group("draw2D")
            .Group2("circleDraw2D")
            .Creator([](Object* object) -> Component*
                {
                    if (auto* c = object->GetComponent<CircleDraw2D>())
                        return c;

                    Component* created = Registry::CreateComponent<CircleDraw2D>(
                        object,
                        16.0f,
                        0.0f, 0.0f,
                        WHITE,
                        0
                    );

                    return created;
                })
            .FieldNumber("x", &CircleDraw2D::GetX, &CircleDraw2D::SetX)
            .FieldNumber("y", &CircleDraw2D::GetY, &CircleDraw2D::SetY)
            .FieldNumber("a", &CircleDraw2D::GetA, &CircleDraw2D::SetA)
            .FieldNumber("r", &CircleDraw2D::GetR, &CircleDraw2D::SetR)
            .FieldInteger("drawOrder", &CircleDraw2D::GetDrawOrder, &CircleDraw2D::SetDrawOrder)
            .FieldColorMember("color", &CircleDraw2D::color)
            .Register();
    }

    void SpriteDraw2D::Draw(Coords2D& pos, float scale)
    {
        (void)scale;

        Texture2D* texture = Gfx::Get(gfx);
        if (!texture) return;

        Rectangle src{
            0.0f,
            0.0f,
            static_cast<float>(texture->width),
            static_cast<float>(texture->height)
        };

        Rectangle dst{
            pos.GetX() + GetOffX(),
            (pos.GetY() + GetOffY()) * (-1.0f),
            w,
            h
        };

        Vector2 origin{
            w / 2.0f,
            h / 2.0f
        };

        DrawTexturePro(*texture, src, dst, origin, pos.GetA() + GetOffA(), tint);
    }

    void SpriteDraw2D::RegisterLuaBinding()
    {
        LuaComponentBuilder<SpriteDraw2D>("spriteDraw2D")
            .Group("draw2D")
            .Group2("spriteDraw2D")
            .Creator([](Object* object) -> Component*
                {
                    if (auto* c = object->GetComponent<SpriteDraw2D>())
                        return c;

                    return Registry::CreateComponent<SpriteDraw2D>(
                        object,
                        0,
                        32.0f, 32.0f,
                        0.0f, 0.0f, 0.0f,
                        WHITE,
                        0
                    );
                })
            .FieldNumber("x", &SpriteDraw2D::GetX, &SpriteDraw2D::SetX)
            .FieldNumber("y", &SpriteDraw2D::GetY, &SpriteDraw2D::SetY)
            .FieldNumber("a", &SpriteDraw2D::GetA, &SpriteDraw2D::SetA)
            .FieldInteger("gfx", &SpriteDraw2D::GetGfx, &SpriteDraw2D::SetGfx)
            .FieldNumber("w", &SpriteDraw2D::GetW, &SpriteDraw2D::SetW)
            .FieldNumber("h", &SpriteDraw2D::GetH, &SpriteDraw2D::SetH)
            .FieldInteger("drawOrder", &SpriteDraw2D::GetDrawOrder, &SpriteDraw2D::SetDrawOrder)
            .FieldColorMember("color", &SpriteDraw2D::tint)
            .Register();
    }
}