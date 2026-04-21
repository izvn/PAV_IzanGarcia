#pragma once
#include "Player.h"
#include "InputManager.h"
#include "Spawner.h"
#include "AnimatedImageRenderer.h"
#include <cmath>

class SplatPlayer : public Player {
private:
    float maxSpeed;
    float smoothFactor;
    bool stuck;
    float stuckTimer;
    float stuckDuration;
    bool swatting;
    float swatRadius;
    bool wasLeftClickPressed;

    AnimatedImageRenderer* normalRenderer; 
    AnimatedImageRenderer* swatRenderer;   

    bool isSwattingAnimation; 
    float swatAnimationTimer;    
    const float SWAT_DURATION = 0.5f;

public:
    SplatPlayer()
        : Player("resources/player_swat.png", Vector2(), Vector2(32, 64)),
        maxSpeed(300.0f),
        smoothFactor(0.1f),
        stuck(false),
        stuckTimer(0),
        stuckDuration(1.0f),
        swatting(false),
        swatRadius(50.0f),
        wasLeftClickPressed(false),
        normalRenderer(nullptr),
        swatRenderer(nullptr),
        isSwattingAnimation(false),
        swatAnimationTimer(0.0f)
    {
        transform->position = Vector2(RM.WINDOW_WIDTH / 2.0f, RM.WINDOW_HEIGHT / 2.0f);
        transform->rotation = 225.0f;

        if (renderer) {
            delete renderer;
            renderer = nullptr;
        }

        normalRenderer = new AnimatedImageRenderer(
            transform,
            "resources/player_swat.png", 
            Vector2(0, 0),
            32,   
            64,  
            2,    
            0.5f, 
            true   
        );

        swatRenderer = new AnimatedImageRenderer(
            transform,
            "resources/swat_animation.png", 
            Vector2(0, 0),
            32,  
            64,   
            2,    
            0.2f,   
            true    
        );

        renderer = normalRenderer;
    }

    ~SplatPlayer() override {
        if (renderer == normalRenderer) {
            renderer = nullptr;
        }
        if (renderer == swatRenderer) {
            renderer = nullptr;
        }
        if (normalRenderer) {
            delete normalRenderer;
            normalRenderer = nullptr;
        }
        if (swatRenderer) {
            delete swatRenderer;
            swatRenderer = nullptr;
        }
    }

    void Update() override {
        if (isSwattingAnimation) {
            swatAnimationTimer += TIME.GetDeltaTime();
            if (swatAnimationTimer >= SWAT_DURATION) {
                isSwattingAnimation = false;
                swatAnimationTimer = 0.0f;
                renderer = normalRenderer;
            }
        }

        if (stuck) {
            stuckTimer += TIME.GetDeltaTime();
            if (stuckTimer >= stuckDuration) {
                stuck = false;
                stuckTimer = 0;
                swatting = false;
            }
        }

        if (!stuck) {
            SmoothFollowMouse();
        }
        ConfineToScreen();
        HandleInput();
        Player::Update();
    }

    void HandleInput() override {
        bool isLeftClickPressed = Input.GetLeftClick();

        if (!stuck && !swatting && isLeftClickPressed && !wasLeftClickPressed) {
            swatting = true;
            if (HitEnemy()) {
                swatting = false;
                AM.PlayClip("splat_swat_hit", 0);

                StartSwatAnimation();
            }
            else {
                stuck = true;
                AM.PlayClip("splat_swat_miss", 0);
            }
        }
        wasLeftClickPressed = isLeftClickPressed;
    }

    bool HitEnemy() {
        Scene* sc = SM.GetCurrentScene();
        for (Object* o : sc->GetObjects()) {
            if (Enemy* e = dynamic_cast<Enemy*>(o)) {
                float dx = e->GetTransform()->position.x - transform->position.x;
                float dy = e->GetTransform()->position.y - transform->position.y;
                float dist = sqrt(dx * dx + dy * dy);
                if (dist <= swatRadius) {
                    e->Destroy();
                    AM.PlayClip("splat_enemy_death", 0);
                    return true;
                }
            }
        }
        return false;
    }

    void StartSwatAnimation() {
        if (!isSwattingAnimation) {
            isSwattingAnimation = true;
            swatAnimationTimer = 0.0f;
            renderer = swatRenderer; 
        }
    }

    void SmoothFollowMouse() {
        int mx = Input.GetMouseX();
        int my = Input.GetMouseY();
        Vector2 target((float)mx, (float)my);

        Vector2 direction = target - transform->position;
        transform->position.x += direction.x * smoothFactor;
        transform->position.y += direction.y * smoothFactor;

        float velocityX = direction.x * smoothFactor;
        float velocityY = direction.y * smoothFactor;

        if (fabs(velocityX) > maxSpeed * TIME.GetDeltaTime()) {
            velocityX = copysign(maxSpeed * TIME.GetDeltaTime(), velocityX);
        }
        if (fabs(velocityY) > maxSpeed * TIME.GetDeltaTime()) {
            velocityY = copysign(maxSpeed * TIME.GetDeltaTime(), velocityY);
        }

        physics->SetVelocity(Vector2(velocityX, velocityY));
    }

    void ConfineToScreen() {
        if (transform->position.x < 0) transform->position.x = 0;
        if (transform->position.x > RM.WINDOW_WIDTH) transform->position.x = (float)RM.WINDOW_WIDTH;
        if (transform->position.y < 0) transform->position.y = 0;
        if (transform->position.y > RM.WINDOW_HEIGHT) transform->position.y = (float)RM.WINDOW_HEIGHT;
    }

    void Shoot() override {
    }

    void OnCollisionEnter(Object* other) override {
        if (Bullet* b = dynamic_cast<Bullet*>(other)) {
            LoseLife();
            AM.PlayClip("splat_player_hit", 0);
            b->Destroy();
        }
    }
};
