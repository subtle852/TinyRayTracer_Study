#include <limits>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm> //<-- max/min ����ϱ� ���� �߰�
#include "geometry.h"

struct Sphere
{
	Vec3f center;
	float radius;
	Vec3f color;

	Sphere(const Vec3f& c, const float& r, const Vec3f& color) : center{ c }, radius{ r }, color{color}
	{

	}

	bool rayIntersect(const Vec3f& orig, const Vec3f& dir, float& closestDist) const
	{
		Vec3f L = center - orig; //orig���� ����Ͽ� center�� ���ϴ� ����
		float tca = L * dir; // L vector�� dir vector�� projection
		float d2 = L * L - tca * tca; //L^2 = tca^2 + r^2�� ��� ���ϴ� �ﰢ��.
		if (d2 > radius*radius) return false; //r^2���� �Ÿ��� �ָ�, ray�� ���� �΋H���� ����
		float thc = sqrtf(radius*radius - d2);
		float t0 = tca - thc;
		float t1 = tca + thc; //t0�� t1�� �����/�� ������������ �Ķ����
		if (t0 < 0) t0 = t1;  //������ �ϳ��� ��� or dir �ݴ� ���⿡ ���� �ִ� ��츦 ó��
		if (t0 < 0) return false;

		if (t0 < closestDist)
		{
			closestDist = t0;
			return true;
		}
		return false;
	}

};

Vec3f castRay(const Vec3f& orig, const Vec3f& dir, const std::vector<Sphere>& scene)
{
	float sphereDist = std::numeric_limits<float>::max();
	Vec3f fillColor{};
	bool filled = false;
	for (const Sphere& s : scene)
	{
		if (s.rayIntersect(orig, dir, sphereDist))
		{
			fillColor = s.color;
			filled = true;
		}
	}
	
	if (!filled)
		return Vec3f(0.2, 0.7, 0.8);
	else
		return fillColor;
}

void render() {
	const int width = 1024;
	const int height = 768;
	const int fov = 3.14f / 2.f;
	std::vector<Vec3f> framebuffer(width*height);

	Vec3f ivory{ 0.4,0.4,0.3 };
	Vec3f red_rubber{ 0.3,0.1,0.1 };

	std::vector<Sphere> scene;
	scene.emplace_back(Vec3f(-3, 0, -16), 2, ivory);
	scene.emplace_back(Vec3f(-1.0, -1.5, -12), 2, red_rubber);
	scene.emplace_back(Vec3f(1.5, -0.5, -18), 3, red_rubber);
	scene.emplace_back(Vec3f(7, 5, -18), 4, ivory);

	for (size_t j = 0; j < height; j++) {
		for (size_t i = 0; i < width; i++) {
			float x = (2 * (i + 0.5) / (float)width - 1) * tan(fov / 2.)*width / (float)height;
			float y = -(2 * (j + 0.5) / (float)height - 1) * tan(fov / 2.);
			Vec3f dir = Vec3f(x, y, -1).normalize();
			framebuffer[i + j * width] = castRay(Vec3f(0, 0, 0), dir, scene); //ī�޶�� 0,0,0�� ��ġ
		}
	}

	std::ofstream ofs; // save the framebuffer to file
	ofs.open("./out.ppm", std::ofstream::out | std::ofstream::binary); //<--����
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (size_t i = 0; i < height*width; ++i) {
		for (size_t j = 0; j < 3; j++) {
			ofs << (char)(255 * std::max(0.f, std::min(1.f, framebuffer[i][j])));
		}
	}
	ofs.close();
}

int main() {
	render();
	return 0;
}