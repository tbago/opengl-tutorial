#include "vertex_buffer.h"
#include "renderer.h"
#include <glfw/glfw3.h>

VertexBuffer::VertexBuffer(const void *data, const unsigned int size) {
    GLCall(glGenBuffers(1, &renderer_id_));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, renderer_id_));
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data,GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer() {
    GLCall(glDeleteBuffers(1, &renderer_id_));
}

void VertexBuffer::Bind() const {
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, renderer_id_));
}

void VertexBuffer::Unbind() const {
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}


