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
*   Extension by Ismail Zegour - 31/10/2020
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
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void InitGame(void);         // Initialize game
static void UpdateGame(void);       // Update game (one frame)
static void DrawGame(void);         // Draw game (one frame)
static void UnloadGame(void);       // Unload game
static void UpdateDrawFrame(void);  // Update and Draw (one frame)

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
static bool automatique = false;

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
	if (avecObstacle) 	DrawText("Oui ", screenWidth / 2, 150, 30, GREEN);
	else DrawText("Non", screenWidth / 2, 150, 30, RED);

	//Affichage traverser les murs
	DrawText("Traverser les murs : ", screenWidth / 4 - MeasureText("SNAKE", 40) / 2 - 50, 200, 30, BLACK);
	DrawText("[T]", screenWidth / 2 + 150, 200, 30, BLACK);
	if (avecTraverser) 	DrawText("Oui ", screenWidth / 2, 200, 30, GREEN);
	else DrawText("Non", screenWidth / 2, 200, 30, RED);

	//Affichage vitesse
	DrawText("Vitesse : ", screenWidth / 4 - MeasureText("SNAKE", 40) / 2 - 50, 250, 30, BLACK);
	DrawText("[<]", screenWidth / 2 - 50, 250, 30, BLACK);
	DrawText("[>]", screenWidth / 2 + 50, 250, 30, BLACK);

	const char* fpss = malloc(sizeof(int));
	convert(fps, fpss);
	if (fps < 30) DrawText(fpss, screenWidth / 2, 250, 30, BLUE);
	else if (fps >= 30 && fps <= 60) DrawText(fpss, screenWidth / 2, 250, 30, GREEN);
	else if (fps > 60) DrawText(fpss, screenWidth / 2, 250, 30, RED);
	free(fpss);

	//Affichage touches
	DrawText("[M] pour le Menu", screenWidth / 8, screenHeight - 50, 20, GRAY);
	DrawText("[ENTREE] pour lancer", screenWidth - MeasureText("[ENTREE] pour lancer", 20) - 125, screenHeight - 50, 20, GRAY);

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


	else if (IsKeyPressed(KEY_ENTER) && fps > 0)
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
	char* nomFichier = "score.txt";
	FILE* fichier;
	fichier = fopen(nomFichier, "r");
	if (fichier == NULL)
	{
		printf("Error opening file score.txt ");
		exit(1);
	}
	fscanf(fichier, "%u\n", &bestScore);
	fclose(fichier);

}
void modifierFichierScore() {
	char* nomFichier = "score.txt";
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
	DrawText("ISMAIL ZEGOUR", 50, 10, 20, GRAY);
	DrawText("RAYLIB", screenWidth - 150, 10, 20, GRAY);
	DrawText("SNAKE 2.0", screenWidth / 2 - MeasureText("SNAKE 2.0", 40) / 2, 100, 40, BLACK);
	DrawText("FACILE :      [F]", screenWidth / 2 - MeasureText("FACILE : [F]", 25) / 2, screenHeight / 2 - 20, 25, BLACK);
	DrawText("INTERMEDIAIRE :   [I]", screenWidth / 2 - MeasureText("INTERMEDIAIRE : [I]", 25) / 2, screenHeight / 2 + 20, 25, BLACK);
	DrawText("DIFFICILE :    [D]", screenWidth / 2 - MeasureText("DIFFICILE :[D]", 25) / 2, screenHeight / 2 + 60, 25, BLACK);
	DrawText("PERSONNALISER : [P]", screenWidth / 2 - MeasureText("PERSONNALISE: [P]", 25) / 2, screenHeight / 2 + 100, 25, BLACK);
	DrawText("MODE AUTOMATIQUE :[A]", screenWidth / 2 - MeasureText("MODE AUTOMATIQUE :[A]", 30) / 2, screenHeight / 2 + 140, 30, BLACK);
	DrawText("BEST SCORE", screenWidth - 140 - MeasureText("BEST SCORE", 25) / 2, 200, 25, RED);
	DrawText("Can you beat that ?", screenWidth - 140 - MeasureText("Can you beat that ?", 20) / 2, 280, 20, BLACK);
	lireFichierScore();
	const char* bestScoreAffiche = (const char*)malloc(sizeof(int));
	convert(bestScore, bestScoreAffiche);
	DrawText(bestScoreAffiche, screenWidth - 140 - MeasureText(bestScoreAffiche, 25) / 2, 225, 25, RED);
	free(bestScoreAffiche);

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
	if (IsKeyPressed(KEY_Q)) {
		InitGame();
		fps = 60;
		SetTargetFPS(fps);
		automatique = true;
		avecObstacle = true;
		avecTraverser = false;
		start = false;
	}
}
void moveRight()
{
	if (allowMove) {
		snake[0].speed = (Vector2){ SQUARE_SIZE, 0 };
		allowMove = false;
	}
}
void moveLeft()
{
	if (allowMove) {
		snake[0].speed = (Vector2){ -SQUARE_SIZE, 0 };
		allowMove = false;
	}
}
void moveUp()
{
	if (allowMove) {
		snake[0].speed = (Vector2){ 0, -SQUARE_SIZE };
		allowMove = false;
	}
}
void moveDown()
{
	if (allowMove) {
		snake[0].speed = (Vector2){ 0, SQUARE_SIZE };
		allowMove = false;
	}

}
bool murHaut()
{
	return ((snake[0].position.y - SQUARE_SIZE) < offset.y / 2)
		|| (avecObstacle && snake[0].position.x == obstacle.position.x && snake[0].position.y - SQUARE_SIZE == obstacle.position.y);
}
bool murBas()
{
	return (snake[0].position.y + SQUARE_SIZE) > (screenHeight - offset.y)
		|| (avecObstacle && snake[0].position.x == obstacle.position.x && snake[0].position.y + SQUARE_SIZE == obstacle.position.y);
}
bool murDroit()
{
	return (snake[0].position.x + SQUARE_SIZE) > (gameScreenWidth - offset.x)
		|| (avecObstacle && snake[0].position.x + SQUARE_SIZE == obstacle.position.x && snake[0].position.y == obstacle.position.y);
}
bool murGauche()
{
	return ((snake[0].position.x - SQUARE_SIZE) < offset.x / 2)
		|| (avecObstacle && snake[0].position.x - SQUARE_SIZE == obstacle.position.x && snake[0].position.y == obstacle.position.y);
}

void mouvementAuto()
{
	float depx = fruit.position.x - snake[0].position.x;
	float depy = fruit.position.y - snake[0].position.y;

	// Mouvement à droite
	if (depx > 0 && allowMove)
	{
		bool queueDroite = false;
		bool queueGauche = false;
		bool queueBas = false;
		bool queueHaut = false;
		for (int i = 1; i < counterTail; i++)// On verifie s'il y a sa queue aux alentours
		{
			if (((snake[0].position.x + SQUARE_SIZE == snake[i].position.x) && (snake[0].position.y == snake[i].position.y))) // Si queue a droite
				queueDroite = true;

			if (((snake[0].position.x - SQUARE_SIZE == snake[i].position.x) && (snake[0].position.y == snake[i].position.y))) // Si queue a gauche
				queueGauche = true;

			if (((snake[0].position.x == snake[i].position.x) && (snake[0].position.y + SQUARE_SIZE == snake[i].position.y))) // Si queue en bas
				queueBas = true;

			if (((snake[0].position.x == snake[i].position.x) && (snake[0].position.y - SQUARE_SIZE == snake[i].position.y))) // Si queue a droite
				queueHaut = true;
		}
		if (queueDroite || murDroit()) //Si queue ou obstacle à droite
		{
			if (snake[0].speed.y < 0)// si on montait
			{
				if (queueHaut || murHaut())
					moveLeft();
				else
					allowMove = false;
			}
			if (snake[0].speed.y > 0)// si on descendait
			{
				if (queueBas || murBas())
					moveLeft();
				else
					allowMove = false;
			}
			//Prévision pour éviter les culs de sac
			if (snake[0].speed.x > 0 && depy == 0 && snake[0].position.y - snake[counterTail / 2].position.y > 0) // Si on allait a droite en venant du haut a n-4
			{
				if (queueBas || murBas())
					moveUp();
				else
					moveDown(); // Alors on va essayer de s'eloigner de nos précedentes positions en allant vers le bas en priorité
			}

			if (snake[0].speed.x > 0 && depy == 0) // Si on allait a droite en venant du bas ou de la gauche 
			{
				if (queueHaut || murHaut())
					moveDown();
				else
					moveUp();
			}

			if (depy == 0 && snake[0].speed.x < 0)// Si on est sur la meme ligne et qu'on va à gauche
			{
				if (queueHaut || murHaut())
				{
					if (queueBas || murBas())
						moveLeft();
					else
						moveDown();
				}
				else
					moveUp();
			}

		}
		else
			moveRight();
	}

	// Mouvement à gauche
	if (depx < 0 && allowMove)
	{
		bool queueDroite = false;
		bool queueGauche = false;
		bool queueBas = false;
		bool queueHaut = false;
		for (int i = 1; i < counterTail; i++)// On verifie s'il y a sa queue aux alentours
		{
			if (((snake[0].position.x + SQUARE_SIZE == snake[i].position.x) && (snake[0].position.y == snake[i].position.y))) // Si queue a droite
				queueDroite = true;

			if (((snake[0].position.x - SQUARE_SIZE == snake[i].position.x) && (snake[0].position.y == snake[i].position.y))) // Si queue a gauche
				queueGauche = true;

			if (((snake[0].position.x == snake[i].position.x) && (snake[0].position.y + SQUARE_SIZE == snake[i].position.y))) // Si queue en bas
				queueBas = true;

			if (((snake[0].position.x == snake[i].position.x) && (snake[0].position.y - SQUARE_SIZE == snake[i].position.y))) // Si queue a droite
				queueHaut = true;
		}
		if (queueGauche || murGauche()) //Si queue ou obstacle à gauche
		{
			if (snake[0].speed.y < 0)// si on montait
			{
				if (queueHaut || murHaut())
					moveRight();
				else
					moveUp();
			}
			if (snake[0].speed.y > 0)// si on descendait
			{
				if (queueBas || murBas())
					moveRight();
				else
					moveDown();
			}

			if (snake[0].speed.x < 0 && depy == 0 && snake[0].position.y - snake[counterTail / 2].position.y > 0) // Si on allait a gauche en venant du haut 
			{
				if (queueBas || murBas())
					moveUp();
				else
					moveDown(); // Alors on va essayer de s'eloigner de nos précedentes positions en allant vers le bas en priorité
			}

			if (snake[0].speed.x < 0 && depy == 0)// Si on allait a gauche en venant du bas ou de la droite a n-4
			{
				if (queueHaut || murHaut())
					moveDown();
				else
					moveUp();
			}

			if (depy == 0 && snake[0].speed.x > 0)// Si on est sur la meme ligne et qu'on va à droite
			{
				if (queueHaut || murHaut())
				{
					if (queueBas || murBas())
						moveRight();
					else
						moveDown();
				}
				else
					moveUp();
			}

		}
		else
			moveLeft();
	}

	// Mouvement en bas
	if (depy > 0 && allowMove)
	{
		bool queueDroite = false;
		bool queueGauche = false;
		bool queueBas = false;
		bool queueHaut = false;
		for (int i = 1; i < counterTail; i++)// On verifie s'il y a sa queue aux alentours
		{
			if (((snake[0].position.x + SQUARE_SIZE == snake[i].position.x) && (snake[0].position.y == snake[i].position.y))) // Si queue a droite
				queueDroite = true;

			if (((snake[0].position.x - SQUARE_SIZE == snake[i].position.x) && (snake[0].position.y == snake[i].position.y))) // Si queue a gauche
				queueGauche = true;

			if (((snake[0].position.x == snake[i].position.x) && (snake[0].position.y + SQUARE_SIZE == snake[i].position.y))) // Si queue en bas
				queueBas = true;

			if (((snake[0].position.x == snake[i].position.x) && (snake[0].position.y - SQUARE_SIZE == snake[i].position.y))) // Si queue a droite
				queueHaut = true;
		}
		if (queueBas || murBas()) //Si queue ou obstacle en bas
		{
			if (snake[0].speed.x < 0)// si on allait a gauche
			{
				if (queueGauche || murGauche())
					moveUp();
				else
					allowMove = false;
			}
			if (snake[0].speed.x > 0)// si on allait a droite
			{
				if (queueDroite || murDroit())
					moveUp();
				else
					allowMove = false;
			}



			// Prevision ( pour éviter les culs de sac)
			if (snake[0].speed.y > 0 && depx == 0 && snake[0].position.x - snake[counterTail / 2].position.x < 0)// Si on allait en bas et que l'on vient de la droite (verification etat n-10 arbitrairement)
			{
				if (queueGauche || murGauche())
					moveRight();
				else
					moveLeft();// Alors on va essayer de s'eloigner de nos précedentes positions en allant vers la gauche en priorité
			}

			if (snake[0].speed.y > 0 && depx == 0)// Sinon on vient de la gauche ou d'en haut
			{
				if (queueDroite || murDroit())
					moveLeft();
				else
					moveRight();
			}
			if (depx == 0 && snake[0].speed.y < 0)// Si on est sur la meme colonne et qu'on va dans le sens contraire
			{
				if (queueDroite || murDroit())
				{
					if (queueGauche || murGauche())
						moveDown();
					else
						moveLeft();
				}
				else
					moveRight();
			}

		}
		else
			moveDown();
	}

	// Mouvement en haut
	if (depy < 0 && allowMove)
	{
		bool queueDroite = false;
		bool queueGauche = false;
		bool queueBas = false;
		bool queueHaut = false;
		for (int i = 1; i < counterTail; i++)// On verifie s'il y a sa queue aux alentours
		{
			if (((snake[0].position.x + SQUARE_SIZE == snake[i].position.x) && (snake[0].position.y == snake[i].position.y))) // Si queue a droite
				queueDroite = true;

			if (((snake[0].position.x - SQUARE_SIZE == snake[i].position.x) && (snake[0].position.y == snake[i].position.y))) // Si queue a gauche
				queueGauche = true;

			if (((snake[0].position.x == snake[i].position.x) && (snake[0].position.y + SQUARE_SIZE == snake[i].position.y))) // Si queue en bas
				queueBas = true;

			if (((snake[0].position.x == snake[i].position.x) && (snake[0].position.y - SQUARE_SIZE == snake[i].position.y))) // Si queue a droite
				queueHaut = true;
		}
		if (queueHaut || murHaut()) //Si queue ou obstacle en haut
		{
			if (snake[0].speed.x < 0)// si on allait a gauche
			{
				if (queueGauche || murGauche())
					moveDown();
				else
					allowMove = false;
			}
			if (snake[0].speed.x > 0)// si on allait a droite
			{
				if (queueDroite || murDroit())
					moveDown();
				else
					allowMove = false;;
			}
			// Prevision ( pour éviter les culs de sac)
			if (snake[0].speed.y < 0 && depx == 0 && snake[0].position.x - snake[counterTail / 2].position.x < 0)// Si on allait en haut et que l'on vient de la droite (verification a la moitié arbitrairement)
			{
				if (queueGauche || murGauche())
					moveRight();
				else
					moveLeft();// Alors on va essayer de s'eloigner de nos précedentes positions en allant vers la gauche en priorité
			}

			if (snake[0].speed.y < 0 && depx == 0)// Si on allait en haut 
			{
				if (queueDroite || murDroit())
					moveLeft();
				else
					moveRight();
			}

			if (depx == 0 && snake[0].speed.y > 0)// Si on est sur la meme colonne et qu'on va dans le sens contraire
			{
				if (queueDroite || murDroit())
				{
					if (queueGauche || murGauche())
						moveDown();
					else
						moveLeft();
				}
				else
					moveRight();
			}
		}
		else
			moveUp();
	}


}
void mouvementManuel()
{
	if (IsKeyPressed(KEY_RIGHT) && (snake[0].speed.x == 0))
		moveRight();

	if (IsKeyPressed(KEY_LEFT) && (snake[0].speed.x == 0))
		moveLeft();

	if (IsKeyPressed(KEY_UP) && (snake[0].speed.y == 0))
		moveUp();

	if (IsKeyPressed(KEY_DOWN) && (snake[0].speed.y == 0))
		moveDown();
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
			mouvementManuel();
			if (automatique) mouvementAuto();


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
			if (IsKeyPressed(KEY_SEMICOLON))// Equivalent de la touche [M] en AZERTY
			{
				gameOver = true;
				modifierFichierScore();
				start = true;
                automatique=false;
			}
			if (IsKeyPressed(KEY_W))
			{

				fps += 5;
				SetTargetFPS(fps);
			}
			if (IsKeyPressed(KEY_Q))
			{
				if (fps > 5)fps -= 5;
				SetTargetFPS(fps);
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
            automatique=false;
		}
		if (IsKeyPressed(KEY_SEMICOLON)) // Equivalent de la touche [M] en AZERTY
		{
			modifierFichierScore();
			start = true;
			personnalise = false;
            automatique=false;
 
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
		const char* bestScoreAffiche = (const char*)malloc(sizeof(int));
		convert(bestScore, bestScoreAffiche);
		DrawText(bestScoreAffiche, screenWidth - 100 - MeasureText(bestScoreAffiche, 25) / 2, 125, 25, RED);
		free(bestScoreAffiche);

		//Draw commands
		DrawText("[P] for pause", screenWidth - 100 - MeasureText("[P] for pause", 20) / 2, screenHeight - 75, 20, GRAY);
		DrawText("[M] for the menu", screenWidth - 100 - MeasureText("[M] for the menu", 20) / 2, screenHeight - 50, 20, GRAY);


		//Draw FPS
		DrawText("[A]- VITESSE +[Z]", screenWidth - 100 - MeasureText("[A]- VITESSE +[Z]", 20) / 2, 200, 20, BLACK);
		const char* fpss = malloc(sizeof(int));
		convert(fps, fpss);
		DrawText(fpss, screenWidth - 100 - MeasureText(fpss, 20) / 2, 220, 20, BLACK);
		free(fpss);

		// Draw traverser
		DrawText("Traverser les murs :", screenWidth - 100 - MeasureText("Traverser les murs :", 20) / 2, 300, 20, BLACK);
		if (avecTraverser) DrawText("Oui", screenWidth - 100 - MeasureText("Oui", 20) / 2, 330, 20, GREEN);
		if (!avecTraverser)DrawText("Non", screenWidth - 100 - MeasureText("Non", 20) / 2, 330, 20, RED);


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
