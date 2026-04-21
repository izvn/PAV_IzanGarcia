#pragma once
#include "ImageObject.h"
#include "TextRenderer.h"


class TextObject : public ImageObject {
private:
    TextRenderer* tr;

public:
    
    TextObject(std::string texturePath, Vector2 sourceOffset, Vector2 sourceSize, std::string text)
        : ImageObject(texturePath, sourceOffset, sourceSize)
    {
       
        tr = new TextRenderer(transform, text);

     
        if (renderer) {
            delete renderer; 
            renderer = nullptr;
        }
        renderer = tr; 

        tr->SetColor(SDL_Color{ 0xFF, 0xFF, 0xFF, 0xFF });
    }

    void SetText(const std::string& newText) {
        tr->SetText(newText);
    }

    ~TextObject() {
       
        if (tr) {
            renderer = nullptr;
            delete tr;
            tr = nullptr;
        }
    }
};
