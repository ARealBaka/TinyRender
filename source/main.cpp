#include"model.h"
#include"scene.h"
int main()
{
	//diablo3_pose african_head
	model mymodel("./objs/african_head.obj");
	if (!mymodel.flag)
	{
		std::cerr << "open file failed\n";
		return 1;
	}
	Scene scene;
	
	flat_shading flat_shader;
	grouraud_shading grouraud_shader;
	texture_shading texture_shader;
	texture_normal_shading texture_normal_shader;
	TGAImage image(scene.width, scene.height, TGAImage::RGB);
	scene.shading(mymodel, texture_normal_shader,image);
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("./images/texture_normal_shadow_mapping_african_head.tga");

	return 0;
}