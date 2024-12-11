#ifndef SCENE_H
#define SCENE_H
#include<Eigen/Core>
#include <Eigen/Geometry>
#include<limits>
#include"model.h"
#include"shader.h"
#include"tgaimage.h"
/*
	scene���ʼ��ʱ����MVP���󣬰����������꣬��Ⱦtriangle �ĺ���
	shading����ִ����Ⱦ
*/
class Scene
{
public:
	Scene();

	void shading(const model&,const Shader&, TGAImage& image);
	void triangle(const model& ,const vector<Eigen::Vector3i>&,const Shader&, TGAImage& image);

	void set_shadowbuffer(const model&);

	Eigen::Vector3f barycentric(const vector<Eigen::Vector3f>&,Eigen::Vector2f);
	//depth = 255���ߵ���˼��ÿ����ȶ�Ӧһ����ɫ������debug
	int width, height, depth = 255;

	//���������ⷽ��ע�⾵ͷ�ƶ��Ļ���Ҫ��������
	Eigen::Vector3f enviroment_light;
	//�����󻷾��ⷽ��
	Eigen::Vector3f light_dir;


	Eigen::Matrix4f M_ModelView;
	Eigen::Matrix4f M_PJT;
	Eigen::Matrix4f M_Viewport;
	Eigen::Matrix4f MVP;
	Eigen::Matrix4f MVP_Viewport;

	Eigen::Matrix4f world_to_shadow;
	Eigen::Matrix4f pixel_to_shadow;
	Eigen::Matrix4f M_test;
	float* zbuffer;
	float* shadowbuffer;

private:
	Eigen::Vector3f lookfrom;
	Eigen::Vector3f lookat;
	Eigen::Vector3f vup;
	Eigen::Matrix4f ModelView(Eigen::Vector3f, Eigen::Vector3f);
	Eigen::Matrix4f Projection(Eigen::Vector3f, Eigen::Vector3f);
	Eigen::Matrix4f Viewport();
};



#endif // !SCENE_H

