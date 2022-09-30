#include "pixelShader.h"

namespace render {
	pixelShader::pixelShader()
	{
		this->textures = nullptr;
	}
	void pixelShader::SetTextures(Texture_loaded* ts)
	{
		this->textures = ts;
	}
	Eigen::Vector3f pixelShader::Shading(ShadingData data, PixelShaderType shader)
	{
		Eigen::Vector3f res;
		switch (shader)
		{
		case render::PixelShaderType::COLOR:
			res = ColorShading(data);
			break;
		case render::PixelShaderType::BLINPHONG:
			res = BlinPhong(data);
			break;
		case render::PixelShaderType::LIGHT:
			res = LightSource(data);
			break;
		case render::PixelShaderType::SKYBOX:
			res = SkyBox(data);
			break;
		default:
			res = errorColor;
			break;
		}

		return res;
	}
	Eigen::Vector3f pixelShader::SampleTexture(int index, Eigen::Vector2f texcoord)
	{
		
		return textures->data[index].GetPixel(texcoord);
	}

	Eigen::Vector3f pixelShader::SampleCubeMap(Eigen::Vector3f dir)
	{
		Eigen::Vector3f res;

		Eigen::Vector3f len = Eigen::Vector3f(std::abs(dir.x()), std::abs(dir.y()), std::abs(dir.z()));

		Eigen::Vector2f texcoord;
		if (len.x() > len.y() && len.x() > len.z())
		{

			if (dir.x() >= 0)
			{
				texcoord = Eigen::Vector2f(dir.z(), dir.y()) * 0.5f / dir.x();
				texcoord = Eigen::Vector2f(0.5f + texcoord.x(), 0.5f - texcoord.y());
				res = SampleTexture(0, texcoord);
			}

			else
			{
				texcoord = Eigen::Vector2f(dir.z(), dir.y()) * 0.5f / dir.x();
				texcoord = Eigen::Vector2f(0.5f + texcoord.x(), 0.5f + texcoord.y());
				res = SampleTexture(1, texcoord);
			}

		}
		else if (len.y() > len.z())
		{

			if (dir.y() >= 0)
			{
				texcoord = Eigen::Vector2f(dir.x(), dir.z()) * 0.5f / dir.y();
				texcoord = Eigen::Vector2f(0.5f + texcoord.x(), 0.5f - texcoord.y());
				res = SampleTexture(2, texcoord);
			}

			else
			{
				texcoord = Eigen::Vector2f(dir.x(), dir.z()) * 0.5f / dir.y();
				texcoord = Eigen::Vector2f(0.5f - texcoord.x(), 0.5f - texcoord.y());
				res = SampleTexture(3, texcoord);
			}

		}
		else
		{
			if (dir.z() >= 0)
			{
				texcoord = Eigen::Vector2f(-dir.x(), dir.y()) * 0.5f / dir.z();
				texcoord = Eigen::Vector2f(0.5f + texcoord.x(), 0.5f - texcoord.y());
				res = SampleTexture(5, texcoord);
			}

			else
			{
				texcoord = Eigen::Vector2f(-dir.x(), dir.y()) * 0.5f / dir.z();
				texcoord = Eigen::Vector2f(0.5f + texcoord.x(), 0.5f + texcoord.y());
				res = SampleTexture(4, texcoord);
			}

		}

		return res;
	}


	Eigen::Vector3f render::pixelShader::ColorShading(ShadingData data)
	{
		return data.color;
	}
	Eigen::Vector3f pixelShader::BlinPhong(ShadingData data)
	{
		Eigen::Vector3f color = Eigen::Vector3f::Zero();
		
		Eigen::Vector3f ambient = Eigen::Vector3f(0.15f, 0.15f, 0.15f) * 255.0f;
		Eigen::Vector3f diffuse = Eigen::Vector3f::Zero();
		Eigen::Vector3f specular = Eigen::Vector3f::Zero();


		for (auto& light : this->lights)
		{
			Eigen::Vector3f vi;
			Eigen::Vector3f vo;
			Eigen::Vector3f h;
			float distance2;
			Eigen::Vector3f kd;
			Eigen::Vector3f ks;
			float p = 100.0f;

			if (light.type == LightType::PointLight)
			{
				vi = light.position - data.worldPos;
			}
			else
			{
				vi = light.direction;
			}

			vo = data.cameraPos - data.worldPos;
			distance2 = vi.squaredNorm();
			vi.normalize();
			vo.normalize();
			h = (vi + vo).normalized();
	
			if (data.textureIndex == -1)
			{
				kd = testColor;
				ks = kd;
			}
			else
			{
				kd = SampleTexture(data.textureIndex, data.texcoord);
				ks = kd;
				//VectorPrint(kd);
			}


			
			diffuse += kd.cwiseProduct(light.intensity) / distance2 * std::max(0.0f, data.normal.dot(vi));
			specular += kd.cwiseProduct(light.intensity) / distance2 * powf(std::max(0.0f, data.normal.dot(h)), p);

			
		}
		color = ambient + diffuse + specular;

		return color;
	}
	Eigen::Vector3f pixelShader::LightSource(ShadingData)
	{
		return Eigen::Vector3f(1.0f,1.0f,1.0f) * 255.0f;
	}
	Eigen::Vector3f pixelShader::SkyBox(ShadingData data)
	{
		Eigen::Vector3f res;

		Eigen::Vector3f dir = (data.worldPos - data.cameraPos).normalized();
		
		res = SampleCubeMap(dir);
		//dir.normalize();
		//VectorPrint(dir);
		return res;
	}
}

