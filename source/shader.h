#ifndef SHADER_H
#define SHADER_H
//��ͬ��shaderʵ�ֲ�ͬ����ȾЧ�� һ����shadingһ��/ÿ������shadingһ�Σ�Gouraud shading��/Bling-Fong shading texture shading
#include"tgaimage.h"
#include<Eigen/Core>
#include <Eigen/Geometry>

//��MVP���� light_dir�ŵ�Scene��һ���鷳�ĵ����ڣ�����дshader��Ҫ���ܶ�Scene�еĲ���
class Shader
{
public:
	Shader(){}
	virtual ~Shader() = default;
	virtual bool fragment_shading(const vector<Eigen::Vector3f>& pixel_coordinate,const Eigen::Vector3f&, const model&, 
		const vector<Eigen::Vector3i>& face, TGAColor& color, const Eigen::Vector3f&, const Eigen::Matrix4f&, float shadow) const {
		return false;
	}
};
//ÿ������ȡ����ƽ�淨����
class plane_shading :public Shader
{
public:
	plane_shading(){}
	bool fragment_shading(const vector<Eigen::Vector3f> &pixel_coordinate,const Eigen::Vector3f& barycentric_uv, const model&m,
		const vector<Eigen::Vector3i>& face, TGAColor& color,const Eigen::Vector3f& light_dir, const Eigen::Matrix4f& MVP, float shadow)const override
	{
		Eigen::Vector3f line1(pixel_coordinate[1] - pixel_coordinate[0]);
		Eigen::Vector3f line2(pixel_coordinate[2] - pixel_coordinate[0]);
		Eigen::Vector3f normal(line1.cross(line2).normalized());
		Eigen::Vector3f light_dir_normalized = light_dir.normalized();
		float intensity = std::fabs(light_dir_normalized.dot(normal));
		color = TGAColor(255, 255, 255) * intensity;
		return false;
	}
};


class grouraud_shading :public Shader
{
public:
	grouraud_shading(){}
	bool fragment_shading(const vector<Eigen::Vector3f>& pixel_coordinate, const Eigen::Vector3f& barycentric_uv, const model& m,
		const vector<Eigen::Vector3i>& face, TGAColor& color, const Eigen::Vector3f& light_dir, const Eigen::Matrix4f& MVP, float shadow)const override
	{
		Eigen::Matrix4f MVP_inverse_transpose = MVP.inverse().transpose();
		Eigen::Vector3f MVP_it_normal(0, 0, 0);
		for (int i = 0; i < 3; i++)
		{
			Eigen::Vector4f normal;
			normal << m.get_normal(face[i][2]), 1;
			normal = MVP_inverse_transpose * normal;
			MVP_it_normal += normal.head<3>().normalized() * barycentric_uv[i];
		}
		MVP_it_normal.normalize();
		Eigen::Vector3f light_dir_normalized = light_dir.normalized();
		float intensity = std::fmax(0, (light_dir_normalized).dot(MVP_it_normal));
		color = TGAColor(255, 255, 255) * intensity;
		return false;
	}
};


class texture_shading :public Shader
{
public:
	texture_shading(){}
	bool fragment_shading(const vector<Eigen::Vector3f>& pixel_coordinate, const Eigen::Vector3f& barycentric_uv, const model& m,
		const vector<Eigen::Vector3i>& face, TGAColor& color, const Eigen::Vector3f& light_dir, const Eigen::Matrix4f& MVP, float shadow)const override
	{
		Eigen::Vector2f uv(0, 0);
		for (int i = 0; i < 3; i++)
		{
			Eigen::Vector2f temp = m.get_uv(face[i][1]);
			uv += temp * barycentric_uv[i];
		}

		//��ַ�����
		Eigen::Matrix4f MVP_inverse_transpose = MVP.inverse().transpose();
		Eigen::Vector3f MVP_it_normal(0, 0, 0);
		for (int i = 0; i < 3; i++)
		{
			Eigen::Vector4f normal;
			normal << m.get_normal(face[i][2]), 1;
			normal = MVP_inverse_transpose * normal;
			MVP_it_normal += normal.head<3>().normalized() * barycentric_uv[i];
		}
		MVP_it_normal.normalize();
		Eigen::Vector3f light_dir_normalized = light_dir.normalized();
		float intensity = std::fmax(0, (light_dir_normalized).dot(MVP_it_normal));
		color = m.get_texture(Eigen::Vector2i(uv[0],uv[1]))*intensity;
		return false;
	}
};

//�ֱ�ͨ��ӳ��ͼ�����ص��color��normal
class texture_normal_shading :public Shader
{
public:
	texture_normal_shading() {}
	bool fragment_shading(const vector<Eigen::Vector3f>& pixel_coordinate, const Eigen::Vector3f& barycentric_uv, const model& m,
		const vector<Eigen::Vector3i>& face, TGAColor& color, const Eigen::Vector3f& light_dir, const Eigen::Matrix4f& MVP, float shadow)const override
	{
		Eigen::Vector2f uv(0, 0);
		for (int i = 0; i < 3; i++)
		{
			Eigen::Vector2f temp = m.get_uv(face[i][1]);
			uv += temp * barycentric_uv[i];
		}
		Eigen::Vector4f temp;
		temp << m.get_normal(Eigen::Vector2i(uv[0], uv[1])), 1;
		temp = MVP.inverse().transpose() * temp;
		Eigen::Vector3f normal = temp.head<3>().normalized();
		Eigen::Vector3f light_dir_normalized = light_dir.normalized();
		float intensity = std::fmax(0, (light_dir_normalized).dot(normal));
		color = m.get_texture(Eigen::Vector2i(uv[0], uv[1])) * intensity*shadow;
		return false;
	}
};

#endif // !SHADER_H

