#pragma once
#include "ImageRenderer.h"
#include <string>

class AnimatedImageRenderer : public ImageRenderer {
private:
    int frameWidth;
    int frameHeight;
    int numFrames;
    float animationSpeed;
    bool loop;
    float currentFrameTime;
    int currentFrame;

public:
    AnimatedImageRenderer(
        Transform* transform,
        std::string resourcePath,
        Vector2 sourceOffset,
        int frameWidth,
        int frameHeight,
        int numFrames,
        float animationSpeed,
        bool loop
    )
        : ImageRenderer(transform, resourcePath, sourceOffset, Vector2(frameWidth, frameHeight)),
        frameWidth(frameWidth),
        frameHeight(frameHeight),
        numFrames(numFrames),
        animationSpeed(animationSpeed),
        loop(loop),
        currentFrameTime(0.0f),
        currentFrame(0)
    {
        sourceRect.x = 0;
        sourceRect.y = 0;
        sourceRect.w = frameWidth;
        sourceRect.h = frameHeight;
    }

    void Update(float dt) override {
        currentFrameTime += dt;
        if (currentFrameTime >= animationSpeed) {
            currentFrameTime = 0;
            currentFrame++;
            if (currentFrame >= numFrames) {
                if (loop) currentFrame = 0;
                else currentFrame = numFrames - 1;
            }
        }

        sourceRect.x = currentFrame * frameWidth;
        sourceRect.y = 0;
        sourceRect.w = frameWidth;
        sourceRect.h = frameHeight;

        ImageRenderer::Update(dt);
    }

    void Render() override {
        ImageRenderer::Render();
    }
};
