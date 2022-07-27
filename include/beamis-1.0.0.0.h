/*
	beamis version 0.1
	- can render walls
*/

#include <SDL.h>
#include <iostream>


class BeamRenderer
{
public:
	void setRenderer(SDL_Renderer*& sRenderer, int sWidth, int sHeight) {
		renderer = sRenderer;
		width = sWidth;
		height = sHeight;
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
	void setTileWalls(char* sTile, int sTilew, int sTileh) {
		tile = sTile;
		tilew = sTilew;
		tileh = sTileh;
	}
	void setTileSprites(char* sTileS, int sTileSw, int sTileSh, int sScolkeyr, int sScolkeyg, int sScolkeyb) {
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
	}
	void init(SDL_Window* window) {
		const SDL_PixelFormat* sFormat = SDL_GetWindowSurface(window)->format;
		Uint32 sCformat = SDL_GetWindowPixelFormat(window);
		renderDataSize += width;
		fstep = camf / width;

		// walls tile
		SDL_Surface* image = SDL_LoadBMP(tile);
		walls = SDL_CreateTextureFromSurface(renderer, image);
		tileww = image->w / tilew; // walls per width
		tilewh = image->h / tileh; // walls per height

		
		// sprites tile
		image = SDL_LoadBMP(tileS);
		image = SDL_ConvertSurfaceFormat(image, sCformat, 0);
		std::cout<<SDL_SetColorKey(image, SDL_TRUE, SDL_MapRGB(sFormat, scolkeyr, scolkeyg, scolkeyb));
		sprites = SDL_CreateTextureFromSurface(renderer, image);
		tileSww = image->w / tileSw; // sprites per width
		tileSwh = image->h / tileSh; // sprites per height
		
		SDL_FreeSurface(image);

		renderData = new float* [renderDataSize];
		for (int i = 0; i < renderDataSize; i++)
			renderData[i] = new float[6];
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

			if (xfar < yfar) { // the iteration over x is the nearest
				obj = xobj;
				far = xfar;
				if (xx > 0)
					cut = camy + sin * far - xy;
				else // mirroring fix
					cut = 1 - camy - sin * far + xy;
			}
			else { // the iteration over y is the nearest
				obj = yobj;
				far = yfar;
				if (yy < 0)
					cut = camx + cos * far - yx;
				else // mirroring fix
					cut = 1 - camx - cos * far + yx;
			}

			if (obj) {
				renderData[col][0] = 0; // what object? | wall = 0
				renderData[col][1] = far; // distance to the wall
				renderData[col][2] = (float)obj; // wall number
				renderData[col][3] = angle; // ray angle
				renderData[col][4] = col; // screen column
				renderData[col][5] = cut; // wall cut (0 - 1 float) wall * cut = wall_line_ver
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
		while (angle > 3.14) angle -= 6.28;
		while (angle < -3.14) angle += 6.28;
		for (int i = 0; i < spriteMapSize; i++) {
			sprite = spriteMap[i][0];
			x = spriteMap[i][1];
			y = spriteMap[i][2];

			sfar = sqrtf(powf(camx - x, 2) + powf(camy - y, 2));
			sdir = atan2f(y - camy, x - camx);

			while (sdir - angle > 3.14) sdir -= 6.28;
			while (sdir - angle < -3.14) sdir += 6.28;

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
			}
			else
				renderData[i + width][0] = -1;
		}
	}
	void show() {
		for (int i = 0; i < renderDataSize; i++) { // depth | painter's algorithm
			renderMax = i;
			for (int j = i; j < renderDataSize; j++)
				if (renderData[j][1] > renderData[renderMax][1])
					renderMax = j;
			renderDataMem = renderData[i];
			renderData[i] = renderData[renderMax];
			renderData[renderMax] = renderDataMem;
		}
		for (int i = 0; i < renderDataSize; i++) {
			if (renderData[i][0] == 0) {
				dop = 0; // = (line_height - height) / 2 when wall line bigger than display height

				cheight = height / renderData[i][1] / cosf(renderData[i][3] - cama);
				if (cheight > height) {
					dop = (cheight - height) / 2;
					cheight = height;
				}

				nw = ((int)renderData[i][2] - 1) % tileww; // wall image number from left border of the tile
				nh = ((int)renderData[i][2] - 1) / tileww; // wall image number from top border of the tile

				r1 = SDL_Rect({ nw * tilew + (int)(tilew * renderData[i][5]), nh * tileh, 1, tileh }); // cutting a line of wall
				r2 = SDL_Rect({ (int)renderData[i][4], (height / 2 - cheight / 2) - dop, 1, cheight + dop * 2 }); // placing a line of wall

				SDL_RenderCopy(renderer, walls, &r1, &r2);
			}
			if (renderData[i][0] == 1) {
				nw = (int)renderData[i][2] % tileSww;
				nh = renderData[i][2] / tileSww;

				r1 = SDL_Rect({ nw * tileSw, nh * tileSh, tileSw, tileSh });
				r2 = SDL_Rect({ (int)renderData[i][3], (int)renderData[i][4], (int)renderData[i][5], (int)renderData[i][5] });

				SDL_RenderCopy(renderer, sprites, &r1, &r2);
			}
		}
	}
	void close() {
		SDL_DestroyTexture(walls);
		SDL_DestroyTexture(sprites);
	}
private:
	int xobj, yobj, xdir, ydir, col, nw, nh, obj, xx, yy, xy, yx, cheight, dop, ssize, shor, sver;
	float far, xfar, yfar, cos, sin, cut, xdel, ydel, fstep, angle, sprite, x, y, sfar, sdir;
	char* tile, * tileS;
	int* level;
	int width, height, levelw, levelh, tilew,
		tileh, tileww, tilewh, tileSw, tileSh,
		tileSww, tileSwh, scolkeyr, scolkeyg, scolkeyb,
		renderDataSize = 0, spriteMapSize, renderMax;
	float** renderData;
	float** spriteMap;
	float* renderDataMem = {};
	float camx, camy, cama, camf;
	SDL_Renderer* renderer;
	SDL_Texture* walls;
	SDL_Texture* sprites;
	SDL_Rect r1, r2;
};