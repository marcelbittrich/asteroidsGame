#include "background.hpp"

bgPoint::bgPoint(int xPos, int yPos)
{
    this->xPos = xPos;
    this->yPos = yPos;
    originPos.x = xPos;
    originPos.y = yPos;
    renderPos.x = originPos.x;
    renderPos.y = originPos.y;
    onOrigin = true;
}

void bgPoint::returnToOrigin(){
    double minVelocity = 0.5;
    double steepness = 0.005;
    double distance; 
    distance = sqrt(pow((originPos.x-xPos),2) + pow((originPos.y-yPos),2));

    std::vector<double> vDistance;
    std::vector<double> nDistance;
    std::vector<double> vChange;
    vDistance.push_back(originPos.x-xPos);
    vDistance.push_back(originPos.y-yPos);
    nDistance.push_back(vDistance[0]/(distance));
    nDistance.push_back(vDistance[1]/(distance));
    
    double getBackVelocity = std::max(minVelocity,(steepness*pow(distance,2)));

    vChange.push_back(nDistance[0] * getBackVelocity);
    vChange.push_back(nDistance[1] * getBackVelocity);

    if (sqrt(pow(vChange[0],2) + pow(vChange[1],2))>distance)
    {
        xPos = originPos.x; 
        yPos = originPos.y;
        renderPos.x = originPos.x;
        renderPos.y = originPos.y;
    }   else {
        xPos += vChange[0];
        yPos += vChange[1];
        renderPos.x = round(xPos);
        renderPos.y = round(yPos);
    }
    vDistance.clear();
    nDistance.clear();
    vChange.clear();
}

void bgPoint::update(Gameobject colObject)
{   

    double distance; 
    distance = sqrt(pow((colObject.midPos[0]) - (xPos),2) + pow((colObject.midPos[1]) - (yPos),2));
    if (distance <= colObject.col_radius)
    {
        std::vector<double> vDistance;
        std::vector<double> nDistance;
        double xDist = xPos - colObject.midPos[0];
        double yDist = yPos - colObject.midPos[1];
        vDistance.push_back(xDist);
        vDistance.push_back(yDist);

        nDistance.push_back(vDistance[0]/(distance));
        nDistance.push_back(vDistance[1]/(distance));

        xPos = colObject.midPos[0] + nDistance[0] * colObject.col_radius;
        yPos = colObject.midPos[1] + nDistance[1] * colObject.col_radius;

        renderPos.x = round(xPos);
        renderPos.y = round(yPos);
    }
}

void bgPoint::render(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], color[3]);
    SDL_RenderDrawPoint(renderer,renderPos.x,renderPos.y);
}

background::background()
{

}

background::background(int windowWidth, int windowHeight, int divider)
{
    this->height = windowHeight;
    this->width = windowWidth;
    this->divider = divider;


    for (int i = 0; i < divider; i++)
    {
        int xPos = width/divider * i + width/divider/2;
        for (int j = 0; j < divider; j++)
        {
            int yPos = height/divider * j + height/divider/2;
            bgPoint newPoint(xPos,yPos); 
            pointCloud.push_back(newPoint);
        } 
    }   

}

void background::update(std::vector<Gameobject>colObjects)
{   
    for (auto object:colObjects)
    {
        //std::cout << object.midPos[0] << " " << object.midPos[1] << std::endl;
        int cellColobject; 
        int intPosX = (int)round(object.midPos[0]);
        int intPosY = (int)round(object.midPos[1]);
        int divX = width/divider;
        int divY = height/divider;
        cellColobject = (int)(intPosX/divX)*divider + (int)(intPosY/divY);//TODO Bug when one Pos is negative

        int b = (int)(object.col_radius/divX);
        int c = (int)(object.col_radius/divY);

        for (int i = -b; i <= +b; i++)
        {
            for (int j = -c; j <= +c; j++)
            {
                int cell = cellColobject + i * 100 + j;
                if (cell >= 0 && cell <= (divider*divider)-1){
                    //pointCloud[cell].color = {0,0,255,255};
                    pointCloud[cell].update(object);
                }
            }
        }
    }
    for (auto &point : pointCloud)
    {
        if (point.renderPos.x != point.originPos.x || point.renderPos.y != point.originPos.y)
        {
            point.onOrigin = false;
        } else
        {
            point.onOrigin = true;
        }
        
        if (!point.onOrigin)
        {   
            point.returnToOrigin();
        }    
    }

}


void background::render(SDL_Renderer *renderer)
{
    for (bgPoint singleBgPoint : pointCloud)
    {
        singleBgPoint.render(renderer);
    }
    
}

//debug
void background::getPointPosition(int vectorPos)
{
    std::cout << "Point: " << vectorPos << "is on: " << pointCloud[vectorPos].xPos << ", " << pointCloud[vectorPos].yPos << std::endl;
}


