#include "Image.h"

void Image::SetToList()
{
	//std::copy(_imagesFileSet.begin(), _imagesFileSet.end(), std::back_inserter(_imagesFileList));
}

void Image::SetBackgroundImage(const fs::path& path)
{
	_imageBackground = gl::Texture::create(loadImage(path));
}

void Image::SaveImage(fs::path& path)
{
	if (!_imagesSavePath.empty()) 
	{
		Surface s8(_imageBackground->createSource());

		writeImage(writeFile(_imagesSavePath.string() + path.string()), s8);
	}
}

void Image::DrawBackgroundImage(Area& window, Texture2dRef imageBackground)
{
	if (imageBackground)
	{
		gl::clear();
		Rectf dest = Rectf(imageBackground->getBounds()).getCenteredFill(window, true);
		gl::draw(imageBackground, dest);
	}
}

void Image::InitSheaders()
{
	_shaderSolid =
		gl::getStockShader(
			gl::ShaderDef().color()
		);

	_shaderBatch = gl::Batch::create(
		geom::Rect()
		.rect(Rectf(-15, -10, 15, 10)),
		_shaderSolid);
}

void Image::DrawShadedImages()
{
	std::vector<Area> _areaCoord;
	CalculateAreaCoordinates(&_areaCoord, _textureList.size(), vec2(0, 0), vec2(1200, 700));

	for (int i = 0; i < _textureList.size(); i++)
	{
		if (_textureList.size() == 0)
			break;
		
		Rectf drawRect(_areaCoord[i]);
		gl::draw(_textureList[i], drawRect);
	}
}

void Image::CreateBackgroundShadedImage(const std::vector<fs::path>& files)
{
	_frameBuffer->bindFramebuffer();

	for(auto& file : files)
	{
		ImageSourceRef image = loadImage(_imagesSavePath.string() + file.filename().string());
		_textureList.push_back(Texture2d::create(image));
	}

	sx2 = 1200, sy2 = 700;

	_frameBuffer->unbindFramebuffer();
}

void Image::CalculateHelper(std::vector<Area>* tmp, int size, vec2 ul, vec2 dr)
{
	int dx = dr.x / size;
	int x1, x2;
	int y1 = ul.y, y2 = dr.y;

	for (int i = 0; i < size; i++)
	{
		x1 = i * dx; x2 = (i + 1) * dx;

		tmp->push_back(Area(x1, y1, x2, y2));
	}
}

void Image::CalculateAreaCoordinates(std::vector<Area>* tmp, int size, vec2 ul, vec2 dr)
{
	if (size == 0)
		return;

	tmp->clear();

	if (size == 1)
	{
		Area a(sx1, sy1, sx2, sy2);

		tmp->push_back(a);

		return;
	}
	else if (size == 2)
	{
		tmp->push_back(Area(sx1,		 sy1, sx2/2, sy2));
		tmp->push_back(Area(sx1 + sx2/2, sy1, sx2,	sy2));
		return;
	}
	else if (size % 2 == 1)
	{
		CalculateHelper(tmp, size, ul, dr);
		return;
	}
	else if (size % 2 == 0)
	{
		CalculateHelper(tmp, size/2, ul, vec2(dr.x, dr.y / 2));
		CalculateHelper(tmp, size/2, vec2(ul.x, dr.y / 2), dr);
	}
}
