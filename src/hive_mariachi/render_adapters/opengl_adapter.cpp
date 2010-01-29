// Hive Mariachi Engine
// Copyright (C) 2008 Hive Solutions Lda.
//
// This file is part of Hive Mariachi Engine.
//
// Hive Mariachi Engine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Hive Mariachi Engine is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Hive Mariachi Engine. If not, see <http://www.gnu.org/licenses/>.

// __author__    = Jo�o Magalh�es <joamag@hive.pt>
// __version__   = 1.0.0
// __revision__  = $LastChangedRevision$
// __date__      = $LastChangedDate$
// __copyright__ = Copyright (c) 2008 Hive Solutions Lda.
// __license__   = GNU General Public License (GPL), Version 3

#include "stdafx.h"

//#ifdef MARIACHI_PLATFORM_OPENGL

#include "../main/engine.h"
#include "../system/system.h"
#include "../render/render.h"
#include "../render_utils/opengl_glut_window.h"
#include "../render_utils/opengl_win32_window.h"
#include "../render_utils/opengl_cocoa_window.h"
#include "definitions/opengl_adapter_definitions.h"

#include "opengl_adapter.h"

using namespace mariachi;
using namespace mariachi::ui;

extern CameraNode *gCameraNode;

/**
* Constructor of the class.
*/
OpenglAdapter::OpenglAdapter() : RenderAdapter() {
}

/**
* Destructor of the class.
*/
OpenglAdapter::~OpenglAdapter() {
}

void OpenglAdapter::start(void *arguments) {
    int argc = ((int *) arguments)[0];
    char **argv = ((char ***) arguments)[1];
    RenderInformation *renderInformation = ((RenderInformation **) arguments)[2];
    RenderAdapter *renderAdapter = ((RenderAdapter **) arguments)[3];
    Engine *engine = ((Engine **) arguments)[4];

    // creates the arguments map
    std::map<std::string, void *> argumentsMap;

    // sets the arguments in the arguments map
    argumentsMap["argc"] = (void *) argc;
    argumentsMap["argv"] = (void *) argv;
    argumentsMap["render_information"] = (void *) renderInformation;
    argumentsMap["adapter"] = (void *) renderAdapter;
    argumentsMap["engine"] = (void *) engine;

    // resets the frame count
    this->frameCount = 0;

    // retrieves the current clock and sets it as
    // the base clock
    time(&this->baseClock);

    // sets the render information
    this->renderInformation = renderInformation;

    // creates the opengl window
    this->window = new DEFAULT_RENDER_OPENGL_WINDOW_CLASS();

    // starts the opengl window
    this->window->start(&argumentsMap);

    // starts the loop in the opengl window
    this->window->loop(&argumentsMap);

    // cleans the open gl system
    this->clean();
}

void OpenglAdapter::stop(void *arguments) {
}

void OpenglAdapter::init() {
    // clears the depth
    glClearDepth(1.0);

    // sets the depth function
    glDepthFunc(GL_LEQUAL);

    // enables depth testing
    glEnable(GL_DEPTH_TEST);

    // sets the front face
    glFrontFace(GL_CCW);

    // enables blending
    glEnable(GL_BLEND);

    // sets the blending function
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // enables the support for 2d textures
    glEnable(GL_TEXTURE_2D);

    // select clearing (background) color
    glClearColor(0.0, 0.0, 0.0, 0.0);

    // initializes viewing values
    glMatrixMode(GL_PROJECTION);

    // loads the identity matrix
    glLoadIdentity();

    // enters into model view matrix mode
    glMatrixMode(GL_MODELVIEW);
}

void OpenglAdapter::clean() {
}

void OpenglAdapter::display() {
    // updates the frame rate
    this->updateFrameRate();

    // clears all pixels
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // waits for the render information mutex
    MUTEX_LOCK(this->renderInformation->getMutex());

    // displays the 3d scene
    this->display3d();

    // displays the 2d scene
    this->display2d();

    // releases the render information mutex
    MUTEX_UNLOCK(this->renderInformation->getMutex());
}

void OpenglAdapter::resizeScene(int windowWidth, int windowHeight) {
    // prevents a division by zero in case the window is to small
    if(windowHeight == 0) {
        // sets the window height to one
        windowHeight = 1;
    }

    // resets the current viewport and perspective transformation
    glViewport(0, 0, windowWidth, windowHeight);

    // sets the window size
    this->windowSize.width = windowWidth;
    this->windowSize.height = windowHeight;

    // sets the window aspect ratio
    this->windowAspectRatio = (float) this->windowSize.width / (float) this->windowSize.height;

    // calculates the width and height ration
    this->widthRatio = windowWidth / REFERENCE_WIDTH_2D;
    this->heightRatio = windowHeight / REFERENCE_HEIGHT_2D;

    // retrieves the lowest ratio of the both
    this->lowestRatio = this->widthRatio < this->heightRatio ? this->widthRatio : this->heightRatio;

    // retrieves the highest ratio of the both
    this->highestRatio = this->widthRatio > this->heightRatio ? this->widthRatio : this->heightRatio;

    // calculates the best ratio of the both
    this->bestRatio = this->highestRatio >  REFERENCE_WIDTH_2D ? REFERENCE_WIDTH_2D : this->highestRatio;

    this->lowestWidthRevertRatio = this->widthRatio / this->lowestRatio;

    this->lowestHeightRevertRatio = this->heightRatio / this->lowestRatio;
}

void OpenglAdapter::keyPressed(unsigned char key, int x, int y) {
}

inline void OpenglAdapter::setTexture(Texture *texture) {
    // allocates the texture id integer
    GLuint textureId;

    // in case the texture is already rendered in open gl
    if(!(textureId = this->textureTextureIdMap[texture])) {
        // retrieves the texture sizes
        IntSize2d_t textureSize = texture->getSize();

        // retrieves the image buffer
        ImageColor_t *imageBuffer = texture->getImageBuffer();

        // allocation space for the texture
        glGenTextures(1, &textureId);

        // binds the current context to the current texture
        glBindTexture(GL_TEXTURE_2D, textureId);

        // sets the pixel store policy
        glPixelStorei(GL_PACK_ALIGNMENT, 1);

        // loads the texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureSize.width, textureSize.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char *) imageBuffer);

        // sets some texture parameters
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

        // sets the texture id for the current texture in the texture
        // texture id map
        this->textureTextureIdMap[texture] = textureId;
    }
    else {
        // binds the current context to the current texture
        glBindTexture(GL_TEXTURE_2D, textureId);
    }
}

inline time_t OpenglAdapter::clockSeconds() {
    // allocates space for the current clock
    time_t currentClock;

    // sets the current clock
    time(&currentClock);

    // returns the current clock
    return currentClock;
}

inline void OpenglAdapter::updateFrameRate() {
    // increments the frame count
    this->frameCount++;

    // retrieves the current clock
    time_t currentClock = this->clockSeconds();

    // calculates the delta clock
    float deltaClock = (float) difftime(currentClock, this->baseClock);

    // checks if the delta clock is greater than the frame
    // sampling limit
    if(deltaClock > FRAME_SAMPLING_LIMIT) {
        // calculates the frame rate
        float frameRate = (float) frameCount / deltaClock;

        // prints the frame rate
        printf("Frame rate: %.2f\n", frameRate);

        // resets the frame count
        this->frameCount = 0;

        // sets the base clock as the current clock
        this->baseClock = currentClock;
    }
}

inline void OpenglAdapter::display2d() {
    // setup the display 2d
    this->setupDisplay2d();

    // retrieves the render 2d (node)
    Scene2dNode *render2d = this->renderInformation->getRender2d();

    this->renderNode2d(render2d);
}

inline void OpenglAdapter::display3d() {
    // setup the display 3d
    this->setupDisplay3d();

    // in case there is a camera node selected
    if(gCameraNode) {
        // renders the current camera node
        this->renderCameraNode(gCameraNode);
    }

    // retrieves the render (node)
    SceneNode *render = this->renderInformation->getRender();

    std::list<Node *> renderChildrenList = render->getChildrenList();

    std::list<Node *>::iterator renderChildrenListIterator = renderChildrenList.begin();

    while(renderChildrenListIterator != renderChildrenList.end()) {
        Node *node = *renderChildrenListIterator;

        // in case the node is renderable
        if(node->renderable) {
            // @todo change this harcode and make it a switch statement
            // casts the node as model node
            ModelNode *modelNode = (ModelNode *) node;

            // renders the model node
            this->renderModelNode(modelNode);
        }

        // increments the render children list iterator
        renderChildrenListIterator++;
    }
}

inline void OpenglAdapter::setupDisplay2d() {
    // sets the matrix mode to projection
    glMatrixMode(GL_PROJECTION);

    // loads the identity matrix
    glLoadIdentity();

    // setup the orthogonal perspective
    glOrtho(0, (float) this->windowSize.width, (float) this->windowSize.height, 0, -1, 1);

    // sets the matrix mode to model view
    glMatrixMode(GL_MODELVIEW);

    // loads the identity matrix
    glLoadIdentity();

    // scales the projection
    glScalef(this->lowestRatio, this->lowestRatio, 0.0);
}

inline void OpenglAdapter::setupDisplay3d() {
    // sets the matrix mode to projection
    glMatrixMode(GL_PROJECTION);

    // loads the identity matrix
    glLoadIdentity();

    // recalculates the glu perspective
    gluPerspective(45.0, this->windowAspectRatio, 0.3, 1000.0);

    // sets the matrix mode to model view
    glMatrixMode(GL_MODELVIEW);

    // resets the view
    glLoadIdentity();
}

inline void OpenglAdapter::renderCameraNode(CameraNode *cameraNode) {
    // retrieves the position
    Coordinate3d_t &position = cameraNode->getPosition();

    // retrieves the orientation
    Rotation3d_t &orientation = cameraNode->getOrientation();

    // performs the rotation to match the elements orientation
    glRotatef(orientation.angle, orientation.x, orientation.y, orientation.z);

    // moves into the screen
    glTranslatef(-position.x, -position.y, -position.z);
}

inline void OpenglAdapter::renderNode2d(Node *node) {
    // retrieves the node children list
    std::list<Node *> nodeChildrenList = node->getChildrenList();

    // retrieves the node children list iterator
    std::list<Node *>::iterator nodeChildrenListIterator = nodeChildrenList.begin();

    // iterates over all the node children
    while(nodeChildrenListIterator != nodeChildrenList.end()) {
        // tenho de sacar o no
        // tenho de desenhar as bounderies
        // tenho de ver se tem layout se tiver tenho de fazer os devidos calculos

        // retrieves the current node
        Node *currentNode = *nodeChildrenListIterator;

        Coordinate2d_t position;

        // in case the node is renderable
        if(currentNode->renderable) {
            // retrieves the node type
            unsigned int nodeType = currentNode->getNodeType();

            // switches over the node type
            switch(nodeType) {
                // in case it's a component node type
                case UI_COMPONENT_NODE_TYPE:
                    break;

                // in case it's a box component node type
                case UI_BOX_COMPONENT_NODE_TYPE:
                    break;

                // in case it's a view port node type
                case UI_VIEW_PORT_NODE_TYPE:
                    // renders the view port node
                    this->renderViewPortNode((ViewPortNode *) currentNode, (SquareNode *) node);

                    // retrieves the view port position
                    position = this->getRealPosition2d((ViewPortNode *) currentNode, (SquareNode *) node);

                    glPushMatrix();

                    glTranslatef(position.x * this->lowestWidthRevertRatio, position.y * this->lowestWidthRevertRatio, 0.0);

                    this->renderNode2d(currentNode);

                    glPopMatrix();

                    break;

                // in case it's a container node type
                case UI_CONTAINER_NODE_TYPE:
                    break;

                // in case it's a panel node type
                case UI_PANEL_NODE_TYPE:
                    // renders the panel node
                    this->renderPanelNode((PanelNode *) currentNode, (SquareNode *) node);

                    // retrieves the panel position
                    position = this->getRealPosition2d((PanelNode *) currentNode, (SquareNode *) node);

                    glPushMatrix();

                    glTranslatef(position.x, position.y, 0.0);

                    this->renderNode2d(currentNode);

                    glPopMatrix();

                    break;

                // in case it's a button node type
                case UI_BUTTON_NODE_TYPE:
                    // renders the button node
                    this->renderButtonNode((ButtonNode *) currentNode, (SquareNode *) node);

                    break;
            }
        }

        // increments the node children list iterator
        nodeChildrenListIterator++;
    }
}

/**
* Renders a square with the default mapping coordinates.
*
* @param x1 The initial x position.
* @param y1 The initial y position.
* @param x2 The final x position.
* @param y2 The final y position.
*/
inline void OpenglAdapter::renderSquare(float x1, float y1, float x2, float y2) {
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 1.0);
        glVertex2f(x1, y1);
        glTexCoord2f(1.0, 1.0);
        glVertex2f(x2, y1);
        glTexCoord2f(1.0, 0.0);
        glVertex2f(x2, y2);
        glTexCoord2f(0.0, 0.0);
        glVertex2f(x1, y2);
    glEnd();
}

inline void OpenglAdapter::renderModelNode(ModelNode *modelNode) {
    // retrieves the mesh list
    std::vector<Mesh_t *> *meshList = modelNode->getMeshList();

    // retrieves the texture
    Texture *texture = modelNode->getTexture();

    // retrieves the position
    Coordinate3d_t &position = modelNode->getPosition();

    // retrieves the mesh list size
    size_t meshListSize = meshList->size();

    // sets the texture
    this->setTexture(texture);

    // pushes the transformation matrix
    glPushMatrix();

    // puts the element in the screen
    glTranslatef(position.x, position.y, position.z);

    // iterates over all the meshes
    for(unsigned int index = 0; index < meshListSize; index++) {
        // retrieves the current mesh
        Mesh_t *mesh = (*meshList)[index];

        // retrieves the position
        Coordinate3d_t position = mesh->position;

        // retrieves the vertex list
        float *vertexList = mesh->vertexList;

        // retrieves the texture vertex list
        float *textureVertexList = mesh->textureVertexList;

        // retrieves the number of vertices
        unsigned int numberVertices = mesh->numberVertices;

        // switches over the mesh type
        switch(mesh->type) {
            case TRIANGLE:
                break;
            case TRIANGLE_STRIP:
                glBegin(GL_TRIANGLE_STRIP);
                break;
            case TRIANGLE_FAN:
                glBegin(GL_TRIANGLE_FAN);
                break;
        }

        // iterates over all the vertices
        for(unsigned int index = 0; index < numberVertices; index++) {
            // retrieves the texture vertex
            float *textureVertex = &textureVertexList[index * 2];

            // retrieves the texture vertex coordinates
            float textureVertexX = textureVertex[0];
            float textureVertexY = textureVertex[1];

            // retrieves the vertex
            float *vertex = &vertexList[index * 3];

            // retrieves the vertex coordinates
            float vertexX = vertex[0];
            float vertexY = vertex[1];
            float vertexZ = vertex[2];

            // sets the texture coordinates of mapping
            glTexCoord2f(textureVertexX, textureVertexY);

            // sets the vertex coordinates
            glVertex3f(vertexX, vertexY, vertexZ);
        }

        // ends the gl drawing
        glEnd();
    }

    // pops the matrix
    glPopMatrix();
}

inline void OpenglAdapter::renderViewPortNode(ViewPortNode *viewPortNode, SquareNode *targetNode) {
    // retrieves the view port color
    FloatColor_t color = viewPortNode->getColor();

    // retrieves the view port texture
    Texture *texture = viewPortNode->getTexture();

    // sets the texture
    this->setTexture(texture);

    // retrieves the position
    Coordinate2d_t position = this->getRealPosition2d(viewPortNode, targetNode);
    FloatSize2d_t size = this->getRealSize2d(viewPortNode);

    // renders a square with the texture mapping
    this->renderSquare(position.x, position.y, position.x + size.width, position.y + size.height);
}

inline void OpenglAdapter::renderPanelNode(PanelNode *panelNode, SquareNode *targetNode) {
    // retrieves the component size
    FloatSize2d_t size = panelNode->getSize();

    // retrieves the button color
    FloatColor_t color = panelNode->getColor();

    // retrieves the button texture
    Texture *texture = panelNode->getTexture();

    // retrieves the button position reference
    PositionReferenceType_t positionReference = panelNode->getPositionReference();

    // retrieves the position
    Coordinate2d_t position = this->getRealPosition2d(panelNode, targetNode);

    // sets the texture
    this->setTexture(texture);

    // renders a square with the texture mapping
    this->renderSquare(position.x, position.y, position.x + size.width, position.y + size.height);
}

inline void OpenglAdapter::renderButtonNode(ButtonNode *buttonNode, SquareNode *targetNode) {
    // retrieves the button size
    FloatSize2d_t size = buttonNode->getSize();

    // retrieves the button color
    FloatColor_t color = buttonNode->getColor();

    // retrieves the button texture
    Texture *texture = buttonNode->getTexture();

    // retrieves the position
    Coordinate2d_t position = this->getRealPosition2d(buttonNode, targetNode);

    // sets the texture
    this->setTexture(texture);

    // renders a square with the texture mapping
    this->renderSquare(position.x, position.y, position.x + size.width, position.y + size.height);
}

inline Coordinate2d_t OpenglAdapter::getRealPosition2d(SquareNode *squareNode, SquareNode *targetNode) {
    // retrieves the square node size
    FloatSize2d_t size = squareNode->getSize();

    // retrieves the target node position
    Coordinate2d_t targetPosition = targetNode->getPosition();

    // retrieves the target node size
    FloatSize2d_t targetSize = targetNode->getSize();

    // retrieves the square node position
    Coordinate2d_t basePosition = squareNode->getPosition();

    // retrieves the button position reference
    PositionReferenceType_t positionReference = squareNode->getPositionReference();

    // the position value
    Coordinate2d_t position;

    float targetPositionX = targetPosition.x <= 100.0 && targetPosition.x >= 0.0 ? targetPosition.x : 0.0;
    float targetPositionY = targetPosition.y <= 100.0 && targetPosition.y >= 0.0 ? targetPosition.y : 0.0;

    float ratio1Width = targetSize.width <= 100.0 && targetSize.width >= 0.0 ? 100.0 / targetSize.width : 100.0;
    float ratio1Height = targetSize.height <= 100.0 && targetSize.width >= 0.0 ? 100.0 / targetSize.height : 100.0;

    float basePosition_x = basePosition.x / ratio1Width + targetPositionX;
    float basePosition_y = basePosition.y / ratio1Height + targetPositionY;

    // switches over the position reference
    switch(positionReference) {
        case TOP_LEFT_REFERENCE_POSITION:
            position.x = basePosition.x * this->lowestWidthRevertRatio;
            position.y = basePosition.y * this->lowestHeightRevertRatio;

            break;

        case CENTER_REFERENCE_POSITION:
            position.x = basePosition.x * this->lowestWidthRevertRatio - size.width / 2.0;
            position.y = basePosition.y * this->lowestHeightRevertRatio - size.height / 2.0;

            break;
    }

    // returns the position
    return position;
}

inline FloatSize2d_t OpenglAdapter::getRealSize2d(SquareNode *squareNode) {
    // retrieves the square node size
    FloatSize2d_t baseSize = squareNode->getSize();

    FloatSize2d_t size;

    size.width = baseSize.width * this->lowestWidthRevertRatio;
    size.height = baseSize.height * this->lowestHeightRevertRatio;

    // returns the size
    return size;
}

//#endif
