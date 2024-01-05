# Rubik's Cube Implementation with OpenGL
### By: Eileen Karin Apaza Coaquira && Gabriel Valdivia

## Description:
This repository contains a comprehensive implementation of a Rubik's Cube using C++ and OpenGL. The project features a fully functional main animation and additional similar ones. The primary objective behind this work was to delve into the fundamental principles of OpenGL and hone skills in computer graphics. This implementation was crafted by referencing the content available on the webpage learnopengl.com.

Para este trabajo se utilizó la librería glm para la aplicación de los fragment y vertex shader y para aplicar las matrices de proyección. 

## Creation of the Rubik's Cube:

First, the structure Point will store the x, y, z coordinates (floats) and the RGB colors (glm::vec3 color).

Then, there is the Cube class, representing each small cube that makes up a Rubik's Cube. For creating this cube, you'll need the center's position, the dimension of each cubelet, its corresponding color for each face, VAO, and VBO to draw each cubelet.

Finally, there's the RubikCube class with nine methods:

The constructor calculates the corresponding centers of each cubelet through the cube's dimension and uses these centers to call the Cube class.
Assigning each cube to its respective layer F, B, L, R, U, D.
Defining the colors for each face of the cubes that make up the Rubik's Cube.
generateBuffers and deleteBuffers methods for creating the VAO and VBO buffers for each cube.
This structure facilitates the creation and representation of a Rubik's Cube, allowing for manipulation and visualization of its components.

It is the solver applied in our Rubik's cube, where we pass as a parameter a std::vectorstd::string of the current shuffle and it returns the solution with the same type of container. It was extracted from hkociemba's repository

The first image depicts our assembled cube. When executed, it displays the assembled Rubik's Cube.

![cubo_rubik_armado](https://github.com/3ileen/CuboRubik-Opengl/assets/65795781/24726c74-9346-4938-a6a9-66192913dc25)


We apply movements to disarrange our cube and then proceed to apply the solver to it

![cubo_rubik_desarmado](https://github.com/3ileen/CuboRubik-Opengl/assets/65795781/b984e847-7514-43d6-a469-aad515586665)
![armado](https://github.com/3ileen/CuboRubik-Opengl/assets/65795781/ebc252e3-c47f-40a9-859b-afd222eef4aa)

Finally, we apply the solver to our Rubik's Cube. Movements are crucial as we'll store them to pass to the solver, allowing it to understand the current state of our cube. In the terminal, it will display the necessary movements to solve the Rubik's Cube.

![armado_2](https://github.com/3ileen/CuboRubik-Opengl/assets/65795781/89e0384f-e36a-4c0f-bee2-f5ba6fbd310b)
![cubo_armado](https://github.com/3ileen/CuboRubik-Opengl/assets/65795781/c496881e-7b39-4dc0-b6c1-10eaf064e39f)


Additional Notes
This project is currently under active development, and there might be areas for improvement. If you have suggestions, ideas, or encounter any issues, please feel free to open an issue or a pull request! You can also reach out to me via my personal email: 21eileenac@gmail.com
