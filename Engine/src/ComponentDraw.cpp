#include "ComponentDraw.h"

#include "LuaComponentBuilder.h"
#include "Registry.h"

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
}