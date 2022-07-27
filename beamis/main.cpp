#include <SDL.h>
#include <beamis.h>


int digits[10][15] = { {1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1},
					   {0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1},
					   {1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1},
					   {1, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1},
					   {1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1}, 
					   {1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1},
					   {1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1},
					   {1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1},
					   {1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1},
					   {1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1} };


int main(int argc, char* args[]) {
	const int width = 320, height = 200;
	int level[100] = { 2, 2, 2, 2, 5, 2, 2, 2, 2, 2,
					   2, 0, 0, 0, 0, 0, 0, 0, 0, 2,
					   2, 0, 0, 0, 0, 0, 0, 4, 0, 2,
					   2, 0, 0, 0, 0, 0, 0, 0, 0, 2,
					   2, 0, 0, 0, 0, 0, 0, 0, 0, 2,
					   2, 0, 0, 0, 0, 0, 0, 0, 0, 2,
					   2, 0, 0, 0, 0, 0, 0, 0, 0, 2,
					   2, 0, 0, 0, 0, 0, 0, 0, 0, 2,
					   2, 0, 0, 0, 0, 0, 0, 0, 0, 2,
					   2, 1, 1, 1, 1, 1, 1, 1, 1, 2 };
	int floor[100] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
					   1, 0, 0, 1, 0, 0, 0, 0, 0, 1,
					   1, 0, 0, 1, 0, 0, 0, 0, 0, 1,
					   1, 0, 0, 1, 0, 0, 1, 1, 1, 1,
					   1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
					   1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
					   1, 1, 1, 1, 0, 0, 1, 0, 0, 1,
					   1, 0, 0, 0, 0, 0, 1, 0, 0, 1,
					   1, 0, 0, 0, 0, 0, 1, 0, 0, 1,
					   1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	int ceil[100] = { 0, 4, 4, 4, 4, 4, 4, 4, 4, 0,
					  0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
					  0, 5, 5, 5, 5, 5, 5, 5, 5, 0,
					  0, 6, 6, 6, 6, 6, 6, 6, 6, 0,
					  0, 6, 6, 6, 6, 6, 6, 6, 6, 0,
					  0, 6, 6, 6, 6, 6, 6, 6, 6, 0,
					  0, 6, 6, 6, 6, 6, 6, 6, 6, 0,
					  0, 5, 5, 5, 5, 5, 5, 5, 5, 0,
					  0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
					  0, 4, 4, 4, 4, 4, 4, 4, 4, 0 };
	float lightmap[100] = { .1, .1, .2, .2, .4, .4, .2, .2, .1, .1,
						 .1, .1, .2, .2, .4, .4, .2, .2, .1, .1,
						 .1, .2, .3, .4, .5, .4, .3, .2, .1, .1,
						 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
						 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
						 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
						 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
						 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
						 1, .5, .5, .5, .5, .5, .5, .5, .5, 1 };

	float sprites[3][3] = { {3, 1.5, 1.5}, {3, 8.5, 1.5}, {0, 2.5, 2.5} };

	int levelw = 10, levelh = 10;

	if (SDL_Init(SDL_INIT_EVERYTHING))
		return 1;

	SDL_Window* window = SDL_CreateWindow("Raycaster", 100, 100, width, height, SDL_WINDOW_SHOWN|SDL_WINDOW_FULLSCREEN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	BeamRenderer raycaster;
	raycaster.setRenderer(renderer, width, height);
	raycaster.setTileWalls("walls.bmp", 32, 32);
	raycaster.setTileSprites("sprites.bmp", 64, 64, 160, 0, 128);
	raycaster.setLevel(level, levelw, levelh);
	raycaster.setLevelSprites(sprites, 3);
	raycaster.setSky("sky.bmp");
	raycaster.setFlats(floor, ceil);
	raycaster.setLights(lightmap);
	float px = 1.5, py = 8.5, pa = 0.0, pf = 1.0;
	raycaster.setCam(px, py, pa, pf);
	raycaster.init(window);
	float speed = 0.05;

	bool run = true;
	int stt, ett = 0, dtt = 0, ctt;
	SDL_Event e;
	const Uint8* keys;

	int mousex, mousey;
	float xm, ym;
	SDL_ShowCursor(SDL_DISABLE);

	while (run) {
		//raycaster.renderWalls();
		//raycaster.renderSprites();
		raycaster.show();
		
		dtt = 0;
		while (dtt < 1) {
			stt = SDL_GetTicks();
			dtt += stt - ett;
			ett = stt;
		}
		ctt = 0;
		dtt = 1000 / dtt;

		xm = 0;
		ym = 0;

		keys = SDL_GetKeyboardState(NULL);
		if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W]) {
			xm += speed * cosf(pa) * 60.f / dtt;
			ym += speed * sinf(pa) * 60.f / dtt;
		}
		if (keys[SDL_SCANCODE_A]) {
			xm += speed * cosf(pa - 1.57) * 60.f / dtt;
			ym += speed * sinf(pa - 1.57) * 60.f / dtt;
		}
		if (keys[SDL_SCANCODE_D]) {
			xm += speed * cosf(pa + 1.57) * 60.f / dtt;
			ym += speed * sinf(pa + 1.57) * 60.f / dtt;
		}
		if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S]) {
			xm -= speed * cosf(pa) * 60.f / dtt;
			ym -= speed * sinf(pa) * 60.f / dtt;
		}
		if (!level[(int)(py + ym + .3) * levelw + (int)(px + xm + .3)] &&
			!level[(int)(py + ym - .3) * levelw + (int)(px + xm + .3)] &&
			!level[(int)(py + ym + .3) * levelw + (int)(px + xm - .3)] &&
			!level[(int)(py + ym - .3) * levelw + (int)(px + xm - .3)]) {
			px += xm;
			py += ym;
		}
		if (!level[(int)(py + ym + .3) * levelw + (int)(px + .3)] &&
			!level[(int)(py + ym - .3) * levelw + (int)(px + .3)] &&
			!level[(int)(py + ym + .3) * levelw + (int)(px - .3)] &&
			!level[(int)(py + ym - .3) * levelw + (int)(px - .3)]) {
			py += ym;
		}
		if (!level[(int)(py + .3) * levelw + (int)(px + xm + .3)] &&
			!level[(int)(py - .3) * levelw + (int)(px + xm + .3)] &&
			!level[(int)(py + .3) * levelw + (int)(px + xm - .3)] &&
			!level[(int)(py - .3) * levelw + (int)(px + xm - .3)]) {
			px += xm;
		}
		if (keys[SDL_SCANCODE_LEFT])
			pa -= speed * 60.f / dtt / 2;
		if (keys[SDL_SCANCODE_RIGHT])
			pa += speed * 60.f / dtt / 2;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)
				run = false;
			if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym == SDLK_ESCAPE)
					run = false;
			}
		}
		
		SDL_GetMouseState(&mousex, &mousey);

		pa += (mousex - width / 2) * 0.001;

		SDL_WarpMouseGlobal(width / 2, height / 2);
		/*
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		while (dtt) {
			for (int i = 0; i < 15; i++) {
				if (digits[dtt % 10][i])
					SDL_RenderDrawPoint(renderer, width - 1 - ctt * 5 - 3 + i % 3, 1 + i / 3);
			}
			ctt++;
			dtt /= 10;
		}*/

		SDL_RenderPresent(renderer);

		//SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		//SDL_RenderClear(renderer);

		raycaster.setCam(px, py, pa, pf);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	return 0;
}
/*
#include <SDL.h>
#include <beamis-1.0.0.0.h>
#include <iostream>

int level[90] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 1, 0, 1,
	1, 0, 0, 0, 0, 0, 2, 0, 1,
	1, 0, 0, 1, 0, 0, 1, 0, 1,
	1, 0, 1, 0, 0, 0, 1, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1 };
float spritemap[][3] = { {0, 1.5, 1.5}, {1, 3.5, 3.5} };
int levelw = 9, levelh = 10;

int main(int argc, char* args[]) {

	int width = 800, height = 600;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		return 1;
	}

	Uint8 scolkeyr = 160, scolkeyg = 0, scolkeyb = 128;
	SDL_Window* window = SDL_CreateWindow("test", 200, 200, width, height, SDL_WINDOW_SHOWN);// | SDL_WINDOW_FULLSCREEN);
	if (window == nullptr) return 1;
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	if (renderer == nullptr) return 1;

	SDL_GameController* controller = nullptr;
	for (int i = 0; i < SDL_NumJoysticks(); i++) {
		if (SDL_IsGameController(i)) {
			controller = SDL_GameControllerOpen(i);
			break;
		}
	}
	bool iscontroller = controller != nullptr;
	std::cout << "controller is " << (iscontroller ? "on" : "off");

	char* path = (char*)"walls.bmp";

	SDL_Surface* image = SDL_LoadBMP(path);
	//SDL_Surface* wall = new SDL_Surface({});
	if (image == nullptr) return 1;
	if (image2 == nullptr) return 1;
	SDL_Rect rect1 = { 0, 0, 1024, 512 };
	SDL_Rect rect2 = { 90, 90, 100, 512 };
	SDL_BlitSurface(image, &rect1, image2, &rect2);
	if (image2 == nullptr) return 1;
	int ww = 32, wh = 32;
	//image = SDL_LoadBMP("numbers.bmp");
	//SDL_Texture* nset = SDL_CreateTextureFromSurface(renderer, image);
	//std::cout << tw << ' ' << th;
	//SDL_FreeSurface(image);
	//SDL_FreeSurface(image2);
	//if (texture == nullptr) return 1;


	float player_a = 0.5f, player_x = 6.0, player_y = 7.0, fov = 1.0;
	float fov_step = fov / 640;
	bool loop = true;


	float far = 0.0, step = 0.1;
	float speed = 0.05;
	int nx, ny, col = 0, cheight;
	int nw, nh;
	int obj = 0;

	int xx = 0, xy = 0, yx = 0, yy = 0, xobj = 0, yobj = 0;
	float xfar = 0.0, yfar = 0.0, cos = 0.0, sin = 0.0, xdel = 0.0, ydel = 0.0, cut = 0.0;
	bool ydir, xdir;
	float sumMotion = 0.0;

	SDL_Event event;

	SDL_Rect r1;
	SDL_Rect r2;

	Uint32 tl = 0;
	int tn, ta = 0;
	float sec;
	const Uint8* keys;

	int rax = 0, lxax = 0, lyax = 0, frames = 0;
	float xm = 0, ym = 0;
	bool show_fps = true;
	int fps_col = 0;
	float fps_delay = 1000 / 1000;

	BeamRenderer beam;
	beam.setRenderer(renderer, width, height);
	beam.setTileWalls(path, ww, wh);
	beam.setTileSprites((char*)"sprites.bmp", 64, 64, scolkeyr, scolkeyg, scolkeyb);
	beam.setLevel(level, levelw, levelh);
	beam.setLevelSprites(spritemap, sizeof(spritemap) / sizeof(float) / 3);
	beam.setCam(player_x, player_y, player_a, fov);
	beam.init(window);

	SDL_Rect floor = { 0, height / 2, width, height / 2 };
	SDL_Rect ceiling = { 0, 0, width, height / 2 };

	while (loop) {

		tn = 0;
		while (tn < fps_delay) {
			ta = SDL_GetTicks();
			tn += ta - tl;
			tl = ta;
			//std::cout << tn << '\n';
		}
		sec = tn / 1000;
		frames = 1000 / tn;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				loop = false;
			if (event.type == SDL_KEYUP) {
				if (event.key.keysym.scancode == SDL_SCANCODE_C) {
					if (!iscontroller) {
						SDL_Init(SDL_INIT_JOYSTICK);
						controller = nullptr;
						for (int i = 0; i < SDL_NumJoysticks(); i++) {
							if (SDL_IsGameController(i)) {
								controller = SDL_GameControllerOpen(i);
								break;
							}
						}
					}
					else
						controller = nullptr;

					iscontroller = controller != nullptr;
					std::cout << "\ncontroller is " << (iscontroller ? "on" : "off");
					break;
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
					loop = false;
				}
			}
		}
		keys = SDL_GetKeyboardState(NULL);
		//cont = SDL_GameControllerEventState(NULL);
		if (keys[SDL_SCANCODE_UP]) {
			xm += cosf(player_a) * speed * tn / 17;
			ym += sinf(player_a) * speed * tn / 17;
		}
		if (keys[SDL_SCANCODE_LEFT])
			player_a -= 0.05 * tn / 17;
		if (keys[SDL_SCANCODE_RIGHT])
			player_a += 0.05 * tn / 17;

		if (iscontroller) {
			//if (event.type == SDL_CONTROLLERBUTTONDOWN) {
			//std::cout << event.cbutton.type << ' ' << (int)event.cbutton.button << '\n';
			if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_START))
				loop = false;
			if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_LEFTSTICK))
				speed = 0.05;
			else
				speed = 0.025;
			rax = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX);
			lyax = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
			lxax = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
			if (rax)
				player_a += (float)rax / 327680 * tn / 17 / 2;
			if (ym == 0 && xm == 0) {
				if (lyax == lxax) {
					xm -= cosf(player_a) * ((float)lyax / 32768 * tn / 17 * speed) / 2;
					ym -= sinf(player_a) * ((float)lyax / 32768 * tn / 17 * speed) / 2;
					xm -= cosf(player_a - 3.14 / 2) * ((float)lxax / 32768 * tn / 17 * speed) / 4;
					ym -= sinf(player_a - 3.14 / 2) * ((float)lxax / 32768 * tn / 17 * speed) / 4;
				}
				else {
					if (lyax) {
						xm -= cosf(player_a) * ((float)lyax / 32768 * tn / 17 * speed);
						ym -= sinf(player_a) * ((float)lyax / 32768 * tn / 17 * speed);
					}
					if (lxax) {
						xm -= cosf(player_a - 3.14 / 2) * ((float)lxax / 32768 * tn / 17 * speed) / 2;
						ym -= sinf(player_a - 3.14 / 2) * ((float)lxax / 32768 * tn / 17 * speed) / 2;
					}
				}
			}
		}
		if (!level[(int)(player_y + ym + .3) * levelw + (int)(player_x + xm + .3)] &&
			!level[(int)(player_y + ym - .3) * levelw + (int)(player_x + xm + .3)] &&
			!level[(int)(player_y + ym + .3) * levelw + (int)(player_x + xm - .3)] &&
			!level[(int)(player_y + ym - .3) * levelw + (int)(player_x + xm - .3)]) {
			player_x += xm;
			player_y += ym;
		}
		if (!level[(int)(player_y + ym + .3) * levelw + (int)(player_x + .3)] &&
			!level[(int)(player_y + ym - .3) * levelw + (int)(player_x + .3)] &&
			!level[(int)(player_y + ym + .3) * levelw + (int)(player_x - .3)] &&
			!level[(int)(player_y + ym - .3) * levelw + (int)(player_x - .3)]) {
			player_y += ym;
		}
		if (!level[(int)(player_y + .3) * levelw + (int)(player_x + xm + .3)] &&
			!level[(int)(player_y - .3) * levelw + (int)(player_x + xm + .3)] &&
			!level[(int)(player_y + .3) * levelw + (int)(player_x + xm - .3)] &&
			!level[(int)(player_y - .3) * levelw + (int)(player_x + xm - .3)]) {
			player_x += xm;
		}
		xm = 0;
		ym = 0;
		SDL_SetRenderDrawColor(renderer, 96, 96, 96, 255);
		SDL_RenderFillRect(renderer, &floor);
		SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
		SDL_RenderFillRect(renderer, &ceiling);
		beam.setCam(player_x, player_y, player_a, fov);
		beam.renderWalls();
		beam.renderSprites();
		beam.show();
		/*
		col = 0;
		for (float angle = player_a - fov / 2; angle < player_a + fov / 2; angle += fov_step) {
			cos = cosf(angle);
			sin = sinf(angle);
			xdir = cos > 0;
			ydir = sin > 0;

			if (sin) {
				ydel = player_y - (int)player_y;
				yy = 0;
				if (ydir)
					while (!yobj) {
						yy += 1;
						yfar = (yy - ydel) / sin;
						yx = player_x + yfar * cos;
						if (yy + player_y >= levelh || yy + player_y < 0 || yx >= levelw || yx < 0)
						{
							yfar = 100.0;
							break;
						}
						yobj = level[(int)(yy + player_y)][yx];
					}
				else
					while (!yobj) {
						yfar = (yy - ydel) / sin;
						yy -= 1;
						yx = player_x + yfar * cos;
						if (yy + player_y >= levelw || yy + player_y < 0 || yx >= levelh || yx < 0)
						{
							yfar = 100.0;
							break;
						}
						yobj = level[(int)(yy + player_y)][yx];
					}
			}
			else
				yfar = 100.0;

			if (cos) {
				xdel = player_x - (int)player_x;
				xx = 0;
				if (xdir)
					while (!xobj) {
						xx += 1;
						xfar = (xx - xdel) / cos;
						xy = player_y + xfar * sin;
						if (xx + player_x >= levelw || xx + player_x < 0 || xy >= levelh || xy < 0)
						{
							xfar = 100.0;
							break;
						}
						xobj = level[xy][(int)(xx + player_x)];
					}
				else
					while (!xobj) {
						xfar = (xx - xdel) / cos;
						xx -= 1;
						xy = player_y + xfar * sin;
						if (xx + player_x >= levelh || xx + player_x < 0 || xy >= levelw || xy < 0)
						{
							xfar = 100.0;
							break;
						}
						xobj = level[xy][(int)(xx + player_x)];
					}
			}
			else
				xfar = 100.0;

			if (xfar < yfar) {
				obj = xobj;
				far = xfar;
				if (xx > 0)
					cut = player_y + sin * far - xy;
				else
					cut = 1 - player_y - sin * far + xy;
			}
			else {
				obj = yobj;
				far = yfar;
				if (yy < 0)
					cut = player_x + cos * far - yx;
				else
					cut = 1 - player_x - cos * far + yx;
			}

			/*
			while (!obj) {
				far += step;
				nx = player_x + far * cosf(angle);
				ny = player_y + far * sinf(angle);
				if (nx < 10 && ny < 10)
					obj = level[ny][nx];
				else
					break;
			}

			if (obj) {
				int dop = 0;
				cheight = height / far / cosf(angle - player_a);
				if (cheight > height) {
					dop = (cheight - height) / 2;
					cheight = height;
				}
				nw = (obj - 1) % tw;
				nh = (obj - 1) / tw;
				r1 = SDL_Rect({ nw * ww + (int)(ww * cut), nh * wh, 1, wh });
				r2 = SDL_Rect({ col, (height / 2 - cheight / 2) - dop, 1, cheight + dop * 2 });

				//SDL_BlitSurface(image, new SDL_Rect({ nw * ww + (int)(ww * cut), nh * wh, 1, wh }), winSurf, new SDL_Rect({ col, (height / 2 - cheight / 2), 1, cheight }));
				SDL_RenderCopy(renderer, texture, &r1, &r2);

				obj = 0;
			}
			col++;
			far = 0;
			xobj = 0;
			yobj = 0;
		}
		*//*
		if (show_fps) {
			while (frames) {
				r1 = SDL_Rect({ frames % 10 * 3, 0, 3, 5 });
				r2 = SDL_Rect({ width - 20 - fps_col * 12, 10, 9, 15 });
				SDL_RenderCopy(renderer, nset, &r1, &r2);
				fps_col += 1;
				frames /= 10;
			}
			fps_col = 0;
		}

		//SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderPresent(renderer);
		//SDL_RenderClear(renderer);
		//SDL_UpdateWindowSurface(window);
		//SDL_Delay(100);
	}

	beam.close();
	SDL_FreeSurface(image);
	//SDL_FreeSurface(wall);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}*/