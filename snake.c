/*******************************************************************************************
*
*   raylib - sample game: snake
*
*   Sample game developed by Ian Eito, Albert Martos and Ramon Santamaria
*
*   This game has been created using raylib v1.3 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2015 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>



#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------
#define SNAKE_LENGTH   256
#define SQUARE_SIZE     31

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct Snake {
	Vector2 position;
	Vector2 size;
	Vector2 speed;
	Color color;
} Snake;

typedef struct Food {
	Vector2 position;
	Vector2 size;
	bool active;
	Color color;
} Food;

typedef struct Obstacle {
	Vector2 position;
	Vector2 size;
	bool active;
	Color color;
} Obstacle;

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
static const int gameScreenWidth = 800;

static const int screenHeight = 450;

static const int screenWidth = 1000;


static int framesCounter = 0;
static bool gameOver = true;
static bool pause = false;
static bool start = true;
static bool personnalise = false;

static bool avecTraverser = false;
static bool avecObstacle = false;


static Food fruit = { 0 };
static Obstacle obstacle = { 0 };

static Snake snake[SNAKE_LENGTH] = { 0 };
static Vector2 snakePosition[SNAKE_LENGTH] = { 0 };
static bool allowMove = false;
static Vector2 offset = { 0 };
static int counterTail = 0;
static int score = 0;
static int bestScore = 0;
static int fps = 30;





//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void InitGame(void);         // Initialize game
static void UpdateGame(void);       // Update game (one frame)
static void DrawGame(void);         // Draw game (one frame)
static void UnloadGame(void);       // Unload game
static void UpdateDrawFrame(void);  // Update and Draw (one frame)

//------------------------------------------------------------------------------------
// Nouvelles fonction implémentées
//------------------------------------------------------------------------------------
void convert(int Nb, char* NombreaEcrire)// Converti un int en char* pour l'affichage
{
	int Puissance, chiffre, i;
	bool HasWriteNumber;
	Puissance = 100000;
	i = 0;
	HasWriteNumber = false;

	while (Puissance != 0)
	{
		chiffre = Nb / Puissance;
		Nb = Nb - Puissance * chiffre;
		if ((chiffre != 0) || (HasWriteNumber == true))
		{
			NombreaEcrire[i] = chiffre + 48;
			i++;
			HasWriteNumber = true;
		}
		Puissance = Puissance / 10;
	}
	if ((i == 0) && (HasWriteNumber == false))
	{
		NombreaEcrire[i] = 48;
		i++;
	}
	NombreaEcrire[i] = '\0';

}
void traverserMur()
{
	if (((snake[0].position.x) > (gameScreenWidth - offset.x)))
		snake[0].position.x = offset.x / 2;

	else if (((snake[0].position.x) < offset.x / 2))
		snake[0].position.x = (gameScreenWidth - offset.x / 2 - SQUARE_SIZE);

	else if (((snake[0].position.y) > (screenHeight - offset.y)))
		snake[0].position.y = offset.y / 2;

	else if (((snake[0].position.y) < offset.y / 2))
		snake[0].position.y = (screenHeight - offset.y / 2 - SQUARE_SIZE);
}
void creerObstacle()
{
	obstacle.active = true;
	obstacle.position = (Vector2){ GetRandomValue(0, (gameScreenWidth / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.x / 2,
		GetRandomValue(0, (screenHeight / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.y / 2 }; // On genere une position aléatoire pour l'obstacle

	for (int i = 0; i < counterTail; i++)
	{
		while (((obstacle.position.x == snake[i].position.x) && (obstacle.position.y == snake[i].position.y)) ||
			((obstacle.position.x == fruit.position.x) && (obstacle.position.y == fruit.position.y))) // On vérifie qu'il ne tombe pas sur un fruit ou sur le serpent
		{
			obstacle.position = (Vector2){ GetRandomValue(0, (gameScreenWidth / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.x / 2,
				GetRandomValue(0, (screenHeight / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.y / 2 };
			i = 0;
		}
	}
}
void collisionFruit()
{
	snake[counterTail].position = snakePosition[counterTail - 1];
	counterTail += 1;
	fruit.active = false;
	obstacle.active = false;
	score += 1;
	if (score >= bestScore) bestScore = score;
}
void personnaliser() {

	ClearBackground(WHITE);
	DrawText("PERSONNALISER", screenWidth / 2 - MeasureText("PERSONNALISER", 30) / 2 - 50, 50, 30, BLACK);

	//Affichage obstacle
	DrawText("Obstacles : ", screenWidth / 4 - MeasureText("SNAKE", 40) / 2 - 50, 150, 30, BLACK);
	DrawText("[O]", screenWidth / 2 + 150, 150, 30, BLACK);
	if(avecObstacle) 	DrawText("Oui ", screenWidth / 2, 150, 30, GREEN);
	else DrawText("Non", screenWidth / 2 , 150, 30, RED);

	//Affichage traverser les murs
	DrawText("Traverser les murs : ", screenWidth / 4 - MeasureText("SNAKE", 40) / 2 - 50, 200, 30, BLACK);
	DrawText("[T]", screenWidth / 2 + 150, 200, 30, BLACK);
	if (avecTraverser) 	DrawText("Oui ", screenWidth / 2, 200, 30, GREEN);
	else DrawText("Non", screenWidth / 2 , 200, 30, RED);

	//Affichage vitesse
	DrawText("Vitesse : ", screenWidth / 4 - MeasureText("SNAKE", 40) / 2 - 50, 250, 30, BLACK);

	const char* fpss = malloc(sizeof(int));
	convert(fps, fpss);
	if(fps<30) DrawText(fpss, screenWidth / 2, 250, 30, BLUE);
	else if(fps>=30 && fps <=60) DrawText(fpss, screenWidth / 2, 250, 30, GREEN);
	else if(fps>60) DrawText(fpss, screenWidth / 2, 250, 30, RED);
	free(fpss);

	//Affichage touches
	DrawText("[M] pour le Menu", screenWidth / 8, screenHeight-50, 20, GRAY);
	DrawText("[ENTREE] pour lancer", screenWidth - MeasureText("[ENTREE] pour lancer", 20)- 125, screenHeight - 50, 20, GRAY);

	if (IsKeyPressed('O')) avecObstacle = !avecObstacle;
	else if (IsKeyPressed('T')) avecTraverser = !avecTraverser;

	else if (IsKeyPressed(KEY_LEFT) && fps > 0) fps -= 5;
	else if (IsKeyPressed(KEY_LEFT) && fps == 0) fps = 0;
	else if (IsKeyPressed(KEY_RIGHT) && fps < 120) fps += 5;

	else if (IsKeyPressed(KEY_SEMICOLON)) // Equivalent de la touche [M] en AZERTY
	{
		start = true;
		personnalise = false;
		gameOver = true;
	}


	else if (IsKeyPressed(KEY_ENTER) && fps > 0 )
	{
		InitGame();
		SetTargetFPS(fps);
		start = false;
		personnalise = false;
	}
	else if (fps == 0)
		DrawText("Veuillez entrer une vitesse superieure a 0", screenWidth / 4, screenHeight - 100, 20, RED);

}
void lireFichierScore()
{
	char* nomFichier = "C:/Users/ismai/Desktop/DossierIsmail/CoursM1/MLOD/LangageC/miniprojet/Snake/score.txt";
	FILE* fichier;
	fichier = fopen(nomFichier, "r");
	if (fichier == NULL)
	{
		printf("Error opening file %s : %d (%s)\n", nomFichier, errno, strerror(errno));
		exit(1);
	}
	fscanf(fichier, "%u\n", &bestScore);
	fclose(fichier);

}
void modifierFichierScore() {
	char* nomFichier = "C:/Users/ismai/Desktop/DossierIsmail/CoursM1/MLOD/LangageC/miniprojet/Snake/score.txt";
	FILE* fichier;
	fichier = fopen(nomFichier, "r+");
	fscanf(fichier, "%u\n", &bestScore);
	if (score > bestScore)
	{
		const char* scoreChar = (const char*)malloc(sizeof(int));
		convert(score, scoreChar);
		fseek(fichier, SEEK_SET, 0);
		fputs(scoreChar, fichier);
		free(scoreChar);
	}
	fclose(fichier);
	score = 0;
}
void menuPrincipal()
{
	ClearBackground(SKYBLUE);

	DrawText("SNAKE", screenWidth / 2 - MeasureText("SNAKE", 40) / 2 - 50, 100, 40, RED);
	DrawText("FACILE :      [F]", screenWidth / 2 - MeasureText("FACILE : [F]", 40) / 2, screenHeight / 2 + 20, 25, GRAY);
	DrawText("INTERMEDIAIRE :   [I]", screenWidth / 2 - MeasureText("INTERMEDIAIRE : [I]", 40) / 2, screenHeight / 2 + 60, 25, GRAY);
	DrawText("DIFFICILE :    [D]", screenWidth / 2 - MeasureText("DIFFICILE :[D]", 40) / 2, screenHeight / 2 + 100, 25, GRAY);
	DrawText("PERSONNALISER : [P]", screenWidth / 2 - MeasureText("PERSONNALISE: [P]", 40) / 2, screenHeight / 2 + 140, 25, GRAY);

	if (IsKeyPressed(KEY_F)) {
		InitGame();
		fps = 45;
		SetTargetFPS(fps);
		avecObstacle = false;
		avecTraverser = true;
		start = false;
	}
	if (IsKeyPressed(KEY_I)) {
		InitGame();
		fps = 45;
		SetTargetFPS(fps);
		avecObstacle = true;
		avecTraverser = true;
		start = false;
	}
	if (IsKeyPressed(KEY_D)) {
		InitGame();
		fps = 60;
		SetTargetFPS(fps);
		avecObstacle = true;
		avecTraverser = false;
		start = false;
	}
	if (IsKeyPressed(KEY_P)) {
		personnalise = true;
		start = false;
	}
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
	// Initialization (Note windowTitle is unused on Android)
	//---------------------------------------------------------
	InitWindow(screenWidth, screenHeight, "Snake");

	start = true;



#if defined(PLATFORM_WEB)
	emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else

	//--------------------------------------------------------------------------------------

	// Main game loop
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		// Update and Draw
		//----------------------------------------------------------------------------------
		UpdateDrawFrame();
		//----------------------------------------------------------------------------------
	}
#endif
	// De-Initialization
	//--------------------------------------------------------------------------------------
	UnloadGame();         // Unload loaded data (textures, sounds, models...)

	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}

//------------------------------------------------------------------------------------
// Module Functions Definitions (local)
//------------------------------------------------------------------------------------

// Initialize game variables
void InitGame(void)
{
	framesCounter = 0;
	gameOver = false;
	pause = false;

	counterTail = 1;
	allowMove = false;

	offset.x = gameScreenWidth % SQUARE_SIZE;
	offset.y = screenHeight % SQUARE_SIZE;

	for (int i = 0; i < SNAKE_LENGTH; i++)
	{
		snake[i].position = (Vector2){ offset.x / 2, offset.y / 2 };
		snake[i].size = (Vector2){ SQUARE_SIZE, SQUARE_SIZE };
		snake[i].speed = (Vector2){ SQUARE_SIZE, 0 };

		if (i == 0) snake[i].color = DARKBLUE;
		else snake[i].color = BLUE;
	}

	for (int i = 0; i < SNAKE_LENGTH; i++)
	{
		snakePosition[i] = (Vector2){ 0.0f, 0.0f };
	}

	fruit.size = (Vector2){ SQUARE_SIZE, SQUARE_SIZE };
	fruit.color = SKYBLUE;
	fruit.active = false;

	obstacle.size = (Vector2){ SQUARE_SIZE, SQUARE_SIZE };
	obstacle.color = RED;
	obstacle.active = false;

	lireFichierScore();

}

// Update game (one frame)
void UpdateGame(void)
{
	if (!gameOver && !start && !personnalise) // en cours de partie
	{
		if (IsKeyPressed('P')) pause = !pause;

		if (!pause)
		{
			// Player control
			if (IsKeyPressed(KEY_RIGHT) && (snake[0].speed.x == 0) && allowMove)
			{
				snake[0].speed = (Vector2){ SQUARE_SIZE, 0 };
				allowMove = false;
			}
			if (IsKeyPressed(KEY_LEFT) && (snake[0].speed.x == 0) && allowMove)
			{
				snake[0].speed = (Vector2){ -SQUARE_SIZE, 0 };
				allowMove = false;
			}
			if (IsKeyPressed(KEY_UP) && (snake[0].speed.y == 0) && allowMove)
			{
				snake[0].speed = (Vector2){ 0, -SQUARE_SIZE };
				allowMove = false;
			}
			if (IsKeyPressed(KEY_DOWN) && (snake[0].speed.y == 0) && allowMove)
			{
				snake[0].speed = (Vector2){ 0, SQUARE_SIZE };
				allowMove = false;
			}

			// Snake movement
			for (int i = 0; i < counterTail; i++) snakePosition[i] = snake[i].position;

			if ((framesCounter % 5) == 0)
			{
				for (int i = 0; i < counterTail; i++)
				{
					if (i == 0)
					{
						snake[0].position.x += snake[0].speed.x;
						snake[0].position.y += snake[0].speed.y;
						allowMove = true;
					}
					else snake[i].position = snakePosition[i - 1];
				}
			}

			// Collision contre mur sans traverserMur
			if (!avecTraverser && (((snake[0].position.x) > (gameScreenWidth - offset.x)) ||
				((snake[0].position.y) > (screenHeight - offset.y)) ||
				(snake[0].position.x < 0) || (snake[0].position.y < 0)))
			{
				gameOver = true;
				modifierFichierScore;
			}
			// Collision contre mur avec traverserMur
			if (((snake[0].position.x) > (gameScreenWidth - offset.x)) ||
				((snake[0].position.y) > (screenHeight - offset.y)) ||
				(snake[0].position.x < 0) || (snake[0].position.y < 0) && avecTraverser)
			{
				traverserMur();
			}



			// Collision with yourself
			for (int i = 1; i < counterTail; i++)
			{
				if ((snake[0].position.x == snake[i].position.x) && (snake[0].position.y == snake[i].position.y)) gameOver = true;
			}

			// Fruit position calculation
			if (!fruit.active)
			{
				fruit.active = true;
				fruit.position = (Vector2){ GetRandomValue(0, (gameScreenWidth / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.x / 2, GetRandomValue(0, (screenHeight / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.y / 2 };

				for (int i = 0; i < counterTail; i++)
				{
					while ((fruit.position.x == snake[i].position.x) && (fruit.position.y == snake[i].position.y))
					{
						fruit.position = (Vector2){ GetRandomValue(0, (gameScreenWidth / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.x / 2, GetRandomValue(0, (screenHeight / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.y / 2 };
						i = 0;
					}
				}
			}


			// Collision avec fruit
			if ((snake[0].position.x < (fruit.position.x + fruit.size.x) && (snake[0].position.x + snake[0].size.x) > fruit.position.x) &&
				(snake[0].position.y < (fruit.position.y + fruit.size.y) && (snake[0].position.y + snake[0].size.y) > fruit.position.y))
			{
				collisionFruit();
			}

			// Calcul de la position de l'obstacle
			if (avecObstacle && !obstacle.active)
			{
				creerObstacle();
			}

			//Collision avec obstacle
			if (avecObstacle && (snake[0].position.x < (obstacle.position.x + obstacle.size.x) && (snake[0].position.x + snake[0].size.x) > obstacle.position.x) &&
				(snake[0].position.y < (obstacle.position.y + obstacle.size.y) && (snake[0].position.y + snake[0].size.y) > obstacle.position.y))
			{
				gameOver = true;
				modifierFichierScore();
			}
			if (IsKeyPressed(KEY_ENTER)) {
				gameOver = true;
				modifierFichierScore();
				start = true;
			}
			if (IsKeyPressed(KEY_SPACE)) {
				SetTargetFPS(80);
			}

			framesCounter++;
		}
	}
	else if (gameOver && !start && !personnalise) { // en fin de partie

	if (IsKeyPressed(KEY_R))
	{
		modifierFichierScore();
		InitGame();
		gameOver = false;
		start = false;
		personnalise = false;
	}
	if (IsKeyPressed(KEY_SEMICOLON)) // Equivalent de la touche [M] en AZERTY
	{
		start = true;
		personnalise = false;
		gameOver = true;
	}

}


}

// Draw game (one frame)
void DrawGame(void)
{
	BeginDrawing();

	ClearBackground(RAYWHITE);

	if (!gameOver && !start) // Pendant la partie
	{

		// Draw grid lines
		for (int i = 0; i < gameScreenWidth / SQUARE_SIZE + 1; i++)
		{
			DrawLineV((Vector2) { SQUARE_SIZE* i + offset.x / 2, offset.y / 2 }, (Vector2) { SQUARE_SIZE* i + offset.x / 2, screenHeight - offset.y / 2 }, LIGHTGRAY);
		}

		for (int i = 0; i < screenHeight / SQUARE_SIZE + 1; i++)
		{
			DrawLineV((Vector2) { offset.x / 2, SQUARE_SIZE* i + offset.y / 2 }, (Vector2) { gameScreenWidth - offset.x / 2, SQUARE_SIZE* i + offset.y / 2 }, LIGHTGRAY);
		}

		// Draw snake
		for (int i = 0; i < counterTail; i++) DrawRectangleV(snake[i].position, snake[i].size, snake[i].color);

		// Draw fruit to pick
		DrawRectangleV(fruit.position, fruit.size, fruit.color);

		// Draw side score
		DrawText("SCORE", screenWidth - 100 - MeasureText("SCORE", 25) / 2, 25, 25, BLUE);

		const char* scoreAffiche = (const char*)malloc(sizeof(int));
		convert(score, scoreAffiche);
		DrawText(scoreAffiche, screenWidth - 100 - MeasureText(scoreAffiche, 25) / 2, 50, 25, BLUE);
		free(scoreAffiche);

		//Draw best score
		DrawText("BEST SCORE", screenWidth - 100 - MeasureText("BEST SCORE", 25) / 2, 100, 25, RED);

		//Draw commands
		DrawText("[P] for pause", screenWidth - 100 - MeasureText("[P] for pause", 20) / 2, screenHeight - 75, 20, GRAY);
		DrawText("[M] for the menu", screenWidth - 100 - MeasureText("[M] for the menu", 20) / 2,screenHeight - 50, 20, GRAY);
		



		const char* bestScoreAffiche = (const char*)malloc(sizeof(int));
		convert(bestScore, bestScoreAffiche);
		DrawText(bestScoreAffiche, screenWidth - 100 - MeasureText(bestScoreAffiche, 25) / 2, 125, 25, RED);
		free(bestScoreAffiche);

		// Draw Obstacle
		if (avecObstacle) DrawRectangleV(obstacle.position, obstacle.size, obstacle.color);




		if (pause) DrawText("GAME PAUSED", gameScreenWidth / 2 - MeasureText("GAME PAUSED", 40) / 2, screenHeight / 2 - 40, 40, GRAY);



	}

	else if (start && gameOver && !personnalise) menuPrincipal(); // Menu principal
	else if (personnalise && gameOver && !start) personnaliser(); // Menu parametres
	else {
		DrawText("Rejouer : [R]", GetScreenWidth() / 2 - MeasureText("Rejouer : [R]", 20) / 2, GetScreenHeight() / 2 - 50, 20, GRAY);
		DrawText("Menu Principal : [M]", GetScreenWidth() / 2 - MeasureText("Menu Principal : [M]", 20) / 2, GetScreenHeight() / 2 + 50, 20, GRAY);
	}

	EndDrawing();
}

// Unload game variables
void UnloadGame(void)
{
	// TODO: Unload all dynamic loaded data (textures, sounds, models...)
}

// Update and Draw (one frame)
void UpdateDrawFrame(void)
{
	UpdateGame();
	DrawGame();
}