#include "Entity.h"


Entity::Entity()
{
    position = glm::vec3(0);
    movement = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);

    speed = 0;

    modelMatrix = glm::mat4(1.0f);
}


bool Entity::CheckSensorCollision(glm::vec3 sensor, Entity* other) {
    if (!isActive || !other->isActive) { return false; }
    float xdist = fabs(sensor.x - other->position.x) - other->width / 2.0f;
    float ydist = fabs(sensor.y - other->position.y) - other->height / 2.0f;
    if (xdist < 0 && ydist < 0) {
        return true;
    }
    return false;
}


bool Entity::CheckCollision(Entity* other) {
    if (!isActive || !other->isActive) { return false; }

    float xdist = fabs(position.x - other->position.x) - (width + other->width) / 2.0f;
    float ydist = fabs(position.y - other->position.y) - (height + other->height) / 2.0f;
    if (xdist < 0 && ydist < 0) {
        return true;
    }
    return false;
}

void Entity::CheckCollisionsY(Entity* objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity* object = &objects[i];
        if (CheckCollision(object))
        {
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
            }
            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
            }
        }
    }
}

void Entity::CheckCollisionsY(Map* map)
{
    // Probes for tiles
    glm::vec3 top = glm::vec3(position.x, position.y + (height / 2), position.z);
    glm::vec3 top_left = glm::vec3(position.x - (width / 2), position.y + (height / 2), position.z);
    glm::vec3 top_right = glm::vec3(position.x + (width / 2), position.y + (height / 2), position.z);

    glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
    glm::vec3 bottom_left = glm::vec3(position.x - (width / 2), position.y - (height / 2), position.z);
    glm::vec3 bottom_right = glm::vec3(position.x + (width / 2), position.y - (height / 2), position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    if (map->IsSolid(top, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }

    if (map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
}

void Entity::CheckCollisionsX(Entity* objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity* object = &objects[i];
        if (CheckCollision(object))
        {
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
            }
            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
            }
        }
    }
}

void Entity::CheckCollisionsX(Map* map)
{
    // Probes for tiles
    glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
    glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    if (map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0) {
        position.x += penetration_x;
        velocity.x = 0;
        collidedLeft = true;
    }

    if (map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) {
        position.x -= penetration_x;
        velocity.x = 0;
        collidedRight = true;
    }
}


void Entity::AIWalker() {
    movement = glm::vec3(-1, 0, 0);
}

void Entity::AIFloater() {
    if (fabs(position.y + 6.2f) >= 0.4) {
        movement.y *= -1;
    }
}

void Entity::AIWaitAndGo(Entity* player) {
    switch (aiState) {
    case IDLE:
        if (glm::distance(position, player->position) < 2.0f) {
            aiState = WALKING;
        }
        break;
    case WALKING:

        if (glm::distance(position, player->position) >= 3.3f) {
            movement = glm::vec3(0);
            aiState = IDLE;
            break;
        }

        if (player->position.x < position.x) {
            movement = glm::vec3(-1, 0, 0);
        }
        else if (player->position.x > position.x) {
            movement = glm::vec3(1, 0, 0);
        }
        else {
            movement = glm::vec3(0);
        }
        break;
    case ATTACKING:
        break;
    }
}

void Entity::AIRover(Map* map) {
    if (fabs(position.x - 12.6f) >= 0.8) {
        movement.x *= -1;
    }

    /*
    bool rightOnPlatform = false;
    bool leftOnPlatform = false;
    sensorRight = glm::vec3(position.x + width / 2 + 0.1f, position.y - height / 2 - 0.1f, 0.0f);
    sensorLeft = glm::vec3(position.x - width / 2 - 0.1f, position.y - height / 2 - 0.1f, 0.0f);
    for (int i = 0; i < platformCount; i++) {
        rightOnPlatform = rightOnPlatform || CheckSensorCollision(sensorRight, &platforms[i]);
        leftOnPlatform = leftOnPlatform || CheckSensorCollision(sensorLeft, &platforms[i]);
    }
    if (!rightOnPlatform || !leftOnPlatform) {
        movement.x = -movement.x;
    }
    */

}


void Entity::AI(Entity* player, Map* map) {
    switch (aiType) {
    case WALKER:
        AIWalker();
        break;
    case WAITANDGO:
        AIWaitAndGo(player);
        break;
    case ROVER:
        AIRover(map);;
        break;
    case FLOATER:
        AIFloater();
        break;
    }

}


void Entity::Update(float deltaTime, Entity* player, Entity* objects, int objectCount, Map* map)
{
    if (!isActive) { return; }

    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;

    if (entityType == ENEMY) {
        AI(player, map);
    }

    if (animIndices != NULL) {
        if (glm::length(movement) != 0) {
            animTime += deltaTime;

            if (animTime >= 0.25f)
            {
                animTime = 0.0f;
                animIndex++;
                if (animIndex >= animFrames)
                {
                    animIndex = 0;
                }
            }
        }
        else {
            animIndex = 0;
        }
    }

    if (jump) {
        jump = false;
        velocity.y += jumpPower;
    }

    velocity.x = movement.x * speed;
    velocity += acceleration * deltaTime;

    position.y += velocity.y * deltaTime; // Move on Y
    CheckCollisionsY(objects, objectCount); // Fix if needed
    CheckCollisionsY(map);

    position.x += velocity.x * deltaTime; // Move on X
    CheckCollisionsX(objects, objectCount); // Fix if needed
    CheckCollisionsX(map);

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}

void Entity::UpdateEnemy(float deltaTime, Entity* player, Map* map)
{
    if (!isActive) { return; }

    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;

    if (entityType == ENEMY) {
        AI(player, map);
    }

    if (animIndices != NULL) {
        if (glm::length(movement) != 0) {
            animTime += deltaTime;

            if (animTime >= 0.25f)
            {
                animTime = 0.0f;
                animIndex++;
                if (animIndex >= animFrames)
                {
                    animIndex = 0;
                }
            }
        }
        else {
            animIndex = 0;
        }
    }

    if (jump) {
        jump = false;
        velocity.y += jumpPower;
    }

    velocity.x = movement.x * speed;
    if (aiType == FLOATER) {
        velocity.y = movement.y * speed;
    }

    velocity += acceleration * deltaTime;

    position.y += velocity.y * deltaTime; // Move on Y
    CheckCollisionsY(map);
    //CheckCollisionsY(objects, objectCount); // Fix if needed

    position.x += velocity.x * deltaTime; // Move on X
    CheckCollisionsX(map);
    //CheckCollisionsX(objects, objectCount); // Fix if needed

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);

    if (movement.x < 0) {
        modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }

}



void Entity::DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index)
{
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;

    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;

    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v };

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };

    glBindTexture(GL_TEXTURE_2D, textureID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Render(ShaderProgram* program) {
    if (!isActive) { return; }

    program->SetModelMatrix(modelMatrix);

    if (animIndices != NULL) {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
    }

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    float ww = width / 2;
    float hh = height / 2;
    float enemyVertices[] = { -ww, -hh, ww, -hh, ww, hh, -ww, -hh, ww, hh, -ww, hh };

    glBindTexture(GL_TEXTURE_2D, textureID);

    if (entityType == ENEMY) {
        glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, enemyVertices);
    }
    else {
        glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    }
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
