#include<iostream>



#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <random>
#include <sstream>


using namespace std;



const int FPS = 60;
const int DELAY_TIME = 1000 / FPS;



class Object 
{
	/*
	Simple class containing information about the object's image,
	dimensions, location, provides functions to move the object.
	*/
	public:
		Object(SDL_Texture* image,int position_x,int position_y); //constructor
		
		SDL_Texture* image_;
		SDL_Rect* rect_;
		
		int position_x_;
		int position_y_;
		
		int size_y_;
		int size_x_;
		
		void MoveX(int distance);
		void MoveY(int distance);
		void PlaceObject(int position_x,int position_y);
	
};


Object::Object(SDL_Texture* image,int position_x,int position_y)  //constructor
 {
	image_ = image;
	 
	position_x_ = position_x;
	position_y_ = position_y;
	 
	//get image size:
	int width,height;
	SDL_QueryTexture(image, NULL, NULL, &width, &height);
	
	size_x_ = width;
	size_y_ = height;
	
	
	rect_ = new SDL_Rect{position_x_, position_y_, width , height}; //update rect

}



void Object::PlaceObject(int position_x,int position_y)
{
	position_x_ = position_x; //place object
	position_y_ = position_y; //place object
	
	rect_ -> x = position_x_; //update rect
	rect_ -> y = position_y_; //update rect
}

void Object::MoveX(int distance)
{

	position_x_ += distance; //move object
	rect_ -> x = position_x_; //update rect
}


void Object::MoveY(int distance)
{
	position_y_ += distance; //move object
	rect_ -> y = position_y_; //update rect
	
}





bool CollisionCheck(Object* obj1, Object* obj2)
{
	//check if objects collide
	
	if (obj1 -> position_x_ < obj2 -> position_x_ + obj2 -> size_x_) //x
	{
		if(obj1 -> position_x_ + obj1->size_x_ > obj2 -> position_x_) //x
		{
			if(obj1 -> position_y_ < obj2 -> position_y_ + obj2 -> size_y_) //y
			{
				if(obj1 -> position_y_ + obj1 -> size_y_ > obj2 -> position_y_) //y
				{
					return true;
				}
				
			}
		}
	}
	
	return false; //no collision
	
}



int RandomNumber(int min, int max)
{
	random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(min,max); // Let's define the distribution interval

	int random_number = dis(gen);
	
	return random_number;
}





void DrawObjects(Object* obj_list,int size,SDL_Renderer* renderer)
{
	//draw objects
	for (int i=0;i<size;i++)
	{
		SDL_RenderCopy(renderer, obj_list[i].image_, nullptr, obj_list[i].rect_);
		SDL_RenderPresent(renderer); // image
	}
	
	
}


int MoveLasers(Object* obj_list,int size,int score)
{
	/*
	 move the lasers down, if the laser reaches the bottom,
	 add more points and move the laser back up.
	 */
	
	
	for (int i = 0;i<size;i++)
	{
		if (obj_list[i].position_y_ > 480)
		{

			obj_list[i].PlaceObject(RandomNumber(0,640),-100*i); //place object up
			score += 1;
		}
		
		obj_list[i].MoveY(5); //move object to down
	}
	return score;

}

void RestartGame(Object* robot, Object* obj_list,int size)
{
	robot->PlaceObject(0,340); //robot to original position
	
	for (int i = 0;i<size;i++)
	{
		obj_list[i].PlaceObject(RandomNumber(0,640),0); //place object up
	} 

	
}




int main(int argc, char *argv[]) //main function
{



	//init variables
	SDL_Renderer* renderer = nullptr;
	SDL_Texture* robot_texture = nullptr;
	SDL_Texture*  laser_texture = nullptr;
	SDL_Event event; // events
	
	
	TTF_Init();//init font system
	TTF_Font* font = TTF_OpenFont("media/font/Roboto-Regular.ttf", 30); //load font
	
	SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);	
	
	Uint32 frameStart, frameTime; //fps limit system
	
	// returns zero on success else non-zero
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("error initializing SDL: %s\n", SDL_GetError());
	}
	SDL_Window* window = SDL_CreateWindow("ROBOT GAME",
									SDL_WINDOWPOS_CENTERED,
									SDL_WINDOWPOS_CENTERED,
									640, 480, 0); //create window



    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); //create renderer
 
	SDL_Surface* robot_surface = IMG_Load("media/robotti.bmp"); //load robot image
	SDL_Surface* laser_surface = IMG_Load("media/laser.bmp"); //load laser image
	
    robot_texture = SDL_CreateTextureFromSurface(renderer, robot_surface); //create texture
    laser_texture = SDL_CreateTextureFromSurface(renderer, laser_surface); //create texture

	//init variables
	int running = 1;
	int score = 0;


	bool move_left = false;
	bool move_right = false;
	bool gameover = false;

	//create robot object:
	Object robot(robot_texture,0,0);

	//laser objects:
	Object laser1(laser_texture ,0,0);
	Object laser2(laser_texture ,0,0);
	Object laser3(laser_texture ,0,0);
	Object laser4(laser_texture ,0,0);

	Object laser_list[4] = {laser1,laser2,laser3,laser4}; //laser objects to list

		
	Object obj_list[5] = {laser1,laser2,laser3,laser4,robot};

	
	RestartGame(&robot,laser_list,4); //set lasers to random positions
	

	

	SDL_Color color = {255, 255, 255}; //text color
	
	//create "game over" text
	SDL_Surface* gameovertext_surface = TTF_RenderText_Solid(font, "GAME OVER", color);
	SDL_Texture* gameovertext_texture = SDL_CreateTextureFromSurface(renderer, gameovertext_surface);
	SDL_Rect gameovertext_rect = {0, 0, 150, 35}; //text position and size
	
	
	SDL_Rect score_text_rect = {350, 0, 150, 30}; //score text position and size

	
	
	while (running) { //main loop	
		frameStart = SDL_GetTicks();


		//create text:
		string text = "Score: ";
		string points_str  = to_string(score);
		text = text + points_str;
		
		//create text:
		SDL_Surface* score_text_surface = TTF_RenderText_Solid(font, text.c_str(), color);
		SDL_Texture* score_text_texture = SDL_CreateTextureFromSurface(renderer, score_text_surface);



		if (gameover == false) 
		{

			// Fill the screen with black color
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);


			DrawObjects(obj_list,5,renderer); //draw objects

			//draw text
			SDL_RenderCopy(renderer, score_text_texture, nullptr, &score_text_rect);
			SDL_RenderPresent(renderer); 

		
			//Update the surface
			SDL_UpdateWindowSurface( window );


			score = MoveLasers(laser_list,4,score); //move lasers
			
			
			
			//collision check
			for(int i=0;i<4;i++)
			{
				if (CollisionCheck(&laser_list[i],&robot))//if collision
				{
					gameover = true;
				}	
			}
			



			if (move_left)
			{
				if (robot.position_x_ > 0) //Let's check that the robot doesn't go over the edge
				{
					robot.MoveX(-5);
				}
			}
			
			if (move_right)
			{
				if (robot.position_x_< 550) // Let's check that the robot doesn't go over the edge
				{
					robot.MoveX(5);
				}
			}

		}
		
		else{ //if gameover
			
			//draw gameover text
			SDL_RenderCopy(renderer, gameovertext_texture, nullptr, &gameovertext_rect);
			SDL_RenderPresent(renderer); 

			//Update the surface
			SDL_UpdateWindowSurface( window );

			
			SDL_Delay(2000); //delay
			RestartGame(&robot,laser_list,4);
			gameover = false;
			score = 0;
		}
	
	



		while ( SDL_PollEvent(&event) != 0 ) { //event loop

				if (event.type == SDL_QUIT) //if window close
				{
					running = 0;
					//close sdl:
					SDL_DestroyTexture(robot_texture);
					SDL_DestroyRenderer(renderer);
					SDL_DestroyWindow(window);
					IMG_Quit();
					SDL_Quit();
					SDL_FreeSurface(score_text_surface);
					SDL_FreeSurface(gameovertext_surface);

					break;
				}
			
			
				if (event.type == SDL_KEYDOWN) { //press down
					if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT) //right arrow key 
					{
						move_right = true;
						break;
					}
					
					else if (event.key.keysym.scancode == SDL_SCANCODE_LEFT) // left arrow key 
					{
						move_left = true;
						break;
					}
				}
					
				if (event.type == SDL_KEYUP) { 
					if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT) //right arrow key 
					{
						move_right = false;
						break;
					}
					
					else if (event.key.keysym.scancode == SDL_SCANCODE_LEFT) // left arrow key 
					{
						move_left = false;
						break;
					}
				}
				
				
			
		}
		


		
		
		
		//fps limit:
		frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < DELAY_TIME) {
            SDL_Delay(DELAY_TIME - frameTime);
        }
		

	}	
    
	return 0;
}
