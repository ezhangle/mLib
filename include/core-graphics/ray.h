#pragma once

#ifndef _RAY_H_
#define _RAY_H_


template<class FloatType>
class Triangle;

template<class FloatType>
class Ray
{
public:



	Ray(const point3d<FloatType> &o, const point3d<FloatType> &d)
	{
		m_Origin = o;
		m_Direction = d;
		m_InverseDirection = point3d<FloatType>(1.0/d.x, 1.0/d.y, 1.0/d.z);

		t = u = v = std::numeric_limits<FloatType>::max();

		m_Sign.x = (m_InverseDirection.x < 0);
		m_Sign.y = (m_InverseDirection.y < 0);
		m_Sign.z = (m_InverseDirection.z < 0);
	}


	~Ray(void)
	{

	}

	bool hasHit() const
	{
		return t != std::numeric_limits<FloatType>::max() && t != -std::numeric_limits<FloatType>::max();
	}

	point3d<FloatType> getHitPoint() const {
		if (hasHit()) {
			return m_Origin + t * m_Direction;
		} else {
			FloatType notvalid = std::numeric_limits<FloatType>::quiet_NaN();
			return point3d<FloatType>(notvalid, notvalid, notvalid);
		}
	}

	const point3d<FloatType>& origin() const
	{
		return m_Origin;
	}

	const point3d<FloatType>& direction() const
	{
		return m_Direction;
	}

	const point3d<FloatType>& inverseDirection() const
	{
		return m_InverseDirection;
	}

	const vec3i& sign() const
	{
		return m_Sign;
	}

	FloatType t;
	FloatType u,v;
	const Triangle<FloatType>* tri;
private:
	point3d<FloatType> m_Direction;
	point3d<FloatType> m_InverseDirection;
	point3d<FloatType> m_Origin;

	vec3i m_Sign;
};

typedef Ray<float> Rayf;
typedef Ray<double> Rayd;

#endif