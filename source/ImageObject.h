#pragma once
#include "Object.h"
#include "ImageRenderer.h"

class ImageObject : public Object {
public:
    ImageObject(std::string texturePath, Vector2 sourceOffset, Vector2 sourceSize)
        : Object() {
        transform->size = sourceSize;

        physics->AddCollider(new AABB(sourceOffset, sourceSize * 0.2f));

        renderer = new ImageRenderer(transform, texturePath, sourceOffset, sourceSize);
    }
};
