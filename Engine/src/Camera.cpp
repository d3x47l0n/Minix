#include "Camera.h"

#include "TypeCoords.h"

namespace Engine
{
	float Camera::_x = 0.0f;
	float Camera::_y = 0.0f;
	float Camera::_a = 0.0f;
	float Camera::_zoom = 1.0f;

	int Camera::_w = 0;
	int Camera::_h = 0;

	Position2D* Camera::_target = nullptr;

	void Camera::Init(int w, int h)
	{
		_w = w;
		_h = h;

		_x = 0.0f;
		_y = 0.0f;
		_a = 0.0f;
		_zoom = 1.0f;
		_target = nullptr;
	}

	void Camera::Stop()
	{
		_target = nullptr;
	}

	void Camera::SetScreenSize(int w, int h)
	{
		_w = w;
		_h = h;
	}

	bool Camera::TargetExists()
	{
		if (!_target) return false;

		for (auto* pos : Position2D::GetList())
		{
			if (pos == _target)
				return true;
		}

		return false;
	}

	void Camera::Update(float dt)
	{
		(void)dt;

		if (_target)
		{
			if (!TargetExists())
			{
				UnbindTarget();
				return;
			}

			_x = _target->GetX();
			_y = _target->GetY();
		}
	}

	void Camera::SetX(float x)
	{
		_x = x;
	}

	void Camera::SetY(float y)
	{
		_y = y;
	}

	void Camera::SetA(float a)
	{
		_a = a;
	}

	void Camera::SetZoom(float zoom)
	{
		if (zoom < 0.1f) zoom = 0.1f;
		if (zoom > 2.5f) zoom = 2.5f;

		_zoom = zoom;
	}

	float Camera::GetX()
	{
		return _x;
	}

	float Camera::GetY()
	{
		return _y;
	}

	float Camera::GetA()
	{
		return _a;
	}

	float Camera::GetZoom()
	{
		return _zoom;
	}

	void Camera::BindTarget(Position2D* target)
	{
		_target = target;
	}

	void Camera::UnbindTarget()
	{
		_target = nullptr;
	}

	Position2D* Camera::GetTarget()
	{
		return _target;
	}

	Camera2D Camera::GetRaylibCamera()
	{
		Camera2D camera{};

		camera.target = Vector2{
			_x,
			_y * (-1.0f)
		};

		camera.offset = Vector2{
			(float)_w / 2.0f,
			(float)_h / 2.0f
		};

		camera.rotation = -_a;
		camera.zoom = Coords::GetScale() * _zoom;

		return camera;
	}
}