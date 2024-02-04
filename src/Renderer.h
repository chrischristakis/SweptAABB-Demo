#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Rect.h"
#include "Shader.h"

struct Renderer {

	const unsigned int LINE_BUFFER_SIZE = sizeof(float) * 4;  // Size of the allocated line buffer, 4 verts = 1 line.

	unsigned int rectVAO, rectVBO, rectEBO;
	unsigned int lineVAO, lineVBO;

	glm::mat4 projection = glm::mat4(1.0f);

	Renderer(int windowWidth, int windowHeight) {
		float rectVerts[] = {
			-0.5f, -0.5f,
			 0.5f, -0.5f,
			 0.5f,  0.5f,
			-0.5f,  0.5f
		};

		unsigned char indexList[] = {
			0, 1, 2,
			2, 3, 0
		};

		// Rect data
		glGenBuffers(1, &rectVBO);
		glGenBuffers(1, &rectEBO);
		glGenVertexArrays(1, &rectVAO);

		glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
		glBindVertexArray(rectVAO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(rectVerts), rectVerts, GL_STATIC_DRAW);
		
		// Index list
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rectEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexList), indexList, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)(0));
		glEnableVertexAttribArray(0);

		// Unbind
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rectEBO); // Unbind after VAO, else VAO will forget the EBO

		// Line data (Empty at first, populated and altered during draw call)
		glGenBuffers(1, &lineVBO);
		glGenVertexArrays(1, &lineVAO);

		glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
		glBindVertexArray(lineVAO);
		glBufferData(GL_ARRAY_BUFFER, LINE_BUFFER_SIZE, 0, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)(0));
		glEnableVertexAttribArray(0);

		// Unbind
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Set up projection (x+ right, y+ down the screen)
		projection = glm::ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f, 0.0f, 100.0f);
	}

	void RenderHollowRect(Rect& rect, Shader& shader) {
		shader.Use();

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(rect.x + (rect.w / 2.0f), rect.y + (rect.h / 2.0f), 0.0f));
		model = glm::scale(model, glm::vec3(rect.w, rect.h, 1.0f));

		glm::mat4 mvp = projection * model;
		shader.SetMatrix4f("mvp", mvp);
		shader.SetVec3f("color", rect.color);

		glBindVertexArray(rectVAO);
		glDrawArrays(GL_LINE_LOOP, 0, 4);
	}
	
	void RenderRect(Rect& rect, Shader& shader) {
		shader.Use();

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(rect.x + (rect.w / 2.0f), rect.y + (rect.h / 2.0f), 0.0f));
		model = glm::scale(model, glm::vec3(rect.w, rect.h, 1.0f));

		glm::mat4 mvp = projection * model;
		shader.SetMatrix4f("mvp", mvp);
		shader.SetVec3f("color", rect.color);

		glBindVertexArray(rectVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
	}

	void RenderLine(float originX, float originY, float endX, float endY, Shader& shader, glm::vec3 color = { 1, 1, 1 }) {
		shader.Use();
		shader.SetMatrix4f("mvp", projection);
		shader.SetVec3f("color", color);

		glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
		float data[] = {
			originX, originY,
			endX, endY
		};
		glBufferSubData(GL_ARRAY_BUFFER, 0, LINE_BUFFER_SIZE, data);

		glBindVertexArray(lineVAO);
		glDrawArrays(GL_LINES, 0, 2);
	}
};