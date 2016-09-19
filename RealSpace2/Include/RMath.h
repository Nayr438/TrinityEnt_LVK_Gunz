#pragma once

#include "RTypes.h"
#include "MUtil.h"
#include "MDebug.h"
#include <cassert>

#ifndef TOLER
#define TOLER 0.001
#endif
#define IS_ZERO(a) ((fabs((double)(a)) < (double) TOLER))
#define IS_EQ(a,b) ((fabs((double)(a)-(b)) >= (double) TOLER) ? 0 : 1)
#define IS_EQ3(a,b) (IS_EQ((a).x,(b).x)&&IS_EQ((a).y,(b).y)&&IS_EQ((a).z,(b).z))
#define SIGNOF(a) ( (a)<-TOLER ? NEGATIVE : (a)>TOLER ? POSITIVE : ZERO )
#define RANDOMFLOAT ((float)rand()/(float)RAND_MAX)

#define FLOAT2RGB24(r, g, b) ( ( ((long)((r) * 255)) << 16) | (((long)((g) * 255)) << 8) | (long)((b) * 255))
#define VECTOR2RGB24(v)		FLOAT2RGB24((v).x,(v).y,(v).z)
#define BYTE2RGB24(r,g,b)	((DWORD) (((BYTE) (b)|((WORD) (g) << 8))|(((DWORD) (BYTE) (r)) << 16)))
#define BYTE2RGB32(a,r,g,b)	((DWORD) (((BYTE) (b)|((WORD) (g) << 8))|(((DWORD) (BYTE) (r)) << 16)|(((DWORD) (BYTE) (a)) << 24)))
#define DWORD2VECTOR(x)		rvector(float(((x)& 0xff0000) >> 16)/255.f, float(((x) & 0xff00) >> 8)/255.f,float(((x) & 0xff))/255.f)

_NAMESPACE_REALSPACE2_BEGIN

inline rvector GetPoint(const rboundingbox& bb, int i) {
	return rvector{ (i & 1) ? bb.vmin.x : bb.vmax.x,
		(i & 2) ? bb.vmin.y : bb.vmax.y,
		(i & 4) ? bb.vmin.z : bb.vmax.z };
}

namespace detail
{
	template <typename T1, typename T2>
	constexpr auto&& max_(const T1& a, const T2& b) {
		return a > b ? a : b;
	}
	template <typename T1, typename T2>
	constexpr auto&& min_(const T1& a, const T2& b) {
		return a > b ? b : a;
	}
}

// Returns the bounding box that contains both a and b.
inline rboundingbox Union(const rboundingbox& a, const rboundingbox& b)
{
	rboundingbox ret;
	auto set = [&](auto member, auto fn) {
		for (size_t i = 0; i < 3; i++)
			(ret.*member)[i] = fn((a.*member)[i], (b.*member)[i]);
	};
	set(&rboundingbox::vmax, detail::max_<float, float>);
	set(&rboundingbox::vmin, detail::min_<float, float>);
	return ret;
}

inline rboundingbox Union(const rboundingbox& a, const rvector& b)
{
	rboundingbox ret;
	auto set = [&](auto member, auto fn) {
		for (size_t i = 0; i < 3; i++)
			(ret.*member)[i] = fn((a.*member)[i], b[i]);
	};
	set(&rboundingbox::vmax, detail::max_<float, float>);
	set(&rboundingbox::vmin, detail::min_<float, float>);;
	return ret;
}

inline rboundingbox Union(const v3& a, const v3& b)
{
	rboundingbox ret;
	auto set = [&](auto member, auto fn) {
		for (size_t i = 0; i < 3; i++)
			(ret.*member)[i] = fn(a[i], b[i]);
	};
	set(&rboundingbox::vmax, detail::max_<float, float>);
	set(&rboundingbox::vmin, detail::min_<float, float>);
	return ret;
}

inline bool Intersects(const rboundingbox& a, const rboundingbox &b)
{
	return a.vmin.x < b.vmax.x &&
		a.vmax.x > b.vmin.x &&
		a.vmin.y < b.vmax.y &&
		a.vmax.y > b.vmin.y &&
		a.vmin.z < b.vmax.z &&
		a.vmax.z > b.vmin.z;
}

inline float Magnitude(const rvector &x) {
	return sqrt(x.x * x.x + x.y * x.y + x.z * x.z);
}
inline float MagnitudeSq(const rvector &x) {
	return x.x * x.x + x.y * x.y + x.z * x.z;
}
// If the input vector has a nonzero length, this function normalizes it in place.
// Otherwise, it does nothing.
inline void Normalize(rvector &x) {
	auto MagSq = MagnitudeSq(x);
	if (MagSq == 0)
		return;
	x *= 1.f / sqrt(MagSq);
}
inline float DotProduct(const rvector& a, const rvector& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}
// Returns a dot (b.x, b.y, b.z, 1).
inline float DotProduct(const rplane& a, const rvector& b) {
	return a.a * b.x + a.b * b.y + a.c * b.z + a.d;
}
// Returns a dot (b.x, b.y, b.z, 0).
inline float DotPlaneNormal(const rplane& a, const rvector& b) {
	return a.a * b.x + a.b * b.y + a.c * b.z;
}
inline v3 CrossProduct(const v3& u, const v3& v) {
	return{ u.y * v.z - u.z * v.y,
		u.z * v.x - u.x * v.z,
		u.x * v.y - u.y * v.x };
}
inline void CrossProduct(rvector *result, const rvector &a, const rvector &b) {
	*result = CrossProduct(a, b);
}

void MakeWorldMatrix(rmatrix *pOut, const rvector& pos, rvector dir, rvector up);

float GetDistance(const rvector &position, const rvector &line1, const rvector &line2);
rvector GetNearestPoint(const rvector &position, const rvector &a, const rvector &b);
float GetDistanceLineSegment(const rvector &position, const rvector &a, const rvector &b);
float GetDistanceBetweenLineSegment(const rvector &a, const rvector &aa, const rvector &c,
	const rvector &cc, rvector *ap, rvector *cp);
float GetDistance(const rvector &position, const rplane &plane);
rvector GetNearestPoint(const rvector &a, const rvector &aa, const rplane &plane);
float GetDistance(const rvector &a, const rvector &aa, const rplane &plane);
float GetDistance(const rboundingbox *bb, const rplane *plane);
void GetDistanceMinMax(rboundingbox &bb, rplane &plane, float *MinDist, float *MaxDist);
float GetDistance(const rboundingbox &bb, const rvector &point);
float GetArea(rvector &v1, rvector &v2, rvector &v3);

// Returns the clockwise rotation of ta such that tb aligns with ta on xy
float GetAngleOfVectors(const rvector &ta, const rvector &tb);

// Returns from spherically interpolated towards to by t.
// t should be in the range (0, 1).
inline rvector Slerp(const rvector &from, const rvector &to, float t)
{
	float angle = acos(DotProduct(from, to));
	float sin1 = sin((1 - t) * angle);
	float sin2 = sin(t * angle);
	float sinangle = sin(angle);

	return sin1 / sinangle * from + sin2 / sinangle * to;
}

template <typename T>
T Lerp(T src, T dest, float t)
{
	return src * (1 - t) + dest * t;
}

inline v3 HadamardProduct(const v3& a, const v3& b)
{
	return{ a.x * b.x, a.y * b.y, a.z * b.z };
}

bool IsIntersect(rboundingbox *bb1, rboundingbox *bb2);
bool isInPlane(const rboundingbox *bb, const rplane *plane);
bool IsInSphere(const rboundingbox &bb, const rvector &point, float radius);
bool isInViewFrustum(const rvector &point, rplane *plane);
bool isInViewFrustum(const rvector &point, float radius, rplane *plane);
bool isInViewFrustum(const rboundingbox *bb, const rplane *plane);
bool isInViewFrustum(const rvector &point1, const rvector &point2, rplane *planes);
bool isInViewFrustumWithZ(rboundingbox *bb, rplane *plane);
bool isInViewFrustumwrtnPlanes(rboundingbox *bb, rplane *plane, int nplane);

bool IsIntersect(const rvector& orig, const rvector& dir,
	rvector& v0, rvector& v1, rvector& v2, float* t);
bool isLineIntersectBoundingBox(rvector &origin, rvector &dir, rboundingbox &bb);
bool IsIntersect(rvector& line_begin_, rvector& line_end_, rboundingbox& box_);
bool IsIntersect(rvector& line_begin_, rvector& line_dir_, rvector& center_, float radius_,
	float* dist = nullptr, rvector* p = nullptr);
bool IsIntersect(const rvector& orig, const rvector& dir, const rvector& center,
	const float radius, rvector* p = nullptr);
bool GetIntersectionOfTwoPlanes(rvector *pOutDir, rvector *pOutAPoint, rplane &plane1, rplane &plane2);

void MergeBoundingBox(rboundingbox *dest, rboundingbox *src);
void TransformBox(rboundingbox* result, const rboundingbox& src, const rmatrix& matrix);

inline rvector GetReflectionVector(const rvector& v, const rvector& n)
{
	auto neg = -v;
	float dot = DotProduct(neg, n);

	return (2 * dot) * n + v;
}

// Intel's reference 4x4 matrix inversion code
// from ftp://download.intel.com/design/pentiumiii/sml/24504301.pdf
inline void Inverse(rmatrix& result, const rmatrix& mat)
{
	auto* src = static_cast<const float*>(mat);
	auto* dest = static_cast<float*>(result);

	__m128 minor0, minor1, minor2, minor3;
	__m128 row0, row1{}, row2, row3{};
	__m128 det, tmp1{};

	tmp1 = _mm_loadh_pi(_mm_loadl_pi(tmp1, (__m64*)(src)), (__m64*)(src + 4));
	row1 = _mm_loadh_pi(_mm_loadl_pi(row1, (__m64*)(src + 8)), (__m64*)(src + 12));
	row0 = _mm_shuffle_ps(tmp1, row1, 0x88);
	row1 = _mm_shuffle_ps(row1, tmp1, 0xDD);
	tmp1 = _mm_loadh_pi(_mm_loadl_pi(tmp1, (__m64*)(src + 2)), (__m64*)(src + 6));
	row3 = _mm_loadh_pi(_mm_loadl_pi(row3, (__m64*)(src + 10)), (__m64*)(src + 14));
	row2 = _mm_shuffle_ps(tmp1, row3, 0x88);
	row3 = _mm_shuffle_ps(row3, tmp1, 0xDD);

	tmp1 = _mm_mul_ps(row2, row3);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	minor0 = _mm_mul_ps(row1, tmp1);
	minor1 = _mm_mul_ps(row0, tmp1);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor0 = _mm_sub_ps(_mm_mul_ps(row1, tmp1), minor0);
	minor1 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor1);
	minor1 = _mm_shuffle_ps(minor1, minor1, 0x4E);

	tmp1 = _mm_mul_ps(row1, row2);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	minor0 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor0);
	minor3 = _mm_mul_ps(row0, tmp1);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row3, tmp1));
	minor3 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor3);
	minor3 = _mm_shuffle_ps(minor3, minor3, 0x4E);

	tmp1 = _mm_mul_ps(_mm_shuffle_ps(row1, row1, 0x4E), row3);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	row2 = _mm_shuffle_ps(row2, row2, 0x4E);
	minor0 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor0);
	minor2 = _mm_mul_ps(row0, tmp1);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row2, tmp1));
	minor2 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor2);
	minor2 = _mm_shuffle_ps(minor2, minor2, 0x4E);

	tmp1 = _mm_mul_ps(row0, row1);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);

	minor2 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor2);
	minor3 = _mm_sub_ps(_mm_mul_ps(row2, tmp1), minor3);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor2 = _mm_sub_ps(_mm_mul_ps(row3, tmp1), minor2);
	minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row2, tmp1));

	tmp1 = _mm_mul_ps(row0, row3);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row2, tmp1));
	minor2 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor2);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor1 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor1);
	minor2 = _mm_sub_ps(minor2, _mm_mul_ps(row1, tmp1));

	tmp1 = _mm_mul_ps(row0, row2);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	minor1 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor1);
	minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row1, tmp1));
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row3, tmp1));
	minor3 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor3);

	det = _mm_mul_ps(row0, minor0);
	det = _mm_add_ps(_mm_shuffle_ps(det, det, 0x4E), det);
	det = _mm_add_ss(_mm_shuffle_ps(det, det, 0xB1), det);
	tmp1 = _mm_rcp_ss(det);
	det = _mm_sub_ss(_mm_add_ss(tmp1, tmp1), _mm_mul_ss(det, _mm_mul_ss(tmp1, tmp1)));
	det = _mm_shuffle_ps(det, det, 0x00);
	minor0 = _mm_mul_ps(det, minor0);
	_mm_storel_pi((__m64*)(dest), minor0);
	_mm_storeh_pi((__m64*)(dest + 2), minor0);
	minor1 = _mm_mul_ps(det, minor1);
	_mm_storel_pi((__m64*)(dest + 4), minor1);
	_mm_storeh_pi((__m64*)(dest + 6), minor1);
	minor2 = _mm_mul_ps(det, minor2);
	_mm_storel_pi((__m64*)(dest + 8), minor2);
	_mm_storeh_pi((__m64*)(dest + 10), minor2);
	minor3 = _mm_mul_ps(det, minor3);
	_mm_storel_pi((__m64*)(dest + 12), minor3);
	_mm_storeh_pi((__m64*)(dest + 14), minor3);
}

inline rmatrix Inverse(const rmatrix& mat)
{
	rmatrix ret;
	Inverse(ret, mat);
	return ret;
}

inline void RMatInv(D3DXMATRIX& q, const D3DXMATRIX& a) {
	Inverse(q, a);
}

template <typename T>
auto Square(const T& x) {
	return x * x;
}

// Returns a matrix that rotates row vectors by
// the given angle in radians around the given axis.
//
// Appears counterclockwise in a right-handed coordinate system
// when the axis is pointing towards the observer.
// Axis should be a unit vector. Angle should be in radians.
inline rmatrix RotationMatrix(const v3& axis, float angle)
{
	rmatrix m;

	auto cosa = cos(angle);
	auto sina = sin(angle);

	m._11 = cosa + Square(axis.x) * (1 - cosa);
	m._12 = axis.y * axis.x * (1 - cosa) + axis.z * sina;
	m._13 = axis.z * axis.x * (1 - cosa) - axis.y * sina;
	m._14 = 0;

	m._21 = axis.x * axis.y * (1 - cosa) - axis.z * sina;
	m._22 = cosa + Square(axis.y) * (1 - cosa);
	m._23 = axis.z * axis.y * (1 - cosa) + axis.x * sina;
	m._24 = 0;

	m._31 = axis.x * axis.z * (1 - cosa) + axis.y * sina;
	m._32 = axis.y * axis.z * (1 - cosa) - axis.x * sina;
	m._33 = cosa + Square(axis.z) * (1 - cosa);
	m._34 = 0;

	m._41 = 0;
	m._42 = 0;
	m._43 = 0;
	m._44 = 1;

	return m;
}

// Returns a matrix that rotates row vectors
// around the x-axis by the given angle in degrees.
inline rmatrix RGetRotX(float angle) {
	auto rads = D3DX_PI / 180.f * angle;
	return RotationMatrix({ 1, 0, 0 }, rads);
}

inline rmatrix RGetRotY(float angle) {
	auto rads = D3DX_PI / 180.f * angle;
	return RotationMatrix({ 0, 1, 0 }, rads);
}

inline rmatrix RGetRotZ(float angle) {
	auto rads = D3DX_PI / 180.f * angle;
	return RotationMatrix({ 0, 0, 1 }, rads);
}

inline void GetIdentityMatrix(rmatrix& m)
{
	m._11 = 1; m._12 = 0; m._13 = 0; m._14 = 0;
	m._21 = 0; m._22 = 1; m._23 = 0; m._24 = 0;
	m._31 = 0; m._32 = 0; m._33 = 1; m._34 = 0;
	m._41 = 0; m._42 = 0; m._43 = 0; m._44 = 1;
}

inline rmatrix GetIdentityMatrix() {
	rmatrix m;
	GetIdentityMatrix(m);
	return m;
}

// Returns the translation a matrix applies as a vector.
inline rvector GetTransPos(const rmatrix& m) {
	return{ m._41, m._42, m._43 };
}

inline float sgn(float a)
{
	if (a > 0.0F) return (1.0F);
	if (a < 0.0F) return (-1.0F);
	return (0.0F);
}

inline v3 GetPlaneNormal(const rplane& plane) {
	return{ plane.a, plane.b, plane.c };
}

inline bool IntersectLineSegmentPlane(v3* hit, const rplane& plane, const v3& l0, const v3& l1)
{
	auto dot1 = DotPlaneNormal(plane, l0) - plane.d;
	auto dot2 = DotPlaneNormal(plane, l1) - plane.d;
	if (sgn(dot1) == sgn(dot2))
		return false;
	else if (!hit)
		return true;

	auto l = l1 - l0;
	Normalize(l);

	auto n = GetPlaneNormal(plane);
	auto p0 = n * plane.d;
	auto d = DotProduct(p0 - l0, n) / DotProduct(l, n);

	*hit = d * l + l0;
	return true;
}

inline v3 Transform(const v3& v, const rmatrix& mat)
{
	v3 ret;

	ret.x = v.x * mat._11 + v.y * mat._21 + v.z * mat._31 + mat._41;
	ret.y = v.x * mat._12 + v.y * mat._22 + v.z * mat._32 + mat._42;
	ret.z = v.x * mat._13 + v.y * mat._23 + v.z * mat._33 + mat._43;

	return ret;
}

inline v3 TransformCoord(const v3& v, const rmatrix& mat)
{
	v3 ret = Transform(v, mat);
	auto w = v.x * mat._14 + v.y * mat._24 + v.z * mat._34 + mat._44;
	ret /= w;
	return ret;
}

inline D3DXVECTOR3 operator*(const D3DXVECTOR3& v, const D3DXMATRIX &mat)
{
	return TransformCoord(v, mat);
}

inline D3DXVECTOR3& operator*=(D3DXVECTOR3& v, const D3DXMATRIX &mat)
{
	v = v * mat;
	return v;
}

_NAMESPACE_REALSPACE2_END
