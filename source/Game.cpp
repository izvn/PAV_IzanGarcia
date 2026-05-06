#include "Game.h"
#include "RenderManager.h"
#include "SceneManager.h"
#include "MainMenu.h"
#include "SpaceInvadersGameplay.h"
#include "SplashScreen.h"
#include "TanksGameplay.h"
#include "AudioManager.h"
#include "SplatGameplay.h"
#include "SpriteSelector.h"
#include "RankingScene.h"
#include "SpaceInvadersPauseScene.h"
#include "TanksPauseScene.h"
#include "SplatPauseScene.h"
#include "AsteroidsGameplay.h"
#include "AsteroidsPauseScene.h"
#include "BreakoutGameplay.h"
#include "BreakoutPauseScene.h"
#include "FroggerGameplay.h"
#include "FroggerPauseScene.h"
#include "ArenaGameplay.h"
#include "ArenaPauseScene.h"
#include "ScrollerGameplay.h"
#include "ScrollerPauseScene.h"
#include "GameOverScene.h"

void Game::Init() {
    RM.Init();

    AM.LoadSongs("space_invaders_music");
    AM.LoadSongs("tank_music");
    AM.LoadSongs("splat_music");
    AM.LoadSongs("menu_music");

    AM.LoadClip("space_invaders_player_shoot");
    AM.LoadClip("space_invaders_enemy_death");
    AM.LoadClip("space_invaders_end_wave");
    AM.LoadClip("tank_player_shoot");
    AM.LoadClip("tank_enemy_death");
    AM.LoadClip("tank_enemy_basic_death");
    AM.LoadClip("tank_enemy_seeker_death");
    AM.LoadClip("tank_enemy_shooting_death");
    AM.LoadClip("tank_enemy_aiming_death");
    AM.LoadClip("tank_enemy_exploding_death");
    AM.LoadClip("tank_end_wave");
    AM.LoadClip("tank_player_impact");
    AM.LoadClip("splat_swat_hit");
    AM.LoadClip("splat_swat_miss");
    AM.LoadClip("splat_player_hit");
    AM.LoadClip("splat_enemy_death");
    AM.LoadClip("button_hover");
    AM.LoadClip("button_click");

    RM.LoadTexture("resources/player.png");
    RM.LoadTexture("resources/player_tank.png");
    RM.LoadTexture("resources/tank_base.png");
    RM.LoadTexture("resources/tank_turret.png");
    RM.LoadTexture("resources/player_swat.png");
    RM.LoadTexture("resources/bullet.png");
    RM.LoadTexture("resources/bullet_enemy.png");
    RM.LoadTexture("resources/SpaceInvadersBullet.png");
    RM.LoadTexture("resources/SplashScreen.png");
    RM.LoadTexture("resources/mainmenu.png");

    RM.LoadTexture("resources/basicEnemy.png");
    RM.LoadTexture("resources/enemy.png");
    RM.LoadTexture("resources/enemy_seeker.png");
    RM.LoadTexture("resources/enemy_shooting.png");
    RM.LoadTexture("resources/enemy_aiming.png");
    RM.LoadTexture("resources/enemy_exploding.png");
    RM.LoadTexture("resources/enemy_splat.png");

    RM.LoadTexture("resources/basicEnemy2.png");
    RM.LoadTexture("resources/enemy2.png");
    RM.LoadTexture("resources/enemy_seeker2.png");
    RM.LoadTexture("resources/enemy_shooting2.png");
    RM.LoadTexture("resources/enemy_aiming2.png");
    RM.LoadTexture("resources/enemy_exploding2.png");
    RM.LoadTexture("resources/enemy_splat2.png");

    RM.LoadTexture("resources/basicEnemy3.png");
    RM.LoadTexture("resources/enemy3.png");
    RM.LoadTexture("resources/enemy_seeker3.png");
    RM.LoadTexture("resources/enemy_shooting3.png");
    RM.LoadTexture("resources/enemy_aiming3.png");
    RM.LoadTexture("resources/enemy_exploding3.png");
    RM.LoadTexture("resources/enemy_splat3.png");

    RM.LoadTexture("resources/SpriteBackground1.png");
    RM.LoadTexture("resources/SpriteBackground2.png");
    RM.LoadTexture("resources/SpriteBackground3.png");

    RM.LoadTexture("resources/SpriteEnemies1.png");
    RM.LoadTexture("resources/SpriteEnemies2.png");
    RM.LoadTexture("resources/SpriteEnemies3.png");

    RM.LoadTexture("resources/SpriteVFX1.png");
    RM.LoadTexture("resources/SpriteVFX2.png");
    RM.LoadTexture("resources/SpriteVFX3.png");

    RM.LoadTexture("resources/explosion.png");
    RM.LoadTexture("resources/explosion2.png");
    RM.LoadTexture("resources/explosion3.png");

    RM.LoadTexture("resources/swat_animation.png");

    RM.LoadTexture("resources/shoot_animation.png");
    RM.LoadTexture("resources/shoot_animation2.png");
    RM.LoadTexture("resources/shoot_animation3.png");

    SM.AddScene("SplashScreen", new SplashScreen());
    SM.AddScene("MainMenu", new MainMenu());
    SM.AddScene("SpriteSelector", new SpriteSelector());
    SM.AddScene("RankingScene", new RankingScene());
    SM.AddScene("SpaceInvaders", new SpaceInvadersGameplay());
    SM.AddScene("Tanks", new TanksGameplay());
    SM.AddScene("Splat", new SplatGameplay());
    SM.AddScene("Asteroids", new AsteroidsGameplay());
    SM.AddScene("Breakout", new BreakoutGameplay());
    SM.AddScene("Frogger", new FroggerGameplay());
    SM.AddScene("Arena", new ArenaGameplay());
    SM.AddScene("Scroller", new ScrollerGameplay());
    SM.AddScene("SpaceInvadersPause", new SpaceInvadersPauseScene());
    SM.AddScene("TanksPause", new TanksPauseScene());
    SM.AddScene("SplatPause", new SplatPauseScene());
    SM.AddScene("AsteroidsPause", new AsteroidsPauseScene());
    SM.AddScene("BreakoutPause", new BreakoutPauseScene());
    SM.AddScene("FroggerPause", new FroggerPauseScene());
    SM.AddScene("ArenaPause", new ArenaPauseScene());
    SM.AddScene("ScrollerPause", new ScrollerPauseScene());
    SM.AddScene("GameOver", new GameOverScene());

    SM.InitFirstScene("SplashScreen");
}

void Game::Update() {
    SM.UpdateCurrentScene();
}

void Game::Render() {
    RM.ClearScreen();
    SM.GetCurrentScene()->Render();
    RM.RenderScreen();
}

void Game::Release() {
    SM.GetCurrentScene()->OnExit();
    RM.Release();
}