#ifndef SCENE_H
#define SCENE_H
#include<Eigen/Core>
#include <Eigen/Geometry>
#include<limits>
#include"model.h"
#include"shader.h"
#include"tgaimage.h"
/*
	scene类初始化时构造MVP矩阵，包含求差分坐标，渲染triangle 的函数
	shading函数执行渲染
*/
class Scene
{
public:
	Scene();

	void shading(const model&,const Shader&, TGAImage& image);
	void triangle(const model& ,const vector<Eigen::Vector3i>&,const Shader&, TGAImage& image);

	void set_shadowbuffer(const model&);

	Eigen::Vector3f barycentric(const vector<Eigen::Vector3f>&,Eigen::Vector2f);
	//depth = 255作者的意思是每个深度对应一个颜色，方便debug
	int width, height, depth = 255;

	//场景环境光方向，注意镜头移动的话需要调整方向
	Eigen::Vector3f enviroment_light;
	//调整后环境光方向
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

