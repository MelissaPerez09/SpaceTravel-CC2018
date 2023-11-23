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

    bool running = true;
    SDL_Event event;

    Model model;

    float rotationAngle = 0.0f; // Inicializa la variable de ángulo de rotación

    frameStart = SDL_GetTicks();

    glm::vec3 cameraPosition3(0.0f, 0.0f, 10.0f); // Mueve la cámara hacia atrás
    glm::vec3 targetPosition3(0.0f, 0.0f, 0.0f);   // Coloca el centro de la escena en el origen
    glm::vec3 upVector3(0.0f, 1.0f, 0.0f);

    float moveSpeed = 0.1f;
    float moveSpeed2 = 0.05f;
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
                        cameraPosition -= moveSpeed * glm::normalize(targetPosition - cameraPosition);
                        cameraPosition3 -= moveSpeed * glm::normalize(targetPosition - cameraPosition);
                        targetPosition -=  moveSpeed * (targetPosition - cameraPosition);
                        break;
                    case SDLK_s:
                        // Mueve la cámara hacia atrás
                        cameraPosition += moveSpeed * glm::normalize(targetPosition - cameraPosition);
                        cameraPosition3 += moveSpeed * glm::normalize(targetPosition - cameraPosition);
                        targetPosition += moveSpeed * (targetPosition - cameraPosition);
                        targetPosition3 +=  moveSpeed * (targetPosition - cameraPosition);
                        break;
                    case SDLK_a:
                        // Mover hacia la izquierda
                        cameraPosition += moveSpeed2 * glm::normalize(glm::cross((targetPosition - cameraPosition), upVector3))*5.0f;
                        cameraPosition3 += moveSpeed2 * glm::normalize(glm::cross((targetPosition - cameraPosition), upVector3))*5.0f;
                        targetPosition += moveSpeed2 * glm::normalize(glm::cross((targetPosition - cameraPosition),upVector3))*5.0f;
                        targetPosition3 += moveSpeed2 * glm::normalize(glm::cross((targetPosition - cameraPosition),upVector3))*5.0f;
                        break;
                    case SDLK_d:
                        // Mover hacia la derecha
                        cameraPosition -= moveSpeed2 * glm::normalize(glm::cross((targetPosition - cameraPosition), upVector3))*5.0f;
                        cameraPosition3 -= moveSpeed2 * glm::normalize(glm::cross((targetPosition - cameraPosition), upVector3))*5.0f;
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
        targetPosition3 = glm::vec3(5.0f * sin(glm::radians(xRotate)) * cos(glm::radians(yRotate)), 5.0f * sin(glm::radians(yRotate)), -5.0f * cos(glm::radians(xRotate)) * cos(glm::radians(yRotate))) + cameraPosition3;


        // Actualizar la matriz de vista con la nueva posición de la cámara
        uniforms.view = glm::lookAt(cameraPosition, targetPosition, upVector);

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

        uniforms.model = createModelMatrixEntity(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 1.0f, 0.0f), 1);
        uniforms.view = glm::lookAt(cameraPosition3, targetPosition3, upVector3);
        uniforms.viewport = createViewportMatrix();
        uniforms.projection = createProjectionMatrix();

        model.uniforms = uniforms;
        model.vertices = vertexArray;
        //cambiar el id del modelo (gasPlanet, orbitalPlanet, misteryPlanet, star, rocoso, earth)
        model.i = star;
        models.push_back(model);

        glm::vec4 transformedLight = glm::inverse(createModelMatrixStars()) * glm::vec4(L, 0.0f);
        glm::vec3 transformedLightDirection = glm::normalize(glm::vec3(transformedLight));

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