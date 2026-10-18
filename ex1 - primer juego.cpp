/*
usa WASD para moverte, R para reiniciar y ENTER para iniciar juego.
este pequeño juego fue hecho por kkaronte como prueba de la librería.

- 17 de octubre de 2026... :)
*/


#include <raylib.h>
#include <raymath.h>
using namespace std;

enum Escena {
	MENU,
	JUGANDO,
	FINAL
};

struct Jugador {
	Vector2 pos;
	Vector2 mov;
	Vector2 t;
	int v;
	bool colision;
	int vida;
	Color color;
};

struct Moneda {
	Vector2 centro;
	int r;
	Color colorC;
	bool colision;
};

void cambiarTiempo (double& tiempo, int& tiempoMax, Color& colorT, Escena& estado) {
	tiempo -= GetFrameTime();
	double porcentaje = tiempo/tiempoMax;
	if (porcentaje > 0.5) {colorT = GREEN;}
	else if (porcentaje > 0.2 && porcentaje <= 0.5) {colorT = ORANGE;} 
	else if (porcentaje > 0 && porcentaje <= 0.2) {colorT = RED;} 
	else if (porcentaje < 0) {estado = FINAL;}
}

void moverJugador(Jugador& j, const int& anchoW, const int& alturaW) {
	if (IsKeyDown(KEY_D)) {j.mov.x++;}
	if (IsKeyDown(KEY_A)) {j.mov.x--;}
	if (IsKeyDown(KEY_W)) {j.mov.y--;}
	if (IsKeyDown(KEY_S)) {j.mov.y++;}
			
	if (Vector2Length(j.mov) > 0) {j.mov = Vector2Normalize(j.mov);}
			
	j.pos.x += j.mov.x * j.v * GetFrameTime();
	j.pos.y += j.mov.y * j.v * GetFrameTime();
	
	j.pos.x = Clamp(j.pos.x, 20, (anchoW-20)-j.t.x);
	j.pos.y = Clamp(j.pos.y, 20, (alturaW-20)-j.t.y);
}

void moverEnemigo (Jugador& e, Jugador& j, const int& anchoW, const int& alturaW) {
	e.mov = {j.pos.x+(j.t.x/2) - e.pos.x, j.pos.y+(j.t.y/2) - e.pos.y};
	e.mov = Vector2Normalize(e.mov);
	e.pos.x += e.mov.x * e.v * GetFrameTime();
	e.pos.y += e.mov.y * e.v * GetFrameTime();
	
	e.pos.x = Clamp(e.pos.x, 20, (anchoW-20)-e.t.x);
	e.pos.y = Clamp(e.pos.y, 20, (alturaW-20)-e.t.y);
}

void monedaAleatoria(Moneda& m, const Jugador& j, Moneda& p, const int aW, const int hW) {
	do {
		m.centro.x = GetRandomValue(m.r+20, aW-(m.r+20));
		m.centro.y = GetRandomValue(m.r+20, hW-(m.r+20));
		m.colision = CheckCollisionCircleRec(m.centro, m.r, 
		{j.pos.x, j.pos.y, j.t.x, j.t.y});
		p.colision = CheckCollisionCircles(m.centro, m.r, p.centro, p.r);
		
	} while (m.colision || p.colision);
				
	m.colorC = {
		(unsigned char)GetRandomValue(0, 255),
		(unsigned char)GetRandomValue(0, 255),
		(unsigned char)GetRandomValue(0, 255),
		255
	};
}

void pocimaAleatoria (Moneda& p, const Jugador& j, Jugador& e, Moneda& m, const int aW, const int hW) {
	do {
		p.centro.x = GetRandomValue(p.r+20, aW-(p.r+20));
		p.centro.y = GetRandomValue(p.r+20, hW-(p.r+20));
		p.colision = CheckCollisionCircleRec(p.centro, p.r, 
				{j.pos.x, j.pos.y, j.t.x, j.t.y});
		e.colision = CheckCollisionCircleRec(p.centro, p.r, 
				{e.pos.x, e.pos.y, e.t.x, e.t.y});
		m.colision = CheckCollisionCircles(p.centro, p.r, m.centro, m.r);
	} while (p.colision || e.colision || m.colision);
}

void dañoEnemigo (Jugador& e, Jugador& j, const int aW, const int hW) {
	do {
		e.pos.x = GetRandomValue(0, aW-e.t.x);
		e.pos.y = GetRandomValue(0, hW-e.t.y);
		e.colision = CheckCollisionRecs({
				j.pos.x, j.pos.y, j.t.x, j.t.y
				}, {
					e.pos.x, e.pos.y, e.t.x, e.t.y
					});
	} while (e.colision);
	Vector2 daño = {20, 20};
	j.t = Vector2Subtract(j.t, daño);
	j.color = {104, 90, 72, 200};
}

void reinicio(Jugador& j, Jugador& e, Moneda& m, Moneda& p, int& puntaje, double& tiempo, int& tiempoMax, double& tiempoPocima, const int& aW, const int& hW, Escena& estado, int& dificultad) {
	puntaje = 0;
	tiempo = tiempoMax;
	tiempoPocima = 0;
	monedaAleatoria(m, j, p, aW, hW);
	dificultad = 0;
	j = {{aW/2 - 40, hW/2}, {0, 0}, {80, 80}, 200, false, 3, BROWN};
	e.pos = {100, 600};
	e.v = 85;
	estado = JUGANDO;
}

int main() {
	const int anchoW = 800, alturaW = 800;

	Jugador jugador = {{anchoW/2 - 40, alturaW/2}, {0, 0}, {80, 80}, 200, false, 3, BROWN};
	Jugador enemigo = {{100, 600}, {0, 0}, {60, 60}, 85, false, 0, RED};
	Moneda moneda = {{0, 0}, 25, YELLOW, false};
	Moneda pocima = {{0, 0}, 25, PURPLE, false};
	
	int puntaje = 0, tiempoMax = 80;
	double tiempo = tiempoMax, tiempoPocima = 0;
	int dificultad = 0;
	bool aparecerPocima = false, powerUp = false;
	Color colorT = GREEN;
	
	moneda.centro.x = GetRandomValue(moneda.r, anchoW-moneda.r);
	moneda.centro.y = GetRandomValue(moneda.r, alturaW-moneda.r);
	
	Escena estado = MENU;
	
	InitWindow(anchoW, alturaW, "Prueba");
	SetTargetFPS(60);
	
	while (!WindowShouldClose()) {
		// codigo		
		if (estado == MENU) {
			if (IsKeyDown(KEY_ENTER)) {
				estado = JUGANDO;
			}
		} else if (estado == JUGANDO) {			
			// tiempo
			cambiarTiempo(tiempo, tiempoMax, colorT, estado);
			
			if (!aparecerPocima) {
				tiempoPocima += GetFrameTime();
			}
			if (tiempoPocima > 4 && powerUp && !aparecerPocima) {
				powerUp = false;
				jugador.v -= 85;
				tiempoPocima = 0;
			}
			else if (tiempoPocima > 7 && !aparecerPocima && !powerUp) {
				pocimaAleatoria(pocima, jugador, enemigo, moneda, anchoW, alturaW);
				tiempoPocima = 0;
				aparecerPocima = true;
			}
			
			// movimiento del jugador
			jugador.mov = {0, 0};
			moverJugador(jugador, anchoW, alturaW);
			
			// movimiento del enemigo
			moverEnemigo(enemigo, jugador, anchoW, alturaW);
			
			// colision con el enemigo
			enemigo.colision = CheckCollisionRecs({
				jugador.pos.x, jugador.pos.y, jugador.t.x, jugador.t.y
				}, {
					enemigo.pos.x, enemigo.pos.y, enemigo.t.x, enemigo.t.y
					});
			
			// colision con la moneda
			moneda.colision = CheckCollisionCircleRec(moneda.centro, moneda.r, 
				{jugador.pos.x, jugador.pos.y, jugador.t.x, jugador.t.y});
			
			// colision con la pocima
			if (aparecerPocima) {
				pocima.colision = CheckCollisionCircleRec(pocima.centro, pocima.r, 
					{jugador.pos.x, jugador.pos.y, jugador.t.x, jugador.t.y});
			}
			
			
			if (moneda.colision)
			{
				monedaAleatoria(moneda, jugador, pocima, anchoW, alturaW);
				puntaje++;
				// aumentar la vida
				if (jugador.vida < 3) {
					jugador.t = Vector2Add(jugador.t, {5, 5});
					if (jugador.t.x > 80) {
						jugador.t = {80, 80};
						jugador.vida = 3;
						jugador.color = BROWN;
					}
				}
				
				// nivel de dificultad
				dificultad++;
				if (dificultad >= 5) {
					jugador.v += 20;
					enemigo.v += 27;
					dificultad = 0;
				}
			}
			if (pocima.colision)
			{
				// aumentar velocidad por 3 segundos
				tiempoPocima = 0;
				aparecerPocima = false;
				pocimaAleatoria(pocima, jugador, enemigo, moneda, anchoW, alturaW);
				powerUp = true;
				jugador.v += 85;
			}
			if (enemigo.colision)
			{
				// daño
				dañoEnemigo(enemigo, jugador, anchoW, alturaW);
				jugador.vida--;
				if (jugador.vida <= 0) {
					estado = FINAL;
				}
			}
		} else if (estado == FINAL) {
			if (IsKeyDown(KEY_R)) {
				// reiniciar
				reinicio(jugador, enemigo, moneda, pocima, puntaje, tiempo, tiempoMax, tiempoPocima, anchoW, alturaW, estado, dificultad);
				aparecerPocima = false;
				tiempoPocima = 0;
				powerUp = false;
			}
		}
				
		// dibujar
		BeginDrawing();
		ClearBackground(DARKPURPLE);
		
		DrawRectangle(20, 20, anchoW-40, alturaW-40, BLACK);
		
		if (estado == MENU) {
			DrawText("PROTOTIPO", anchoW/2 -(MeasureText("PROTOTIPO", 40))/2, alturaW/2 -20, 40, ORANGE);
			DrawText("Presione ENTER...", anchoW/2 -(MeasureText("Presione ENTER...", 40))/2, alturaW/2 +60, 40, GREEN);
		}
		else if (estado == JUGANDO) {
			// moneda
			DrawCircleV(moneda.centro, moneda.r, moneda.colorC);
			for (int rotacion = 0; rotacion < 360; rotacion += 60) {
				DrawRectanglePro({moneda.centro.x, moneda.centro.y, 10, 30}, {5, 15}, rotacion, {
				(unsigned char)(moneda.colorC.r*0.5),
				(unsigned char)(moneda.colorC.g*0.5),
				(unsigned char)(moneda.colorC.b*0.5), 
				255}
				);
			}
			
			// pocima
			if (aparecerPocima) {
				DrawCircleV(pocima.centro, pocima.r, pocima.colorC);
				DrawRectangleV({pocima.centro.x-10, pocima.centro.y-32}, {20, 20}, pocima.colorC );
				DrawCircleV(pocima.centro, pocima.r-8, VIOLET);
			}
			
			// jugador
			DrawRectangleV(jugador.pos, jugador.t, jugador.color);
			DrawRectangleV({jugador.pos.x+(jugador.t.x*0.2), jugador.pos.y+(jugador.t.y*0.2)}, {jugador.t.x*0.6, jugador.t.y*0.6}, {
				40, 41, 35, 255});
			
			// enemigo
			DrawRectangleV(enemigo.pos, enemigo.t, enemigo.color);
			DrawRectanglePro({enemigo.pos.x+13, enemigo.pos.y+15, 45, 10}, {0, 5}, 40, {115, 21, 28, 255});
			DrawRectanglePro({enemigo.pos.x+13, enemigo.pos.y+45, 45, 10}, {0, 5}, -40, {115, 21, 28, 255});
			
			
				
			DrawText(TextFormat("PUNTAJE: %i", puntaje), 30, 90, 40, YELLOW);
			DrawText(TextFormat("TIEMPO: %.2f", tiempo), 30, 40, 40, colorT);
		} else if (estado == FINAL) {
			if (jugador.vida == 0) {
				DrawText("HAS PERDIDO", anchoW/2 -(MeasureText("HAS PERDIDO", 40))/2, alturaW/2 -60, 40, RED);
			} else {
				DrawText("TIEMPO AGOTADO", anchoW/2 -(MeasureText("TIEMPO AGOTADO", 40))/2, alturaW/2 -60, 40, RED);
			}
			DrawText(TextFormat("PUNTAJE: %i", puntaje), anchoW/2 -(MeasureText(TextFormat("PUNTAJE: %i", puntaje), 40))/2, alturaW/2, 40, YELLOW);
			DrawText("Presione R para reiniciar...", anchoW/2 -(MeasureText("Presione R para reiniciar...", 40))/2, alturaW/2 +60, 40, GREEN);
		}
		
		EndDrawing();
	}
	
	CloseWindow();
	
	return 0;
}
