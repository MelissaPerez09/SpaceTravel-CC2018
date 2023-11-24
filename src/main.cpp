#include "logic.cpp"

int main(int argc, char* argv[]) {

    SDL_Init(SDL_INIT_EVERYTHING);
    std::vector<Model> models;
    glm::vec3 cameraPosition(0.0f, 0.0f, 30.0f); // Mueve la cámara hacia atrás
    glm::vec3 targetPosition(0.0f, 0.0f, 0.0f);   // Coloca el centro de la escena en el origen
    glm::vec3 upVector(0.0f, 1.0f, 0.0f);

    uniforms.view = glm::lookAt(cameraPosition, targetPosition, upVector);

    srand(time(nullptr));

    SDL_Window* window = SDL_CreateWindow("Pixel Drawer", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    int renderWidth, renderHeight;
    SDL_GetRendererOutputSize(renderer, &renderWidth, &renderHeight);

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normal;
    std::vector<Face> faces;
    bool success = loadOBJ("./models/sphere.obj", vertices, normal, faces);
    if (!success) {
        return 1;
    }

    std::vector<Vertex> vertexArray = setupVertexArray(vertices, normal, faces);

    std::vector<glm::vec3> verticesNave;
    std::vector<glm::vec3> normalNave;
    std::vector<Face> facesNave;
    bool success2 = loadOBJ("./models/nave.obj", verticesNave, normalNave, facesNave);
    if (!success2) {
        return 1;
    }

    std::vector<Vertex> vertexArray2 = setupVertexArray(verticesNave, normalNave, facesNave);

    bool running = true;
    SDL_Event event;

    Model model;
    Model model2;
    Model model3;
    Model model4;
    Model model5;
    Model model6;
    Model model7;
    Model model8;

    float rotationAngle = 0.0f; // Inicializa la variable de ángulo de rotación

    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1), glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));

    frameStart = SDL_GetTicks();

    glm::vec3 cameraPosition2(0.0f, 0.0f, 10.0f); // Mueve la cámara hacia atrás
    glm::vec3 targetPosition3(0.0f, 0.0f, 0.0f);   // Coloca el centro de la escena en el origen
    glm::vec3 upVector3(0.0f, 1.0f, 0.0f);

    float moveSpeed = 0.1f;
    float moveSpeed2 = 0.05f;
    float moveSpeedBackForward = 0.1f;
    float moveSpeedLeftRight = 0.05f;
    float xRotate = 1;
    float yRotate = 1;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                // Maneja las teclas presionadas
                switch (event.key.keysym.sym) {
                    case SDLK_w:
                        // Mueve la cámara hacia adelante
                        cameraPosition -= moveSpeedBackForward * glm::normalize(targetPosition - cameraPosition);
                        targetPosition -= moveSpeedBackForward * (targetPosition - cameraPosition);
                        break;
                    case SDLK_s:
                        // Mueve la cámara hacia atrás
                        cameraPosition += moveSpeedBackForward * glm::normalize(targetPosition - cameraPosition);
                        targetPosition += moveSpeedBackForward * (targetPosition - cameraPosition);
                        break;
                    case SDLK_a:
                        // Mover hacia la izquierda
                        cameraPosition += moveSpeed2 * glm::normalize(glm::cross((targetPosition - cameraPosition), upVector3))*5.0f;
                        cameraPosition2 += moveSpeed2 * glm::normalize(glm::cross((targetPosition - cameraPosition), upVector3))*5.0f;
                        targetPosition += moveSpeed2 * glm::normalize(glm::cross((targetPosition - cameraPosition),upVector3))*5.0f;
                        targetPosition3 += moveSpeed2 * glm::normalize(glm::cross((targetPosition - cameraPosition),upVector3))*5.0f;
                        break;
                    case SDLK_d:
                        // Mover hacia la derecha
                        cameraPosition -= moveSpeed2 * glm::normalize(glm::cross((targetPosition - cameraPosition), upVector3))*5.0f;
                        cameraPosition2 -= moveSpeed2 * glm::normalize(glm::cross((targetPosition - cameraPosition), upVector3))*5.0f;
                        targetPosition -= moveSpeed2 * glm::normalize(glm::cross((targetPosition - cameraPosition),upVector3))*5.0f;
                        targetPosition3 -= moveSpeed2 * glm::normalize(glm::cross((targetPosition - cameraPosition),upVector3))*5.0f;
                        break;
                    case SDLK_LEFT:
                        xRotate += 10.0f;
                        break;
                    case SDLK_RIGHT:
                        xRotate -= 10.0f;
                        break;
                    case SDLK_UP:
                        yRotate -= 10.0f;
                        break;
                    case SDLK_DOWN:
                        yRotate += 10.0f;
                        break;

                }
            }

        }
        targetPosition = glm::vec3(5.0f * sin(glm::radians(xRotate)) * cos(glm::radians(yRotate)), 5.0f * sin(glm::radians(yRotate)), -5.0f * cos(glm::radians(xRotate)) * cos(glm::radians(yRotate))) + cameraPosition;
        targetPosition3 = glm::vec3(5.0f * sin(glm::radians(xRotate)) * cos(glm::radians(yRotate)), 5.0f * sin(glm::radians(yRotate)), -5.0f * cos(glm::radians(xRotate)) * cos(glm::radians(yRotate))) + cameraPosition2;


        // Actualizar la matriz de vista con la nueva posición de la cámara
        uniforms.view = glm::lookAt(cameraPosition, targetPosition, upVector);

        models.clear();
        float radius = 3.0f;

        // Calcula la posición en el círculo
        float posX = glm::cos(rotationAngle) * radius;
        float posZ = glm::sin(rotationAngle) * radius;

        glm::mat4 translationMatrix = glm::translate(glm::mat4(1), glm::vec3(posX, 0.0f, posZ));

        float radius2 = 8.0f;

        // Calcula la posición en el círculo
        float posX2 = glm::cos(rotationAngle) * radius2;
        float posZ2 = glm::sin(rotationAngle) * radius2;
        glm::mat4 translationMatrix2 = glm::translate(glm::mat4(1), glm::vec3(posX2, 0.0f, posZ2));

        //SKYBOX
        uniforms.model = createModelMatrixStars();
        uniforms.projection = createProjectionMatrix();
        uniforms.viewport = createViewportMatrix();
        model.uniforms = uniforms;
        model.vertices = vertexArray;
        model.i = skyboxS;
        models.push_back(model);

        //SUN
        uniforms2.model = createModelMatrixEntity(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.5f, 1.5f, 1.5f), glm::vec3(0.0f, 1.0f, 0.0f), 0.5);
        uniforms2.view = glm::lookAt(cameraPosition2, targetPosition3, upVector3);
        uniforms2.viewport = createViewportMatrix();
        uniforms2.projection = createProjectionMatrix();
        model2.uniforms = uniforms2;
        model2.vertices = vertexArray;
        model2.i = star;
        models.push_back(model2);

        //ROCOSO
        uniforms3.model = createModelMatrixEntity(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(0.0f, 1.0f, 0.0f), 0.1) * translationMatrix;
        uniforms3.view = glm::lookAt(cameraPosition2, targetPosition3, upVector3);
        uniforms3.viewport = createViewportMatrix();
        uniforms3.projection = createProjectionMatrix();
        model3.uniforms = uniforms3;
        model3.vertices = vertexArray;
        model3.i = rocoso;
        models.push_back(model3);

        //EARTH
        uniforms4.model = createModelMatrixEntity(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), 0.1) * translationMatrix;
        uniforms4.view = glm::lookAt(cameraPosition2, targetPosition3, upVector3);
        uniforms4.viewport = createViewportMatrix();
        uniforms4.projection = createProjectionMatrix();
        model4.uniforms= uniforms4;
        model4.vertices = vertexArray;
        model4.i = earth;
        models.push_back(model4);

        //MISTERY PLANET
        uniforms5.model = createModelMatrixEntity(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.8f, 0.8f, 0.8f),glm::vec3(0.0f, 1.0f, 0.0f), 0.1) * translationMatrix;
        uniforms5.view = glm::lookAt(cameraPosition2, targetPosition3, upVector3);
        uniforms5.viewport = createViewportMatrix();
        uniforms5.projection = createProjectionMatrix();
        model5.uniforms= uniforms5;
        model5.vertices = vertexArray;
        model5.i = misteryPlanet;
        models.push_back(model5);

        //GAS PLANET
        uniforms6.model = createModelMatrixEntity(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.4f, 0.4f, 0.4f),glm::vec3(0.0f, 1.0f, 0.0f), 0.1) * translationMatrix2;
        uniforms6.view = glm::lookAt(cameraPosition2, targetPosition3, upVector3);
        uniforms6.viewport = createViewportMatrix();
        uniforms6.projection = createProjectionMatrix();
        model6.uniforms= uniforms6;
        model6.vertices = vertexArray;
        model6.i = gasPlanet;
        models.push_back(model6);

        //ORBITAL PLANET
        uniforms7.model = createModelMatrixEntity(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.6f, 0.6f, 0.6f),glm::vec3(0.0f, 1.0f, 0.0f), 0.1) * translationMatrix2;
        uniforms7.view = glm::lookAt(cameraPosition2, targetPosition3, upVector3);
        uniforms7.viewport = createViewportMatrix();
        uniforms7.projection = createProjectionMatrix();
        model7.uniforms= uniforms7;
        model7.vertices = vertexArray;
        model7.i = orbitalPlanet;
        models.push_back(model7);

        //SPACESHIP
        uniforms8.model = createModelMatrixSpaceship(cameraPosition, targetPosition, upVector, xRotate, yRotate);
        uniforms8.view = glm::lookAt(cameraPosition, targetPosition, upVector);
        uniforms8.viewport = createViewportMatrix();
        uniforms8.projection = createProjectionMatrix();
        model8.uniforms = uniforms8;
        model8.vertices = vertexArray2;
        model8.i = spaceship;
        models.push_back(model8);

        SDL_SetRenderDrawColor(renderer, clearColor.r, clearColor.g, clearColor.b, clearColor.a);
        SDL_RenderClear(renderer);

        // Clear z-buffer at the beginning of each frame
        std::fill(zbuffer.begin(), zbuffer.end(), std::numeric_limits<double>::max());

        for (const Model& model : models) {
            render(model.vertices, model.uniforms, model.i);
        }

        //render(vertexArray, uniforms);
        SDL_RenderPresent(renderer);
        frameTime = SDL_GetTicks() - frameStart;
        frameCount++;

        if (frameTime >= 1000) {
            fps = frameCount;
            frameCount = 0;
            frameStart = SDL_GetTicks(); // Reinicia el tiempo de inicio para el siguiente segundo
        }

        std::string fpsText = "FPS: " + std::to_string(fps);
        SDL_SetWindowTitle(window, fpsText.c_str());
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}