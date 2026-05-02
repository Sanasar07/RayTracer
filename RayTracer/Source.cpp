#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <limits>
#include <memory>
using namespace std;



struct vec3
{
	double vec[3];

	vec3() : vec{ 0, 0, 0 } {}

	vec3(double x, double y, double z) : vec{ x,y,z } {}

	double x() const { return vec[0]; }
	double y() const { return vec[1]; }
	double z() const { return vec[2]; }

	vec3 operator+(const vec3& other) const
	{
		return vec3(vec[0] + other.vec[0], vec[1] + other.vec[1], vec[2] + other.vec[2]);
	}

	vec3 operator-(const vec3& other) const
	{
		return vec3(vec[0] - other.vec[0], vec[1] - other.vec[1], vec[2] - other.vec[2]);

	}

	double dot(const vec3& other) const
	{
		return vec[0] * other.vec[0] + vec[1] * other.vec[1] + vec[2] * other.vec[2];
	}

	vec3 operator/(double scalar) const
	{
		return vec3(vec[0] / scalar, vec[1] / scalar, vec[2] / scalar);
	}

	double length() const
	{
		return sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
	}

	vec3 normalize() const
	{
		return *this / length();

	}

};

using color = vec3;

// 1. Умножение вектора на число (vec3 * double)
inline vec3 operator*(const vec3& v, double t) {
	return vec3(v.vec[0] * t, v.vec[1] * t, v.vec[2] * t);
}

// 2. Умножение числа на вектор (double * vec3) — просто перенаправляем на верхнюю
inline vec3 operator*(double t, const vec3& v) {
	return v * t;
}

// 3. Покомпонентное умножение векторов (vec3 * vec3) — часто нужно для цветов
inline vec3 operator*(const vec3& u, const vec3& v) {
	return vec3(u.vec[0] * v.vec[0], u.vec[1] * v.vec[1], u.vec[2] * v.vec[2]);
}


struct ray
{
	vec3 origin;
	vec3 direction;

	ray(const vec3& o, const vec3& d) : origin(o), direction(d) {}

	vec3 at(double t) const
	{
		return origin + direction * t;
	}
};

struct camera
{
	vec3 origin = { 0,0,0 };
	vec3 lower_left_corner = { -2, -1, -1 };
	vec3 horizontal = { 4,0,0 };
	vec3 vertical = { 0, 2, 0 };

	ray get_ray(double u, double v)const
	{
		return ray{ origin, lower_left_corner + horizontal * u + vertical * v - origin };

	}

};



struct HitRecord
{
	vec3 point;
	vec3 normal;
	double t;
};




 class Hittable
{
private:


public:
	virtual bool hit(const ray& r, double t_min, double t_max, HitRecord& rec) const = 0;

	
		virtual ~Hittable(){}
	

};

 class Sphere : public Hittable {
 private:
	 vec3 center;
	 double radius;
 public:
	 Sphere(const vec3& c, double r) : center(c), radius(r) {}

	 bool hit(const ray& r, double t_min, double t_max, HitRecord& rec) const override
	 {// 1. Используем поле origin (без скобок)
		 vec3 oc = r.origin - center;

		 // 2. Исправляем dot: вызываем метод через объект
		 // Вместо dot(r.direction, r.direction) пишем r.direction.dot(r.direction)
		 auto a = r.direction.dot(r.direction);

		 // Вместо dot(oc, r.direction) пишем oc.dot(r.direction)
		 auto b = 2.0 * oc.dot(r.direction);

		 auto c = oc.dot(oc) - radius * radius;


		 auto discriminant = b * b - 4 * a * c;

		 if (discriminant < 0)
		 {
			 return false;
		 }
		 else
		 {
			 // Математическое уточнение: 
			 // В уравнении ray-sphere intersection для b = 2*dot(oc, dir) 
			 // формула корней t = (-b ± sqrt(D)) / (2*a)
			 // Если b уже удвоено, то 2*a в знаменателе корректно.
			 double t =  (-b - sqrt(discriminant)) / (2.0 * a);
			 if(t < t_min or t > t_max)
			 {
				 return false;
			 }
			 rec.t = t;
			 rec.point = r.at(t);
			 rec.normal = (rec.point - center) / radius;
			 return true;
		 }
	 }

	 
 };








double hit_sphere(const vec3& center, double radius, const ray& r)
{
	// 1. Используем поле origin (без скобок)
	vec3 oc = r.origin - center;

	// 2. Исправляем dot: вызываем метод через объект
	// Вместо dot(r.direction, r.direction) пишем r.direction.dot(r.direction)
	auto a = r.direction.dot(r.direction);

	// Вместо dot(oc, r.direction) пишем oc.dot(r.direction)
	auto b = 2.0 * oc.dot(r.direction);

	auto c = oc.dot(oc) - radius * radius;

	auto discriminant = b * b - 4 * a * c;

	if (discriminant < 0)
	{
		return -1.0;
	}
	else
	{
		// Математическое уточнение: 
		// В уравнении ray-sphere intersection для b = 2*dot(oc, dir) 
		// формула корней t = (-b ± sqrt(D)) / (2*a)
		// Если b уже удвоено, то 2*a в знаменателе корректно.
		return (-b - sqrt(discriminant)) / (2.0 * a);
	}
}

color ray_color(const ray& r, const vector<shared_ptr<Hittable>>& world)
{
	HitRecord rec;
	bool hit_anything = false;
	double closest = 1e9;
	for(auto& object : world)
	{
		HitRecord temp_rec;
		if(object->hit(r, 0.001, closest, temp_rec))
		{
			hit_anything = true;
			closest = temp_rec.t;
			rec = temp_rec;
		}
	}
	if (hit_anything)
	{
		return 0.5 * color(rec.normal.x() + 1, rec.normal.y() + 1, rec.normal.z() + 1);
	}
	vec3 unit_direction = r.direction.normalize();
	double t = 0.5 * (unit_direction.y() + 1.0);
	if (t > 0)
	{
		vec3 normal = (r.at(t) - vec3(0, 0, -1)).normalize();
		return 0.5 * color(normal.x() + 1, normal.y() + 1, normal.z() + 1);

	} // Красный цвет для сферы;
	return (1.0 - t) * color(1, 1, 1) + t * color(0.5, 0.7, 1.0); // Градиент от белого к голубому
}


int main()
{
	vector<shared_ptr<Hittable>> world;
	world.push_back(make_shared<Sphere>(vec3(0, 0, -1), 0.5));      // маленькая сфера
	world.push_back(make_shared<Sphere>(vec3(0, -100.5, -1), 100)); // большая сфера-пол

	int width = 400;
	int height = 200;
	ofstream file("version5.ppm");

	file << "P3\n" << width << ' ' << height << "\n255\n";

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			auto u = double(j) / (width - 1);
			auto v = double(i) / (height - 1);
			ray r = camera().get_ray(u, v);

			vec3 color = ray_color(r,world);

			int ir = int(255.999 * color.x());
			int ig = int(255.999 * color.y());
			int ib = int(255.999 * color.z());

			file << ir << ' ' << ig << ' ' << ib << '\n';

		}


	}

	file.close();
}