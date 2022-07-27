/*
	beamis version 0.1
	- can render walls
*/

#include <SDL.h>
#include <iostream>


inline Uint32 RGB(uint32_t red, uint32_t green, uint32_t blue)
{
	return (red << 16) | (green << 8) | blue;
}

Uint32 getpixel(SDL_Surface* surface, int x, int y) {

	int bpp = surface->format->BytesPerPixel;
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp)
	{
	case 1:
		return *p;
		break;

	case 2:
		return *(Uint16*)p;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;
		break;

	case 4:
		return *(Uint32*)p;
		break;

	default:
		return 0;       /* shouldn't happen, but avoids warnings */
	}
}


class BeamRenderer
{
public:
	void setRenderer(SDL_Renderer*& sRenderer, int sWidth, int sHeight) {
		renderer = sRenderer;
		width = sWidth;
		height = sHeight;
		halfHeight = height / 2;
	}
	void setLevel(int* sLevel, int sLevelw, int sLevelh) {
		level = sLevel; // level map | walls
		levelw = sLevelw; // level map width
		levelh = sLevelh; // level map height
	}
	void setLevelSprites(float sSpriteMap[][3], int size) {
		renderDataSize += size;
		spriteMapSize = size;
		spriteMap = new float* [size];
		for (int i = 0; i < size; i++)
			spriteMap[i] = sSpriteMap[i];
	}
	void setFlats(int* sFloor, int* sCeil) {
		floor_level = sFloor;
		ceil_level = sCeil;
	}
	void setLights(float* sLight) {
		light_level = sLight;
	}
	void setSky(const char* sPath) {
		skyPath = sPath;
	}
	void setTileWalls(const char* sTile, int sTilew, int sTileh) {
		tile = sTile;
		tilew = sTilew;
		tileh = sTileh;
	}
	void setTileSprites(const char* sTileS, int sTileSw, int sTileSh, int sScolkeyr, int sScolkeyg, int sScolkeyb) {
		tileS = sTileS; // sprites tile
		tileSw = sTileSw; // ... width
		tileSh = sTileSh; // ... height
		scolkeyr = sScolkeyr; // sprite color key
		scolkeyg = sScolkeyg;
		scolkeyb = sScolkeyb;
	}
	void setCam(float sCamx, float sCamy, float sCama, float sCamf) {
		camx = sCamx;
		camy = sCamy;
		cama = sCama;
		camf = sCamf;
		while (cama > 3.14) cama -= 6.28;
		while (cama < -3.14) cama += 6.28;
	}
	void init(SDL_Window* window) {
		const SDL_PixelFormat* sFormat = SDL_GetWindowSurface(window)->format;
		Uint32 sCformat = SDL_GetWindowPixelFormat(window);
		renderDataSize += width;
		fstep = camf / width;

		// walls tile
		SDL_Surface* image = SDL_LoadBMP(tile);
		wallsurf = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGB888, 0);
		walls = SDL_CreateTextureFromSurface(renderer, wallsurf);
		tileww = image->w / tilew; // walls per width
		tilewh = image->h / tileh; // walls per height

		RGBwallSurf = new Uint8 * [wallsurf->w * wallsurf->h];
		for (int i = 0; i < wallsurf->w * wallsurf->h; i++) {
			SDL_GetRGB(getpixel(wallsurf, i % wallsurf->w, i / wallsurf->w), wallsurf->format, &_fcr, &_fcg, &_fcb);
			//std::cout << i % wallsurf->w << ' ' << i / wallsurf->w << ' ' << wallsurf->format->format << ' ' << SDL_PIXELFORMAT_RGB888 << '\n';
			RGBwallSurf[i] = new Uint8[3];
		}
		for (int i = 0; i < wallsurf->w * wallsurf->h; i++) {
			SDL_GetRGB(getpixel(wallsurf, i % wallsurf->w, i / wallsurf->w), wallsurf->format, &_fcr, &_fcg, &_fcb);
			RGBwallSurf[i][0] = _fcr;
			RGBwallSurf[i][1] = _fcg;
			RGBwallSurf[i][2] = _fcb;
		}

		
		// sprites tile
		image = SDL_LoadBMP(tileS);
		sprsurf = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGB888, 0);
		//image = SDL_ConvertSurfaceFormat(image, sCformat, 0);
		//std::cout<<SDL_SetColorKey(image, SDL_TRUE, SDL_MapRGB(sFormat, scolkeyr, scolkeyg, scolkeyb));
		//sprites = SDL_CreateTextureFromSurface(renderer, image);
		tileSww = image->w / tileSw; // sprites per width
		tileSwh = image->h / tileSh; // sprites per height

		RGBsprSurf = new Uint8 * [sprsurf->w * sprsurf->h];
		for (int i = 0; i < sprsurf->w * sprsurf->h; i++) {
			SDL_GetRGB(getpixel(sprsurf, i % sprsurf->w, i / sprsurf->w), sprsurf->format, &_fcr, &_fcg, &_fcb);
			//std::cout << i % wallsurf->w << ' ' << i / wallsurf->w << ' ' << wallsurf->format->format << ' ' << SDL_PIXELFORMAT_RGB888 << '\n';
			RGBsprSurf[i] = new Uint8[3];
		}
		for (int i = 0; i < sprsurf->w * sprsurf->h; i++) {
			SDL_GetRGB(getpixel(sprsurf, i % sprsurf->w, i / sprsurf->w), sprsurf->format, &_fcr, &_fcg, &_fcb);
			RGBsprSurf[i][0] = _fcr;
			RGBsprSurf[i][1] = _fcg;
			RGBsprSurf[i][2] = _fcb;
		}
		
		SDL_FreeSurface(image);

		image = SDL_LoadBMP(skyPath);
		image = SDL_ConvertSurfaceFormat(image, sCformat, 0);
		skysurf = image;

		renderData = new float* [renderDataSize];
		for (int i = 0; i < renderDataSize; i++)
			renderData[i] = new float[7];

		surftex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, width, height);
		_d_del = 1.f / height;
		_w_del = 1.f / width;

		colorkey_conv = RGB(scolkeyr, scolkeyg, scolkeyb);
	}
	void renderWalls() {
		col = 0;
		for (angle = cama - camf / 2; angle < cama + camf / 2; angle += fstep) {
			cos = cosf(angle);
			sin = sinf(angle);
			xdir = cos > 0;
			ydir = sin > 0;

			// Bresenham's algorithm
			// and yes, you can unite it, but then this mf will be slower
			if (sin) { // y iteration
				ydel = camy - (int)camy;
				yy = 0;
				if (ydir) // down
					while (!yobj) {
						yy += 1;
						yfar = (yy - ydel) / sin;
						yx = camx + yfar * cos;
						if (yy + camy >= levelh || yy + camy < 0 || yx >= levelw || yx < 0)
						{
							yfar = 100.0;
							break;
						}
						yobj = level[(int)(yy + camy) * levelw + yx];
					}
				else // up
					while (!yobj) {
						yfar = (yy - ydel) / sin;
						yy -= 1;
						yx = camx + yfar * cos;
						if (yy + camy >= levelh || yy + camy < 0 || yx >= levelw || yx < 0)
						{
							yfar = 100.0;
							break;
						}
						yobj = level[(int)(yy + camy) * levelw + yx];
					}
			}
			else
				yfar = 100.0;

			if (cos) { // x iteration
				xdel = camx - (int)camx;
				xx = 0;
				if (xdir) // right
					while (!xobj) {
						xx += 1;
						xfar = (xx - xdel) / cos;
						xy = camy + xfar * sin;
						if (xx + camx >= levelw || xx + camx < 0 || xy >= levelh || xy < 0)
						{
							xfar = 100.0;
							break;
						}
						xobj = level[xy * levelw + (int)(xx + camx)];
					}
				else // left
					while (!xobj) {
						xfar = (xx - xdel) / cos;
						xx -= 1;
						xy = camy + xfar * sin;
						if (xx + camx >= levelw || xx + camx < 0 || xy >= levelh || xy < 0)
						{
							xfar = 100.0;
							break;
						}
						xobj = level[xy * levelw + (int)(xx + camx)];
					}
			}
			else
				xfar = 100.0;

			bright = 1;

			if (xfar < yfar) { // the iteration over x is the nearest
				obj = xobj;
				far = xfar;
				if (xx > 0) {
					cut = camy + sin * far - xy;
					bright = light_level[(int)camx + xx + levelw * (int)xy - 1];
				}
				else {
					cut = 1 - camy - sin * far + xy;
					bright = light_level[(int)camx + xx + levelw * (int)xy + 1];
				}
			}
			else { // the iteration over y is the nearest
				obj = yobj;
				far = yfar;
				if (yy < 0) {
					cut = camx + cos * far - yx;
					bright = light_level[levelw * (int)camy + (int)yx + levelw * yy + levelw];
				}
				else {
					cut = 1 - camx - cos * far + yx;
					bright = light_level[levelw * (int)camy + (int)yx + levelw * yy - levelw];
				}
			}

			if (obj) {
				renderData[col][0] = 0; // what object? | wall = 0
				renderData[col][1] = far; // distance to the wall
				renderData[col][2] = (float)obj; // wall number
				renderData[col][3] = angle; // ray angle
				renderData[col][4] = col; // screen column
				renderData[col][5] = cut; // wall cut (0 - 1 float) wall * cut = wall_line_ver
				renderData[col][6] = bright; // brightness
				obj = 0;
			}
			else
				renderData[col][0] = -1;
			col++;
			far = 0;
			xobj = 0;
			yobj = 0;
			if (col >= width)
				break;
		}
	}
	void renderSprites() {
		angle = cama;
		for (int i = 0; i < spriteMapSize; i++) {
			sprite = spriteMap[i][0];
			x = spriteMap[i][1];
			y = spriteMap[i][2];

			sfar = sqrtf(powf(camx - x, 2) + powf(camy - y, 2));
			sdir = atan2f(y - camy, x - camx);

			if (sdir - angle > 3.14) sdir -= 6.28;
			if (sdir - angle < -3.14) sdir += 6.28;

			ssize = fminf(5000, height / sfar);
			shor = (sdir - angle + camf / 2) / fstep - ssize / 2;

			if ((shor + ssize >= 0) && (shor < width)) {
				sfar *= cosf(sdir - angle);
				ssize /= cosf(sdir - angle);
				sver = (height - ssize) / 2;
				renderData[i + width][0] = 1; // what object? | sprite = 1
				renderData[i + width][1] = sfar; // distance to the sprite
				renderData[i + width][2] = sprite; // sprite number
				renderData[i + width][3] = shor; // horizontal position
				renderData[i + width][4] = sver; // vertical position
				renderData[i + width][5] = ssize; // size of sprite
				renderData[i + width][6] = light_level[int(x) + int(y) * levelw];
			}
			else
				renderData[i + width][0] = -1;
		}
	}
	void show() {

		renderWalls();
		renderSprites();

		for (int i = 0; i < renderDataSize; i++) { // depth | painter's algorithm
			renderMax = i;
			for (int j = i; j < renderDataSize; j++)
				if (renderData[j][1] > renderData[renderMax][1])
					renderMax = j;
			renderDataMem = renderData[i];
			renderData[i] = renderData[renderMax];
			renderData[renderMax] = renderDataMem;
		}

		// Oh my fucking God... Rendering code...

		if (!SDL_LockTexture(surftex, NULL, (void**)&pixbuf, &pitch)) {

			for (int y = 0; y < halfHeight; y++) {
				for (int x = 0; x < width; x++) {

					_fs = (camf / width) * (x - width / 2);
					_sax = camx + 1.f / y * (halfHeight * cosf(cama + _fs)) / cosf(_fs);
					_say = camy + 1.f / y * (halfHeight * sinf(cama + _fs)) / cosf(_fs);

					if (_say < levelh && _say > 0 && _sax < levelw && _sax > 0) {
						int ind = (int)_say * levelw + (int)_sax;

						floor_tile = floor_level[ind];
						ceil_tile = ceil_level[ind];
						_sax = _sax - (int)_sax;
						_say = _say - (int)_say;

						int _cpx = (ceil_tile % tileww) * tilew + _sax * tilew, _cpy = (ceil_tile / tileww) * tileh + _say * tileh;
						int _fpx = (floor_tile % tileww) * tilew + _sax * tilew, _fpy = (floor_tile / tileww) * tileh + _say * tileh;
						
						ceil_color = getpixel(wallsurf, _cpx, _cpy);

						if (ceil_color != colorkey_conv) { // ceiling
							_fcr = RGBwallSurf[_cpx + _cpy * wallsurf->w][0];
							_fcg = RGBwallSurf[_cpx + _cpy * wallsurf->w][1];
							_fcb = RGBwallSurf[_cpx + _cpy * wallsurf->w][2];
							pixbuf[halfHeight * width - y * width + x - width] = RGB(_fcr * light_level[ind], _fcg * light_level[ind], _fcb * light_level[ind]);
						}
						else {  // sky
							slice = (cama + _fs) / 3.15;  // ~PI
							if (slice < 0) slice++;
							if (slice > 1) slice--;

							pixbuf[halfHeight * width - y * width + x - width] = getpixel(skysurf, slice * skysurf->w, (float)skysurf->h / (halfHeight) * (halfHeight - y));
						}

						// floor
						_fcr = RGBwallSurf[_fpx + _fpy * wallsurf->w][0];
						_fcg = RGBwallSurf[_fpx + _fpy * wallsurf->w][1];
						_fcb = RGBwallSurf[_fpx + _fpy * wallsurf->w][2];

						pixbuf[y * width + x + halfHeight * width] = RGB(_fcr * light_level[ind], _fcg * light_level[ind], _fcb * light_level[ind]);
					}
				}
			}
			for (int i = 0; i < renderDataSize; i++) {
				if (renderData[i][0] == 0) { // walls
					dop = 0;

					cheight = height / renderData[i][1] / cosf(renderData[i][3] - cama);
					if (cheight > height) {
						dop = (cheight - height) / 2;
						cheight = height;
					}

					nw = ((int)renderData[i][2] - 1) % tileww; // wall image number from left border of the tile
					nh = ((int)renderData[i][2] - 1) / tileww; // wall image number from top border of the tile

					int sp = (halfHeight - cheight / 2) - dop, ep = (halfHeight + cheight / 2) + dop - 1;
					int _cpx = nw * tilew + (int)(tilew * renderData[i][5]), _cpy = 0;
					float _a1 = -halfHeight + cheight / 2 + dop, _a2 = ((float)tileh / (cheight + dop * 2));

					for (int j=halfHeight - cheight / 2; j < halfHeight + cheight / 2; j++) {
						_cpy = nh * tileh + (j + _a1) * _a2;
						_fcr = RGBwallSurf[_cpx + _cpy * wallsurf->w][0];
						_fcg = RGBwallSurf[_cpx + _cpy * wallsurf->w][1];
						_fcb = RGBwallSurf[_cpx + _cpy * wallsurf->w][2];
						pixbuf[(int)renderData[i][4] + j * width] = RGB(_fcr * renderData[i][6], _fcg * renderData[i][6], _fcb * renderData[i][6]); // getpixel(wallsurf, nw * tilew + (int)(tilew * renderData[i][5]), nh * tileh + (j - halfHeight + cheight / 2 + dop) * ((float)tileh / (cheight + dop * 2)));
					}
				}
				if (renderData[i][0] == 1) { // sprites
					nw = (int)renderData[i][2] % tileSww;
					nh = renderData[i][2] / tileSww;

					int _cpx = nw * tileSw, _cpy = nh * tileSh;
					int _sdy = (renderData[i][4] < 0 ? 0 : renderData[i][4]), _sdx = (renderData[i][3] < 0 ? 0 : renderData[i][3]),  // cut (x < 0 || x >= width etc...)
						_edy = (renderData[i][4] + renderData[i][5] >= height ? height : renderData[i][4] + renderData[i][5]),
						_edx = (renderData[i][3] + renderData[i][5] >= width ? width : renderData[i][3] + renderData[i][5]);

					int xdt = _sdx - renderData[i][3], ydt = _sdy - renderData[i][4]; // pixels out of the left bound
					int ind = 0;
					for (int y = _sdy; y < _edy; y++) {
						for (int x = _sdx; x < _edx; x++) {
							xx = _cpx + (x - _sdx + xdt) * tileSw / renderData[i][5];
							yy = _cpy + (y - _sdy + ydt) * tileSh / renderData[i][5];
							if (getpixel(sprsurf, xx, yy) != colorkey_conv) {
								ind = xx + yy * sprsurf->w;
								_fcr = RGBsprSurf[ind][0];
								_fcg = RGBsprSurf[ind][1];
								_fcb = RGBsprSurf[ind][2];
								pixbuf[x + y * width] = RGB(_fcr * renderData[i][6], _fcg * renderData[i][6], _fcb * renderData[i][6]);
							}
						}
					}
				}
			}
			SDL_UnlockTexture(surftex);
			SDL_RenderCopy(renderer, surftex, NULL, NULL);
		}
		else
			std::cout << "notlocked\n";
	}

	void close() {
		SDL_DestroyTexture(walls);
		SDL_DestroyTexture(sprites);
	}

private:
	float _d, _w, _sx, _sy, _ex, _ey, _sax, _say, _fs, slice;
	int fps_start = 0, fps_end = 0, fps_delta = 0, fps_digits = 0;
	int xobj, yobj, xdir, ydir, col, nw, nh, obj, xx, yy, xy, yx, cheight, dop, ssize, shor, sver;
	
	Uint8 _fcr, _fcg, _fcb;

	Uint32 colorkey_conv, ceil_color, floor_color;

	float _d_del, _w_del;
	int floor_tile, ceil_tile;

	SDL_Texture* surftex;
	SDL_Surface* wallsurf, *skysurf, *sprsurf;
	Uint32* pixbuf = nullptr;
	Uint8** RGBwallSurf, **RGBsprSurf;
	int pitch = 0;

	// 1.1 fspeed | 0.7 ffar

	float far, xfar, yfar, cos, sin, cut, xdel, ydel, fstep, angle, sprite, x, y, sfar, sdir;
	const char* tile, *tileS, *skyPath;
	int* level, *floor_level, *ceil_level;
	int halfHeight;
	float* light_level;
	int width, height, levelw, levelh, tilew,
		tileh, tileww, tilewh, tileSw, tileSh,
		tileSww, tileSwh, scolkeyr, scolkeyg, scolkeyb,
		renderDataSize = 0, spriteMapSize, renderMax;
	float** renderData;
	float** spriteMap;
	float* renderDataMem = {};
	float camx, camy, cama, camf;
	float bright;
	Uint32* walls_pixels;
	SDL_Renderer* renderer;
	SDL_Texture* walls;
	SDL_Texture* sprites;
	SDL_Rect r1, r2;
};